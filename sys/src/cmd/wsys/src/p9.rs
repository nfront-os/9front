// SPDX-License-Identifier: MIT
//
// The few Plan 9 system calls wsys needs that std does not offer, from 9front's
// own C library as Rust's std for plan9 links it (libp9.a: its names are
// __p9_*). Only these: the libc crate of ../9front-rust stays out of the tree.

use std::ffi::{CString, c_char, c_int, c_long, c_uint, c_void};
use std::io;

pub const OREAD: c_int = 0;
pub const OWRITE: c_int = 1;
pub const ORDWR: c_int = 2;
/// close on exec: what wsys opens stays out of the processes it starts (else they
/// would hold the mouse, the keyboard and wsys' end of its pipe after it exits)
const OCEXEC: c_int = 32;

unsafe extern "C" {
    #[link_name = "__p9_create"]
    fn p9create(file: *const c_char, mode: c_int, perm: c_uint) -> c_int;
    #[link_name = "__p9_open"]
    fn p9open(file: *const c_char, mode: c_int) -> c_int;
    #[link_name = "__p9_close"]
    fn p9close(fd: c_int) -> c_int;
    #[link_name = "__p9_read"]
    fn p9read(fd: c_int, buf: *mut c_void, n: c_long) -> c_long;
    #[link_name = "__p9_write"]
    fn p9write(fd: c_int, buf: *const c_void, n: c_long) -> c_long;
    #[link_name = "__p9_pread"]
    fn p9pread(fd: c_int, buf: *mut c_void, n: c_long, off: i64) -> c_long;
    #[link_name = "__p9_pipe"]
    fn p9pipe(fd: *mut c_int) -> c_int;
    #[link_name = "__p9_remove"]
    fn p9remove(file: *const c_char) -> c_int;
    #[link_name = "__p9_segattach"]
    fn p9segattach(attr: c_int, class: *const c_char, va: *mut c_void, len: c_uint) -> *mut c_void;
    #[link_name = "__p9_segdetach"]
    fn p9segdetach(addr: *mut c_void) -> c_int;
    #[link_name = "__p9_rfork"]
    fn p9rfork(flags: c_int) -> c_int;
    #[link_name = "__p9_rerrstr"]
    fn p9rerrstr(buf: *mut c_char, n: c_uint);
    #[link_name = "__p9_stat"]
    fn p9stat(file: *const c_char, edir: *mut u8, nedir: c_int) -> c_int;
}

/// which file it is: stat(5)'s type, dev and qid (two names of one file give
/// the same)
pub fn fileid(file: &str) -> Option<[u8; 19]> {
    let name = std::ffi::CString::new(file).ok()?;
    let mut b = [0u8; 512];
    let n = unsafe { p9stat(name.as_ptr(), b.as_mut_ptr(), b.len() as c_int) };
    (n >= 21).then(|| b[2..21].try_into().ok()).flatten()
}

/// the process's error string as an io::Error
pub fn err() -> io::Error {
    let mut b = [0u8; 256];
    unsafe { p9rerrstr(b.as_mut_ptr() as *mut c_char, b.len() as c_uint) };
    let n = b.iter().position(|&c| c == 0).unwrap_or(b.len());
    io::Error::other(String::from_utf8_lossy(&b[..n]).into_owned())
}

fn cs(s: &str) -> CString {
    CString::new(s).expect("nul in name")
}

/// A file descriptor of Plan 9's: plain reads and writes, each one a system call
/// (9P messages and draw(3) messages must not be split or joined).
pub struct Fd(pub c_int);

impl Fd {
    pub fn open(name: &str, mode: c_int) -> io::Result<Fd> {
        let fd = unsafe { p9open(cs(name).as_ptr(), mode | OCEXEC) };
        if fd < 0 { Err(err()) } else { Ok(Fd(fd)) }
    }

    pub fn create(name: &str, mode: c_int, perm: c_uint) -> io::Result<Fd> {
        let fd = unsafe { p9create(cs(name).as_ptr(), mode | OCEXEC, perm) };
        if fd < 0 { Err(err()) } else { Ok(Fd(fd)) }
    }

