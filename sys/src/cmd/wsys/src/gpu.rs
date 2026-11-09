// SPDX-License-Identifier: MIT
//
// Clients' GPU buffers (gpu(4), ../9front-ui stage 2): a client exports its
// buffer from its own context (ctx ctl `export` gives a token) and names it to
// wsys by the token (window ctl `gpubuffer`). wsys imports it into a context of
// its own and maps it where the processor sees it, and reads the pixels there:
// the client copies nothing. The context is made when the first such buffer
// comes, on the first device (/dev/gpu/0): aux/virtiogpu's (venus: a blob,
// in the device's host visible memory, whose segments stay attached while
// wsys runs), aux/amdgpu's (a GEM object, a segment of its own each) or
// aux/i915's (a GEM object, its pages a segment of its own).

use crate::p9::{Fd, ORDWR, Seg};
use std::collections::HashMap;
use std::io;
use std::rc::Rc;

#[derive(Clone, Copy, PartialEq)]
enum Drv {
    Virtio, // aux/virtiogpu: import gives "res r size", unref lets go
    Amd,    // aux/amdgpu: import gives "import handle size", close lets go
    Intel,  // aux/i915: import gives "import id seg va size", close lets go
}

pub struct Gpu {
    ctl: Rc<Fd>,
    drv: Drv,
    ctx: Option<u32>, // the context's number (clone's "ctx n"): its ctl for another fid
    bars: HashMap<String, Seg>, // virtio: the host visible memory, by segment class
}

/// a buffer imported into wsys's context: released there when dropped
pub struct GpuMem {
    ctl: Rc<Fd>,
    res: u32,         // the resource (virtio) or the object's handle (amd)
    seg: Option<Seg>, // amd: the object's own segment
    va: usize,
    pub len: usize,
    copy: Vec<u8>, // what read read last
    // Intel: the object may be uncached (i915 makes one the display shows so: the
    // overlay plane), the GPU writes past the processor's cache: read gets fresh
    // lines (clflush first, as Linux before a read of such an object)
    flush: bool,
}

impl GpuMem {
    /// the processor sees it (else only the display does: in VRAM, on the overlay plane)
    pub fn mapped(&self) -> bool {
        self.va != 0
    }

    /// its handle in wsys' context (aux/amdgpu: the object's; virtio: the resource)
    pub fn handle(&self) -> u32 {
        self.res
    }

    /// n bytes from offset off, read into wsys's memory: with SSE4.1's
    /// streaming loads where there are any, as Mesa's plan9 WSI reads them (the
    /// host visible memory of venus is write-combined on the host; plain loads
    /// are many times slower)
    pub fn read(&mut self, off: usize, n: usize) -> &[u8] {
        let n = n.min(self.len.saturating_sub(off));
        self.copy.resize(n, 0);
        let src = (self.va + off) as *const u8;
        #[cfg(target_arch = "x86_64")]
        if self.flush {
            unsafe { flushlines(src, n) };
        }
        #[cfg(target_arch = "x86_64")]
        {
            if std::arch::is_x86_feature_detected!("sse4.1") && src as usize % 16 == 0 {
                unsafe { stream_copy(self.copy.as_mut_ptr(), src, n) };
                return &self.copy;
            }
        }
        unsafe { std::ptr::copy_nonoverlapping(src, self.copy.as_mut_ptr(), n) };
        &self.copy
    }
}

/// the cache lines of n bytes at p out of the processor's caches (clflush): the
/// next loads come from memory
#[cfg(target_arch = "x86_64")]
unsafe fn flushlines(p: *const u8, n: usize) {
    use std::arch::x86_64::{_mm_clflush, _mm_mfence};
    let mut a = p as usize & !63;
    let end = p as usize + n;
    unsafe {
        _mm_mfence();
        while a < end {
            _mm_clflush(a as *const u8);
            a += 64;
        }
        _mm_mfence();
    }
}

/// n bytes from src (16-byte aligned) to dst with streaming loads (movntdqa)
#[cfg(target_arch = "x86_64")]
#[target_feature(enable = "sse4.1")]
unsafe fn stream_copy(dst: *mut u8, src: *const u8, n: usize) {
    use std::arch::x86_64::{__m128i, _mm_storeu_si128, _mm_stream_load_si128};
    let mut i = 0;
    unsafe {
        while i + 64 <= n {
            let a = _mm_stream_load_si128(src.add(i) as *const __m128i);
            let b = _mm_stream_load_si128(src.add(i + 16) as *const __m128i);
            let c = _mm_stream_load_si128(src.add(i + 32) as *const __m128i);
            let d = _mm_stream_load_si128(src.add(i + 48) as *const __m128i);
            _mm_storeu_si128(dst.add(i) as *mut __m128i, a);
            _mm_storeu_si128(dst.add(i + 16) as *mut __m128i, b);
            _mm_storeu_si128(dst.add(i + 32) as *mut __m128i, c);
            _mm_storeu_si128(dst.add(i + 48) as *mut __m128i, d);
            i += 64;
        }
        std::ptr::copy_nonoverlapping(src.add(i), dst.add(i), n - i);
    }
}

