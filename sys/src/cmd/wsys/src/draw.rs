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
}

/// a channel descriptor of draw(3) ("x8r8g8b8") as its number (strtochan(2))
pub fn strtochan(s: &str) -> Option<u32> {
    let b = s.as_bytes();
    if b.is_empty() || b.len() % 2 != 0 {
        return None;
    }
    let mut c = 0u32;
    for p in b.chunks(2) {
        let t = match p[0] {
            b'r' => 0,
            b'g' => 1,
            b'b' => 2,
            b'k' => 3,
            b'a' => 4,
            b'm' => 5,
            b'x' => 6,
            _ => return None,
        };
        let n = (p[1] as char).to_digit(10)?;
        c = c << 8 | (t << 4 | n);
    }
    Some(c)
}

pub const REFBACKUP: u8 = 0;

pub struct Display {
    _ctl: Fd, // the connection lives while /dev/draw/new is open
    data: Fd,
    buf: Vec<u8>,
    pub screen: u32,  // image id drawn on: 0, or the rio window's
    pub chan: u32,    // its channels: windows on it have them too
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
            chan: strtochan(&f[2]).unwrap_or(XRGB32),
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
                d.chan = strtochan(&f[2]).unwrap_or(XRGB32);
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

    /// 'A': a screen for windows on image, its background fill
    /// (screen ids are devdraw's, of all clients: as libdraw, try ids until one is free)
    pub fn allocscreen(&mut self, image: u32, fill: u32) -> io::Result<u32> {
        self.flushbuf()?;
        let mut last = None;
        for _ in 0..25 {
            let id = self.id();
            let mut m = vec![b'A'];
            for v in [id, image, fill] {
                m.extend_from_slice(&v.to_le_bytes());
            }
            m.push(0); // not public
            match self.data.write1(&m) {
                Ok(()) => return Ok(id),
                Err(e) => last = Some(e),
            }
        }
        Err(last.unwrap())
    }

    /// 'b' on a screen: a window (layer) at r, in screen coordinates; devdraw keeps
    /// what is hidden of it (Refbackup) and shows the screen's fill where none is
    pub fn allocwindow(&mut self, scr: u32, r: Rect, col: u32) -> io::Result<u32> {
        let id = self.id();
        let mut m = vec![b'b'];
        m.extend_from_slice(&id.to_le_bytes());
        m.extend_from_slice(&scr.to_le_bytes());
        m.push(REFBACKUP);
        m.extend_from_slice(&self.chan.to_le_bytes());
        m.push(0);
        Self::rect(&mut m, &r);
        Self::rect(&mut m, &r);
        m.extend_from_slice(&col.to_le_bytes());
        self.cmd(&m)?;
        Ok(id)
    }

    /// 'N': give image id a public name (or take it back), for other clients'
    /// namedimage (a draw(3) program's window, as rio's)
    pub fn name(&mut self, id: u32, name: &str, on: bool) -> io::Result<()> {
        let mut m = vec![b'N'];
        m.extend_from_slice(&id.to_le_bytes());
        m.push(on as u8);
        m.push(name.len() as u8);
        m.extend_from_slice(name.as_bytes());
        self.cmd(&m)
    }

    /// 't': windows to the top
    pub fn top(&mut self, ids: &[u32]) -> io::Result<()> {
        self.stack(ids, true)
    }

    /// 't': windows to the bottom
    pub fn bottom(&mut self, ids: &[u32]) -> io::Result<()> {
        self.stack(ids, false)
    }

    fn stack(&mut self, ids: &[u32], top: bool) -> io::Result<()> {
        let mut m = vec![b't', top as u8];
        m.extend_from_slice(&(ids.len() as u16).to_le_bytes());
        for id in ids {
            m.extend_from_slice(&id.to_le_bytes());
        }
        self.cmd(&m)
    }

    /// 'o': window id to screen point scr, its coordinates starting at log there
    pub fn origin(&mut self, id: u32, log: (i32, i32), scr: (i32, i32)) -> io::Result<()> {
        let mut m = vec![b'o'];
        m.extend_from_slice(&id.to_le_bytes());
        for v in [log.0, log.1, scr.0, scr.1] {
            m.extend_from_slice(&v.to_le_bytes());
        }
        self.cmd(&m)
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

    /// 'd' through a mask: src at sp into r of dst, where mask (at mp) lets it
    pub fn drawmask(&mut self, dst: u32, r: &Rect, src: u32, sp: (i32, i32), mask: u32, mp: (i32, i32)) -> io::Result<()> {
        if r.empty() {
            return Ok(());
        }
        let mut m = vec![b'd'];
        for id in [dst, src, mask] {
            m.extend_from_slice(&id.to_le_bytes());
        }
        Self::rect(&mut m, r);
        for v in [sp.0, sp.1, mp.0, mp.1] {
            m.extend_from_slice(&v.to_le_bytes());
        }
        self.cmd(&m)
    }

    /// 'y': rows of image id in its own format, bpl bytes a row (image(6)), in
    /// bands that fit a message (a font's subfont, rows short enough)
    pub fn loadraw(&mut self, id: u32, r: &Rect, data: &[u8], bpl: usize) -> io::Result<()> {
        let room = MAXMSG - 1 - 4 - 16;
        if bpl == 0 || bpl > room {
            return Err(io::Error::other("rows too long to load"));
        }
        let mut y = r.y0;
        while y < r.y1 {
            let rows = ((room / bpl) as i32).min(r.y1 - y);
            let o = (y - r.y0) as usize * bpl;
            let end = o + rows as usize * bpl;
            if end > data.len() {
                return Err(io::Error::other("short image data"));
            }
            let mut m = vec![b'y'];
            m.extend_from_slice(&id.to_le_bytes());
            Self::rect(&mut m, &Rect::new(r.x0, y, r.x1, y + rows));
            m.extend_from_slice(&data[o..end]);
            self.cmd(&m)?;
            y += rows;
        }
        Ok(())
    }

    /// 'Y': rows of image id from a block of a compressed image (image(6)),
    /// as it is: devdraw takes the compressed data
    pub fn loadcompressed(&mut self, id: u32, r: &Rect, block: &[u8]) -> io::Result<()> {
        let mut m = vec![b'Y'];
        m.extend_from_slice(&id.to_le_bytes());
        Self::rect(&mut m, r);
        m.extend_from_slice(block);
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