    // an empty slice's pointer is dangling (0x1 for bytes), and Plan 9's kernel
    // checks the address even for no bytes ("bad address in syscall"): one of
    // its own then
    pub fn read(&self, buf: &mut [u8]) -> io::Result<usize> {
        let mut none = [0u8; 1];
        let p = if buf.is_empty() { none.as_mut_ptr() } else { buf.as_mut_ptr() };
        let n = unsafe { p9read(self.0, p as *mut c_void, buf.len() as c_long) };
        if n < 0 { Err(err()) } else { Ok(n as usize) }
    }

    /// a read at offset off (a ctl file's reply: from its start)
    pub fn pread(&self, buf: &mut [u8], off: i64) -> io::Result<usize> {
        let mut none = [0u8; 1];
        let p = if buf.is_empty() { none.as_mut_ptr() } else { buf.as_mut_ptr() };
        let n = unsafe { p9pread(self.0, p as *mut c_void, buf.len() as c_long, off) };
        if n < 0 { Err(err()) } else { Ok(n as usize) }
    }

    pub fn write(&self, buf: &[u8]) -> io::Result<usize> {
        let none = [0u8; 1];
        let p = if buf.is_empty() { none.as_ptr() } else { buf.as_ptr() };
        let n = unsafe { p9write(self.0, p as *const c_void, buf.len() as c_long) };
        if n < 0 { Err(err()) } else { Ok(n as usize) }
    }

    /// all of buf in one write, or an error
    pub fn write1(&self, buf: &[u8]) -> io::Result<()> {
        let n = self.write(buf)?;
        if n != buf.len() {
            return Err(io::Error::other(format!("short write {n} of {}", buf.len())));
        }
        Ok(())
    }

    /// leave the descriptor open for good (posted to /srv, kept by a child)
    pub fn leak(self) -> c_int {
        let fd = self.0;
        std::mem::forget(self);
        fd
    }
}

impl Drop for Fd {
    fn drop(&mut self) {
        unsafe { p9close(self.0) };
    }
}

pub const RFNOTEG: c_int = 8;

pub fn rfork(flags: c_int) -> io::Result<()> {
    if unsafe { p9rfork(flags) } < 0 { Err(err()) } else { Ok(()) }
}

pub fn pipe() -> io::Result<(Fd, Fd)> {
    let mut p = [0 as c_int; 2];
    if unsafe { p9pipe(p.as_mut_ptr()) } < 0 {
        return Err(err());
    }
    let (a, b) = (Fd(p[0]), Fd(p[1]));
    // pipe(2) has no close on exec: the ends again through dup(3), with it
    Ok((Fd::open(&format!("#d/{}", a.0), ORDWR)?, Fd::open(&format!("#d/{}", b.0), ORDWR)?))
}

pub fn remove(name: &str) -> io::Result<()> {
    if unsafe { p9remove(cs(name).as_ptr()) } < 0 { Err(err()) } else { Ok(()) }
}

/// A client's global segment of segment(3) (#g), attached here by its name. It
/// lies at the address its maker gave it (segment(3): the same in every process);
/// wsys checks that it is inside the range it gave the window.
pub struct Seg {
    pub va: usize,
    pub len: usize,
}

/// The end of a process's user addresses (USTKTOP of 9front's mem.h); 0 where
/// not known here: no regions, no windows.
#[cfg(target_arch = "x86_64")]
const USTKTOP: usize = 0x0000_7fff_ffff_f000;
#[cfg(target_arch = "aarch64")]
const USTKTOP: usize = (((1usize << 36) - 1) >> 1) - 0xFFFF;
// 386 (pc/mem.h): VMAP - BY2PG, VMAP 256 MB below KZERO 0xF0000000
#[cfg(target_arch = "x86")]
const USTKTOP: usize = 0xE000_0000 - 0x1000;
#[cfg(not(any(target_arch = "x86_64", target_arch = "aarch64", target_arch = "x86")))]
const USTKTOP: usize = 0;

/// The region of addresses the kernel gives segments made with "va 0 len"
/// (segment(3); 9front's port/portdat.h, SHAREDBASE and SHAREDTOP): unique in
/// the system, so a client's segment there cannot collide with another's.
pub const fn shared_region() -> (usize, usize) {
    const MB2: usize = 2 * 1024 * 1024 - 1;
    let base = (USTKTOP / 2) & !MB2;
    (base, base + ((USTKTOP / 16) & !MB2))
}