impl Drop for GpuMem {
    fn drop(&mut self) {
        // the view first, then the object
        self.seg = None;
        // close: gpu(4)'s word of every driver (virtio's unref, amdgpu's GEM_CLOSE)
        let _ = self.ctl.write(format!("close {}", self.res).as_bytes());
    }
}

fn bad(what: &str, reply: &str) -> io::Error {
    io::Error::other(format!("gpu: {what}: {}", reply.trim()))
}

impl Gpu {
    /// a context of the first device's (/dev/gpu/0); on virtio-gpu for
    /// venus' capability set (the clients' buffers are venus')
    pub fn open() -> io::Result<Gpu> {
        let status = std::fs::read_to_string("/dev/gpu/0/status")?;
        let drv = match status.lines().next().unwrap_or("") {
            l if l.starts_with("device virtio-gpu") => Drv::Virtio,
            l if l.starts_with("device amd") => Drv::Amd,
            l if l.starts_with("device intel") => Drv::Intel,
            l => return Err(io::Error::other(format!("gpu: no buffers of {l:?} yet"))),
        };
        let ctl = Fd::open("/dev/gpu/0/clone", ORDWR)?;
        let mut b = [0u8; 64];
        let n = ctl.pread(&mut b, 0).unwrap_or(0);
        let ctx = String::from_utf8_lossy(&b[..n]).split_whitespace().nth(1).and_then(|x| x.parse::<u32>().ok());
        let g = Gpu { ctl: Rc::new(ctl), drv, ctx, bars: HashMap::new() };
        if drv == Drv::Virtio {
            g.req("init 4")?;
        }
        Ok(g)
    }

    /// the context's ctl, for a fid of another thread's (the overlay's: it waits for the
    /// display); aux/amdgpu and aux/i915 show a client's window on an overlay plane
    /// (gpu(4) overlay; virtio-gpu has none)
    pub fn overlay_ctl(&self) -> Option<String> {
        match (self.drv, self.ctx) {
            (Drv::Amd | Drv::Intel, Some(n)) => Some(format!("/dev/gpu/0/ctx/{n}/ctl")),
            _ => None,
        }
    }

    /// a request and its reply
    fn req(&self, r: &str) -> io::Result<String> {
        self.ctl.write1(r.as_bytes()).map_err(|e| io::Error::other(format!("gpu: {r}: {e}")))?;
        let mut b = [0u8; 256];
        let n = self.ctl.pread(&mut b, 0)?;
        Ok(String::from_utf8_lossy(&b[..n]).into_owned())
    }

    /// the buffer token names, at least size bytes, mapped here
    pub fn import(&mut self, token: &str, size: usize) -> io::Result<GpuMem> {
        if token.is_empty() || !token.bytes().all(|c| c.is_ascii_hexdigit()) {
            return Err(io::Error::other("gpu: bad token"));
        }
        if self.drv == Drv::Amd {
            return self.importamd(token, size);
        }
        if self.drv == Drv::Intel {
            return self.importintel(token, size);
        }
        // res r size [seg name va]
        let rep = self.req(&format!("import {token}"))?;
        let f: Vec<&str> = rep.split_whitespace().collect();
        let (Some(&"res"), Some(res), Some(rsize)) = (f.first(), f.get(1).and_then(|x| x.parse::<u32>().ok()), f.get(2).and_then(|x| x.parse::<usize>().ok())) else {
            return Err(bad("import", &rep));
        };
        // from here on dropping m lets go of it again
        let mut m = GpuMem { ctl: self.ctl.clone(), res, seg: None, va: 0, len: 0, copy: vec![], flush: false };
        if rsize < size {
            return Err(io::Error::other(format!("gpu: buffer of {rsize} bytes, {size} wanted")));
        }
        // map r seg barsize offset size info
        let rep = self.req(&format!("map {res}"))?;
        let f: Vec<&str> = rep.split_whitespace().collect();
        let hex = |x: &str| usize::from_str_radix(x.trim_start_matches("0x"), 16).ok();
        let (Some(&"map"), Some(&class), Some(barsize), Some(off)) = (f.first(), f.get(2), f.get(3).and_then(|x| hex(x)), f.get(4).and_then(|x| hex(x))) else {
            return Err(bad("map", &rep));
        };
        if !self.bars.contains_key(class) {
            self.bars.insert(class.to_string(), Seg::phys(class, barsize)?);
        }
        let bar = &self.bars[class];
        if off.checked_add(size).is_none_or(|e| e > bar.len) {
            return Err(bad("map: outside the bar", &rep));
        }
        m.va = bar.va + off;
        m.len = size;
        Ok(m)
    }

