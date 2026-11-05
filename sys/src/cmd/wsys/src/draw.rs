// SPDX-License-Identifier: MIT
//
// A client of draw(3), the protocol libdraw speaks (its messages as init.c's
// vdrawcmd writes them, little-endian): wsys's processor backend draws through
// it, so devdraw (memdraw) composes and the screen is any draw(3) has: VESA,
// a vga driver, a GPU driver's framebuffer, or a rio window.

use crate::p9::{Fd, ORDWR};
use std::io;

pub const XRGB32: u32 = 0x6808_1828; // CHAN4(CIgnore,8, CRed,8, CGreen,8, CBlue,8)
const GREY1: u32 = 0x31;
const BORDERWIDTH: i32 = 4; // rio's, around the window image a rio client draws in
// devdraw takes messages of any size, libdraw sends at most its buffer (8000
// bytes without an iounit); the same here
const MAXMSG: usize = 8000;

#[derive(Clone, Copy, Debug, Default, PartialEq)]
pub struct Rect {
    pub x0: i32,
    pub y0: i32,
    pub x1: i32,
    pub y1: i32,
}

impl Rect {
    pub fn new(x0: i32, y0: i32, x1: i32, y1: i32) -> Rect {
        Rect { x0, y0, x1, y1 }
    }
    pub fn dx(&self) -> i32 {
        self.x1 - self.x0
    }
    pub fn dy(&self) -> i32 {
        self.y1 - self.y0
    }
    pub fn empty(&self) -> bool {
        self.x0 >= self.x1 || self.y0 >= self.y1
    }
    pub fn clip(&self, o: &Rect) -> Rect {
        Rect::new(self.x0.max(o.x0), self.y0.max(o.y0), self.x1.min(o.x1), self.y1.min(o.y1))
    }
    pub fn add(&self, x: i32, y: i32) -> Rect {
        Rect::new(self.x0 + x, self.y0 + y, self.x1 + x, self.y1 + y)
    }
    pub fn inset(&self, n: i32) -> Rect {
        Rect::new(self.x0 + n, self.y0 + n, self.x1 - n, self.y1 - n)
    }
    pub fn contains(&self, x: i32, y: i32) -> bool {
        x >= self.x0 && x < self.x1 && y >= self.y0 && y < self.y1
    }
    pub fn union(&self, o: &Rect) -> Rect {
        if self.empty() {
            return *o;
        }
        if o.empty() {
            return *self;
        }
        Rect::new(self.x0.min(o.x0), self.y0.min(o.y0), self.x1.max(o.x1), self.y1.max(o.y1))
    }
}

pub struct Display {
    _ctl: Fd, // the connection lives while /dev/draw/new is open
    data: Fd,
    buf: Vec<u8>,
    pub screen: u32,  // image id drawn on: 0, or the rio window's
    pub r: Rect,      // its rectangle, inside rio's border
    next: u32,
    pub opaque: u32,
}

fn info(b: &[u8]) -> io::Result<Vec<String>> {
    if b.len() < 144 {
        return Err(io::Error::other(format!("draw: short info {}", b.len())));
    }
    Ok(b[..144].chunks(12).map(|f| String::from_utf8_lossy(f).trim().to_string()).collect())
}

fn int(s: &str) -> i32 {
    s.parse().unwrap_or(0)
}

impl Display {
    /// the screen, or the window of /dev/winname when there is one (wsys in rio)
    pub fn open() -> io::Result<Display> {
        let ctl = Fd::open("/dev/draw/new", ORDWR)?;
        let mut b = [0u8; 12 * 12 + 1];
        let n = ctl.read(&mut b)?;
        let f = info(&b[..n])?;
        let data = Fd::open(&format!("/dev/draw/{}/data", f[0]), ORDWR)?;
        let mut d = Display {
            _ctl: ctl,
            data,
            buf: Vec::with_capacity(MAXMSG),
            screen: 0,
            r: Rect::new(int(&f[4]), int(&f[5]), int(&f[6]), int(&f[7])),
            next: 16,
            opaque: 0,
        };
        if let Ok(w) = std::fs::read_to_string("/dev/winname") {
            let w = w.trim().to_string();
            if !w.is_empty() {
                let id = d.id();
                let mut m = vec![b'n'];
                m.extend_from_slice(&id.to_le_bytes());
                m.push(w.len() as u8);
                m.extend_from_slice(w.as_bytes());
                d.cmd(&m)?;
                d.flush()?;
                let n = d._ctl.read(&mut b)?;
                let f = info(&b[..n])?;
                d.screen = id;
                d.r = Rect::new(int(&f[4]), int(&f[5]), int(&f[6]), int(&f[7]));
                if !w.starts_with("noborder.") {
                    d.r = d.r.inset(BORDERWIDTH);
                }
            }
        }
        d.opaque = d.alloc(Rect::new(0, 0, 1, 1), GREY1, true, 0xFFFF_FFFF)?;
        Ok(d)
    }

