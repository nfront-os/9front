// SPDX-License-Identifier: MIT
//
// draw(3) in wsys itself (../9front-ui PLAN stage 4c): devdraw, libmemdraw and
// libmemlayer built for APE (sys/src/ape/lib/devdraw: drawfs' devdraw.c behind
// core.c's calls) draw into a screen in a segment of #g of wsys' (wsys.screen.pid),
// which the compositor reads; wsys serves their files (draw/new, draw/n/...) to
// its clients and draws through them itself (draw.rs). One lock in the C core
// keeps devdraw to one caller at a time.

use crate::draw::Rect;
use crate::p9::{Fd, OWRITE, Seg};
use std::ffi::{CString, c_char, c_int, c_long, c_void};
use std::io;

// APE's builds, in the order they need each other; lib9, libfmt and libutf
// after them (APE's: print, werrstr)
#[link(name = "devdraw")]
#[link(name = "memlayer")]
#[link(name = "memdraw")]
#[link(name = "draw")]
#[link(name = "9")]
#[link(name = "fmt")]
#[link(name = "utf")]
unsafe extern "C" {
    fn drawcore_init(mem: *mut c_void, minx: c_int, miny: c_int, maxx: c_int, maxy: c_int, chan: *const c_char, err: *mut c_char, nerr: c_int) -> c_int;
    fn drawcore_width() -> c_int;
    fn drawcore_open(path: u64, mode: c_int, npath: *mut u64, err: *mut c_char, nerr: c_int) -> *mut c_void;
    fn drawcore_read(c: *mut c_void, buf: *mut c_void, n: c_long, off: i64, err: *mut c_char, nerr: c_int) -> c_long;
    fn drawcore_write(c: *mut c_void, buf: *const c_void, n: c_long, err: *mut c_char, nerr: c_int) -> c_long;
    fn drawcore_close(c: *mut c_void);
    fn drawcore_clientslot(id: c_int) -> c_int;
    fn drawcore_clientid(slot: c_int) -> c_int;
    fn drawcore_dirty(r: *mut c_int, max: c_int) -> c_int;
    fn drawcore_wake() -> c_int;
    fn drawcore_stats(buf: *mut c_char, n: c_int) -> c_int;
}

/// devdraw's file types: the low four bits of a qid's path, slot<<4 above them
pub const QNEW: u64 = 1;
pub const QCOLORMAP: u64 = 5;
pub const QCTL: u64 = 6;
pub const QDATA: u64 = 7;
pub const QREFRESH: u64 = 8;
pub const QSHIFT: u32 = 4;

fn errtext(b: &[u8]) -> String {
    let n = b.iter().position(|&c| c == 0).unwrap_or(b.len());
    String::from_utf8_lossy(&b[..n]).into_owned()
}

/// the screen: devdraw's, in a segment of #g
pub struct Core {
    pub seg: Seg,
    pub name: String, // the segment's: the compositor attaches it too
    pub r: Rect,
    pub stride: usize, // bytes a row
}

impl Core {
    /// devdraw on a screen of r, channels chan ("x8r8g8b8"); once in wsys
    pub fn start(r: Rect, chan: &str) -> io::Result<Core> {
        let name = format!("wsys.screen.{}", std::process::id());
        let depth = crate::draw::chandepth(crate::draw::strtochan(chan).ok_or_else(|| io::Error::other("bad channel"))?) as usize;
        // a row as devdraw lays it out (wordsperline), a word to spare
        let size = ((r.dx() as usize * depth + 31) / 32 + 1) * 4 * r.dy() as usize + 4096;
        // the segment: a directory of #g, its addresses where the kernel puts them
        Fd::create(&format!("#g/{name}"), 0, 0x8000_0000 | 0o700)?;
        Fd::open(&format!("#g/{name}/ctl"), OWRITE)?.write1(format!("va 0 {size}").as_bytes())?;
        let seg = Seg::attach(&name, Seg::find(&name)?)?;
        let mut e = [0u8; 128];
        let c = CString::new(chan).unwrap();
        if unsafe { drawcore_init(seg.va as *mut c_void, r.x0, r.y0, r.x1, r.y1, c.as_ptr(), e.as_mut_ptr() as *mut c_char, e.len() as c_int) } < 0 {
            return Err(io::Error::other(format!("devdraw: {}", errtext(&e))));
        }
        let stride = unsafe { drawcore_width() } as usize * 4;
        Ok(Core { seg, name, r, stride })
    }