    /// aux/i915: import token gives "import id seg va size", the object's pages
    /// as a segment of their own (in system memory: the processor reads them
    /// as the GPU wrote them, the LLC keeps both coherent)
    fn importintel(&mut self, token: &str, size: usize) -> io::Result<GpuMem> {
        let rep = self.req(&format!("import {token}"))?;
        let f: Vec<&str> = rep.split_whitespace().collect();
        let hex = |x: &str| usize::from_str_radix(x.trim_start_matches("0x"), 16).ok();
        let (Some(&"import"), Some(id), Some(&seg), Some(osize)) =
            (f.first(), f.get(1).and_then(|x| x.parse::<u32>().ok()), f.get(2), f.get(4).and_then(|x| hex(x)))
        else {
            return Err(bad("import", &rep));
        };
        let mut m = GpuMem { ctl: self.ctl.clone(), res: id, seg: None, va: 0, len: 0, copy: vec![], flush: false };
        if osize < size {
            return Err(io::Error::other(format!("gpu: buffer of {osize} bytes, {size} wanted")));
        }
        let s = Seg::phys(seg, osize)?;
        m.va = s.va;
        m.len = size;
        m.seg = Some(s);
        m.flush = true;
        Ok(m)
    }

    /// aux/amdgpu: import token gives "import handle size", map handle "map
    /// handle seg va size", a segment of the object's own
    fn importamd(&mut self, token: &str, size: usize) -> io::Result<GpuMem> {
        let rep = self.req(&format!("import {token}"))?;
        let f: Vec<&str> = rep.split_whitespace().collect();
        let (Some(&"import"), Some(h), Some(osize)) = (f.first(), f.get(1).and_then(|x| x.parse::<u32>().ok()), f.get(2).and_then(|x| x.parse::<usize>().ok())) else {
            return Err(bad("import", &rep));
        };
        let mut m = GpuMem { ctl: self.ctl.clone(), res: h, seg: None, va: 0, len: 0, copy: vec![], flush: false };
        if osize < size {
            return Err(io::Error::other(format!("gpu: buffer of {osize} bytes, {size} wanted")));
        }
        // in VRAM not in one piece where BAR 0 shows it (the WSI's images for the overlay
        // plane, variant b): no view for the processor, the display shows it
        if !self.mapamd(&mut m, size)? {
            m.len = size;
        }
        Ok(m)
    }

    /// aux/amdgpu: map handle gives "map handle seg va size", a segment of the
    /// object's own; false: the driver gives the processor no view of it
    fn mapamd(&self, m: &mut GpuMem, size: usize) -> io::Result<bool> {
        let Ok(rep) = self.req(&format!("map {}", m.res)) else {
            return Ok(false);
        };
        let f: Vec<&str> = rep.split_whitespace().collect();
        let hex = |x: &str| usize::from_str_radix(x.trim_start_matches("0x"), 16).ok();
        let (Some(&"map"), Some(&class), Some(len)) = (f.first(), f.get(2), f.get(4).and_then(|x| hex(x))) else {
            return Err(bad("map", &rep));
        };
        if len < size {
            return Err(bad("map: smaller than the buffer", &rep));
        }
        let seg = Seg::phys(class, len)?;
        m.va = seg.va;
        m.len = size;
        m.seg = Some(seg);
        Ok(true)
    }

    /// a copy of b made now where the processor sees it (aux/amdgpu's readback: the
    /// copy engine copies an object in VRAM wsys cannot map into a new one in GTT);
    /// let go of when dropped
    pub fn readback(&self, b: &GpuMem) -> io::Result<GpuMem> {
        if self.drv != Drv::Amd {
            return Err(io::Error::other("gpu: no readback but aux/amdgpu's"));
        }
        let rep = self.req(&format!("readback {}", b.res))?;
        let f: Vec<&str> = rep.split_whitespace().collect();
        let (Some(&"readback"), Some(h), Some(osize)) = (f.first(), f.get(1).and_then(|x| x.parse::<u32>().ok()), f.get(2).and_then(|x| x.parse::<usize>().ok())) else {
            return Err(bad("readback", &rep));
        };
        let mut m = GpuMem { ctl: self.ctl.clone(), res: h, seg: None, va: 0, len: 0, copy: vec![], flush: false };
        if osize < b.len {
            return Err(bad("readback: smaller than the buffer", &rep));
        }
        if !self.mapamd(&mut m, b.len)? {
            return Err(io::Error::other("gpu: readback: no view of the copy"));
        }
        Ok(m)
    }
}