    fn id(&mut self) -> u32 {
        self.next += 1;
        self.next
    }

    /// append a message, writing out what is buffered when it would not fit
    fn cmd(&mut self, m: &[u8]) -> io::Result<()> {
        if self.buf.len() + m.len() > MAXMSG {
            self.flushbuf()?;
        }
        if m.len() > MAXMSG {
            return self.data.write1(m);
        }
        self.buf.extend_from_slice(m);
        Ok(())
    }

    fn flushbuf(&mut self) -> io::Result<()> {
        if !self.buf.is_empty() {
            let r = self.data.write1(&self.buf);
            self.buf.clear();
            r?;
        }
        Ok(())
    }

    /// 'v': what is drawn reaches the screen
    pub fn flush(&mut self) -> io::Result<()> {
        self.buf.push(b'v');
        self.flushbuf()
    }

    fn rect(m: &mut Vec<u8>, r: &Rect) {
        for v in [r.x0, r.y0, r.x1, r.y1] {
            m.extend_from_slice(&v.to_le_bytes());
        }
    }

    /// 'b': a new image; repl images of 1x1 are colours
    pub fn alloc(&mut self, r: Rect, chan: u32, repl: bool, col: u32) -> io::Result<u32> {
        let id = self.id();
        let mut m = vec![b'b'];
        m.extend_from_slice(&id.to_le_bytes());
        m.extend_from_slice(&0u32.to_le_bytes()); // screen id: none
        m.push(0); // refresh
        m.extend_from_slice(&chan.to_le_bytes());
        m.push(repl as u8);
        Self::rect(&mut m, &r);
        let clip = if repl { Rect::new(-0x3FFF_FFFF, -0x3FFF_FFFF, 0x3FFF_FFFF, 0x3FFF_FFFF) } else { r };
        Self::rect(&mut m, &clip);
        m.extend_from_slice(&col.to_le_bytes());
        self.cmd(&m)?;
        Ok(id)
    }

    /// a colour: 0xRRGGBB
    pub fn color(&mut self, rgb: u32) -> io::Result<u32> {
        self.alloc(Rect::new(0, 0, 1, 1), XRGB32, true, rgb << 8 | 0xFF)
    }

    /// 'f'
    pub fn free(&mut self, id: u32) -> io::Result<()> {
        let mut m = vec![b'f'];
        m.extend_from_slice(&id.to_le_bytes());
        self.cmd(&m)
    }

    /// 'd': src at sp drawn into r of dst, through the opaque mask
    pub fn draw(&mut self, dst: u32, r: &Rect, src: u32, sx: i32, sy: i32) -> io::Result<()> {
        if r.empty() {
            return Ok(());
        }
        let mut m = vec![b'd'];
        for id in [dst, src, self.opaque] {
            m.extend_from_slice(&id.to_le_bytes());
        }
        Self::rect(&mut m, r);
        for v in [sx, sy, 0, 0] {
            m.extend_from_slice(&v.to_le_bytes());
        }
        self.cmd(&m)
    }

    /// 'y': rectangle r of image id from pixels laid out with stride (XRGB32), in
    /// messages of at most MAXMSG bytes: bands of rows, or pieces of a row
    pub fn load(&mut self, id: u32, r: &Rect, px: &[u8], stride: usize) -> io::Result<()> {
        let room = MAXMSG - 1 - 4 - 16;
        let bpl = r.dx() as usize * 4;
        let mut y = r.y0;
        while y < r.y1 {
            if bpl <= room {
                let rows = ((room / bpl) as i32).min(r.y1 - y);
                let band = Rect::new(r.x0, y, r.x1, y + rows);
                let mut m = vec![b'y'];
                m.extend_from_slice(&id.to_le_bytes());
                Self::rect(&mut m, &band);
                for yy in y..y + rows {
                    let o = yy as usize * stride + r.x0 as usize * 4;
                    m.extend_from_slice(&px[o..o + bpl]);
                }
                self.cmd(&m)?;
                y += rows;
            } else {
                let w = (room / 4) as i32;
                let mut x = r.x0;
                while x < r.x1 {
                    let x1 = (x + w).min(r.x1);
                    let mut m = vec![b'y'];
                    m.extend_from_slice(&id.to_le_bytes());
                    Self::rect(&mut m, &Rect::new(x, y, x1, y + 1));
                    let o = y as usize * stride + x as usize * 4;
                    m.extend_from_slice(&px[o..o + (x1 - x) as usize * 4]);
                    self.cmd(&m)?;
                    x = x1;
                }
                y += 1;
            }
        }
        Ok(())
    }
}