/// The windows' ranges for their clients' segments: slot i is base + i*size,
/// (base, size, slots). A quarter of the addresses from 5/8 of USTKTOP on:
/// above the shared region (it ends at 9/16), below the stacks. amd64: 4 GB
/// slots from 0x5000_0000_0000; arm64 (32 GB of addresses): 256 MB ones; 386
/// (3.5 GB): 27 of 32 MB from 0x8c00_0000 (std's thread stacks go down from
/// 2 GB, the shared region is at 1.75 GB).
pub const fn window_region() -> (usize, usize, usize) {
    #[cfg(target_arch = "aarch64")]
    const SIZE: usize = 0x1000_0000;
    #[cfg(target_arch = "x86")]
    const SIZE: usize = 0x200_0000;
    #[cfg(not(any(target_arch = "aarch64", target_arch = "x86")))]
    const SIZE: usize = 0x1_0000_0000;
    let base = (USTKTOP / 8 * 5 + SIZE - 1) & !(SIZE - 1);
    (base, SIZE, USTKTOP / 4 / SIZE)
}

impl Seg {
    /// where segment name is (its ctl: "va 0xaddr 0xlen"), if it is a plain
    /// shared one
    pub fn find(name: &str) -> io::Result<(usize, usize)> {
        if name.is_empty() || name.contains('/') {
            return Err(io::Error::other("bad segment name"));
        }
        let ctl = Fd::open(&format!("#g/{name}/ctl"), OREAD)?;
        let mut b = [0u8; 128];
        let n = ctl.read(&mut b)?;
        let s = String::from_utf8_lossy(&b[..n]);
        let f: Vec<&str> = s.split_whitespace().collect();
        let num = |x: &str| usize::from_str_radix(x.trim_start_matches("0x"), 16).ok();
        let (Some(va), Some(len)) = (f.get(1).and_then(|x| num(x)), f.get(2).and_then(|x| num(x))) else {
            return Err(io::Error::other(format!("segment {name}: ctl {s:?}")));
        };
        if f.len() > 3 {
            return Err(io::Error::other(format!("segment {name}: not a plain shared segment")));
        }
        Ok((va, len))
    }

    /// segment name attached at its address, (va, len) as find gave them
    pub fn attach(name: &str, (va, len): (usize, usize)) -> io::Result<Seg> {
        let p = unsafe { p9segattach(0, cs(name).as_ptr(), std::ptr::null_mut(), len as c_uint) };
        if p as isize == -1 {
            return Err(err());
        }
        let s = Seg { va: p as usize, len };
        if s.va != va {
            return Err(io::Error::other(format!("segment {name} at {:#x}, its ctl says {va:#x}", s.va)));
        }
        Ok(s)
    }

    /// segment name as this proc has it already: inherited from the proc that
    /// attached it before starting this one (std's threads are procs; rfork
    /// RFMEM shares the segments there are), else attached
    #[cfg(feature = "gpu")] // (comp.rs)
    pub fn attach_or_have(name: &str, (va, len): (usize, usize)) -> io::Result<Seg> {
        match Seg::attach(name, (va, len)) {
            Ok(s) => Ok(s),
            Err(e) if e.to_string().contains("overlap") => Ok(Seg { va, len }),
            Err(e) => Err(e),
        }
    }

    /// len bytes of a segment of the kernel's of class (segment(3)'s physical
    /// segments: a device's memory, gpu(3)), where the kernel puts it
    pub fn phys(class: &str, len: usize) -> io::Result<Seg> {
        if class.is_empty() || class.contains('/') {
            return Err(io::Error::other("bad segment class"));
        }
        let p = unsafe { p9segattach(0, cs(class).as_ptr(), std::ptr::null_mut(), len as c_uint) };
        if p as isize == -1 {
            return Err(err());
        }
        Ok(Seg { va: p as usize, len })
    }

    pub fn bytes(&self) -> &[u8] {
        unsafe { std::slice::from_raw_parts(self.va as *const u8, self.len) }
    }
}

impl Drop for Seg {
    fn drop(&mut self) {
        // the client's: wsys only lets go of it (gone when nobody has it attached)
        unsafe { p9segdetach(self.va as *mut c_void) };
    }
}
