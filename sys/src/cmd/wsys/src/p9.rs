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
pub const DMDIR: c_uint = 0x8000_0000;

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
    #[link_name = "__p9_pipe"]
    fn p9pipe(fd: *mut c_int) -> c_int;
    #[link_name = "__p9_remove"]
    fn p9remove(file: *const c_char) -> c_int;
    #[link_name = "__p9_segattach"]
    fn p9segattach(attr: c_int, class: *const c_char, va: *mut c_void, len: c_uint) -> *mut c_void;
    #[link_name = "__p9_segdetach"]
    fn p9segdetach(addr: *mut c_void) -> c_int;
    #[link_name = "__p9_rerrstr"]
    fn p9rerrstr(buf: *mut c_char, n: c_uint);
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
        let fd = unsafe { p9open(cs(name).as_ptr(), mode) };
        if fd < 0 { Err(err()) } else { Ok(Fd(fd)) }
    }

    pub fn create(name: &str, mode: c_int, perm: c_uint) -> io::Result<Fd> {
        let fd = unsafe { p9create(cs(name).as_ptr(), mode, perm) };
        if fd < 0 { Err(err()) } else { Ok(Fd(fd)) }
    }

    pub fn read(&self, buf: &mut [u8]) -> io::Result<usize> {
        let n = unsafe { p9read(self.0, buf.as_mut_ptr() as *mut c_void, buf.len() as c_long) };
        if n < 0 { Err(err()) } else { Ok(n as usize) }
    }

    pub fn write(&self, buf: &[u8]) -> io::Result<usize> {
        let n = unsafe { p9write(self.0, buf.as_ptr() as *const c_void, buf.len() as c_long) };
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

pub fn pipe() -> io::Result<(Fd, Fd)> {
    let mut p = [0 as c_int; 2];
    if unsafe { p9pipe(p.as_mut_ptr()) } < 0 {
        return Err(err());
    }
    Ok((Fd(p[0]), Fd(p[1])))
}

pub fn remove(name: &str) -> io::Result<()> {
    if unsafe { p9remove(cs(name).as_ptr()) } < 0 { Err(err()) } else { Ok(()) }
}

/// A global segment of segment(3) (#g), made by wsys at an address it picks,
/// attached here; the client attaches it by name and gets the same address.
pub struct Seg {
    pub name: String,
    pub va: usize,
    pub len: usize,
}

impl Seg {
    pub fn new(name: &str, va: usize, len: usize) -> io::Result<Seg> {
        let dir = format!("#g/{name}");
        Fd::create(&dir, OREAD, DMDIR | 0o600)?;
        let ctl = Fd::open(&format!("{dir}/ctl"), OWRITE).inspect_err(|_| {
            let _ = remove(&dir);
        })?;
        let s = Seg { name: name.to_string(), va, len };
        if let Err(e) = ctl.write1(format!("va {va:#x} {len:#x}").as_bytes()) {
            let _ = remove(&dir);
            return Err(e);
        }
        let p = unsafe { p9segattach(0, cs(name).as_ptr(), std::ptr::null_mut(), len as c_uint) };
        if p as isize == -1 || p as usize != va {
            let e = err();
            let _ = remove(&dir);
            return Err(e);
        }
        Ok(s)
    }

    pub fn bytes(&self) -> &[u8] {
        unsafe { std::slice::from_raw_parts(self.va as *const u8, self.len) }
    }
}

impl Drop for Seg {
    fn drop(&mut self) {
        // the segment lives on while a client has it attached
        unsafe { p9segdetach(self.va as *mut c_void) };
        let _ = remove(&format!("#g/{}", self.name));
    }
}