    /// the screen's rectangles drawn on since the last call
    pub fn dirty(&self) -> Vec<Rect> {
        let mut b = [0 as c_int; 4 * 32];
        let mut v = vec![];
        loop {
            let n = unsafe { drawcore_dirty(b.as_mut_ptr(), 32) } as usize;
            v.extend((0..n).map(|i| Rect::new(b[4 * i], b[4 * i + 1], b[4 * i + 2], b[4 * i + 3])));
            if n < 32 {
                return v;
            }
        }
    }
}

/// what memdraw was asked to do so far (hwdraw's counts), devdraw's time
pub fn stats() -> String {
    let mut b = [0u8; 1024];
    let n = unsafe { drawcore_stats(b.as_mut_ptr() as *mut c_char, b.len() as c_int) } as usize;
    String::from_utf8_lossy(&b[..n]).into_owned()
}

impl Drop for Core {
    /// the segment's name goes (its memory with the last who has it attached)
    fn drop(&mut self) {
        let _ = crate::p9::remove(&format!("#g/{}", self.name));
    }
}

/// a refresh read may be answered now
pub fn wake() -> bool {
    unsafe { drawcore_wake() != 0 }
}

/// the slot (1...) of client id, 0 if none
pub fn clientslot(id: u32) -> u32 {
    unsafe { drawcore_clientslot(id as c_int) as u32 }
}

/// the id of the client in slot s (1...): None past the last, Some(0) an empty one
pub fn clientid(s: u32) -> Option<u32> {
    let id = unsafe { drawcore_clientid(s as c_int) };
    (id >= 0).then_some(id as u32)
}

/// an open file of devdraw's (a channel of the kernel's devdraw: its qid's path)
pub struct Chan {
    c: *mut c_void,
    pub path: u64,
}

pub enum Read {
    Data(usize),
    Wait, // refresh: nothing yet
}

impl Chan {
    /// path opened; new gives the new client's ctl
    pub fn open(path: u64, mode: u8) -> Result<Chan, String> {
        let mut e = [0u8; 128];
        let mut np = 0u64;
        let c = unsafe { drawcore_open(path, mode as c_int, &mut np, e.as_mut_ptr() as *mut c_char, e.len() as c_int) };
        if c.is_null() {
            return Err(errtext(&e));
        }
        Ok(Chan { c, path: np })
    }

    pub fn read(&self, buf: &mut [u8], off: u64) -> Result<Read, String> {
        let mut e = [0u8; 128];
        match unsafe { drawcore_read(self.c, buf.as_mut_ptr() as *mut c_void, buf.len() as c_long, off as i64, e.as_mut_ptr() as *mut c_char, e.len() as c_int) } {
            -2 => Ok(Read::Wait),
            n if n < 0 => Err(errtext(&e)),
            n => Ok(Read::Data(n as usize)),
        }
    }

    pub fn write(&self, buf: &[u8]) -> Result<usize, String> {
        let mut e = [0u8; 128];
        match unsafe { drawcore_write(self.c, buf.as_ptr() as *const c_void, buf.len() as c_long, e.as_mut_ptr() as *mut c_char, e.len() as c_int) } {
            n if n < 0 => Err(errtext(&e)),
            n => Ok(n as usize),
        }
    }
}

impl Drop for Chan {
    fn drop(&mut self) {
        unsafe { drawcore_close(self.c) };
    }
}
