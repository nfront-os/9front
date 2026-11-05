// SPDX-License-Identifier: MIT
//
// Clients' GPU buffers (gpu(4), ../9front-ui stage 2): a client exports its
// buffer from its own context (ctx ctl `export r` gives a token) and names it to
// wsys by the token (window ctl `gpubuffer`). wsys imports it into a context of
// its own and maps it where the processor sees it, the device's host visible
// memory (a segment of gpu(3)), and reads the pixels there: the client copies
// nothing. The context is made when the first such buffer comes; the device's
// segments stay attached while wsys runs (a handful at most).

use crate::p9::{Fd, ORDWR, Seg};
use std::collections::HashMap;
use std::io;
use std::rc::Rc;

pub struct Gpu {
    ctl: Rc<Fd>,
    bars: HashMap<String, Seg>, // the host visible memory, by segment class
}

/// a buffer imported into wsys's context: released there when dropped
pub struct GpuMem {
    ctl: Rc<Fd>,
    res: u32,
    va: usize,
    pub len: usize,
    copy: Vec<u8>, // what read read last
}

impl GpuMem {
    /// n bytes from offset off, read into wsys's memory: with SSE4.1's
    /// streaming loads where there are any, as Mesa's plan9 WSI reads them (the
    /// host visible memory of venus is write-combined on the host; plain loads
    /// are many times slower)
    pub fn read(&mut self, off: usize, n: usize) -> &[u8] {
        let n = n.min(self.len.saturating_sub(off));
        self.copy.resize(n, 0);
        let src = (self.va + off) as *const u8;
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
        let _ = self.ctl.write(format!("unref {}", self.res).as_bytes());
    }
}

fn bad(what: &str, reply: &str) -> io::Error {
    io::Error::other(format!("gpu: {what}: {}", reply.trim()))
}

impl Gpu {
    /// a context of the first device's (/dev/gpu/0), for venus' capability set:
    /// the clients' buffers are venus' (virtio-gpu) for now
    pub fn open() -> io::Result<Gpu> {
        let ctl = Fd::open("/dev/gpu/0/clone", ORDWR)?;
        let g = Gpu { ctl: Rc::new(ctl), bars: HashMap::new() };
        g.req("init 4")?;
        Ok(g)
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
        // res r size [seg name va]
        let rep = self.req(&format!("import {token}"))?;
        let f: Vec<&str> = rep.split_whitespace().collect();
        let (Some(&"res"), Some(res), Some(rsize)) = (f.first(), f.get(1).and_then(|x| x.parse::<u32>().ok()), f.get(2).and_then(|x| x.parse::<usize>().ok())) else {
            return Err(bad("import", &rep));
        };
        // from here on dropping m lets go of it again
        let mut m = GpuMem { ctl: self.ctl.clone(), res, va: 0, len: 0, copy: vec![] };
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
}
