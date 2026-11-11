// SPDX-License-Identifier: MIT
//
// A client of draw(3), the protocol libdraw speaks (its messages as init.c's
// vdrawcmd writes them, little-endian): wsys's processor backend draws through
// it, so devdraw (memdraw) composes and the screen is any draw(3) has: VESA,
// a vga driver, a GPU driver's framebuffer, or a rio window.

use crate::drawcore;
use crate::p9::{Fd, ORDWR};
use std::io;

pub const XRGB32: u32 = 0x6808_1828; // CHAN4(CIgnore,8, CRed,8, CGreen,8, CBlue,8)
pub const ARGB32: u32 = 0x4808_1828; // CHAN4(CAlpha,8, CRed,8, CGreen,8, CBlue,8)
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

/// the bits a pixel of channels chan has (chantodepth)
pub fn chandepth(chan: u32) -> i32 {
    (0..4).map(|i| ((chan >> (8 * i)) & 15) as i32).sum()
}

/// channels chan as their name ("x8r8g8b8", chantostr)
pub fn chanstr(chan: u32) -> String {
    let mut s = String::new();
    for i in (0..4).rev() {
        let c = (chan >> (8 * i)) & 0xFF;
        if c == 0 {
            continue;
        }
        s.push(b"rgbkamx"[((c >> 4) as usize).min(6)] as char);
        s.push_str(&(c & 15).to_string());
    }
    s
}

pub const REFBACKUP: u8 = 0;

/// the connection: the files of a devdraw (the kernel's, or rio's window's), or
/// wsys' own devdraw's channels (drawcore)
enum Conn {
    Files { ctl: Fd, data: Fd },
    Core { ctl: drawcore::Chan, data: drawcore::Chan },
}

impl Conn {
    fn write1(&self, b: &[u8]) -> io::Result<()> {
        match self {
            Conn::Files { data, .. } => data.write1(b),
            Conn::Core { data, .. } => data.write(b).map(|_| ()).map_err(io::Error::other),
        }
    }
    fn read(&self, b: &mut [u8]) -> io::Result<usize> {
        match self {
            Conn::Files { data, .. } => data.read(b),
            Conn::Core { data, .. } => match data.read(b, 0).map_err(io::Error::other)? {
                drawcore::Read::Data(n) => Ok(n),
                drawcore::Read::Wait => Ok(0),
            },
        }
    }
    fn ctlread(&self, b: &mut [u8]) -> io::Result<usize> {
        match self {
            Conn::Files { ctl, .. } => ctl.read(b),
            Conn::Core { ctl, .. } => match ctl.read(b, 0).map_err(io::Error::other)? {
                drawcore::Read::Data(n) => Ok(n),
                drawcore::Read::Wait => Ok(0),
            },
        }
    }
}

pub struct Display {
    conn: Conn, // the connection lives while new (the ctl) is open
    buf: Vec<u8>,
    pub screen: u32,  // image id drawn on: 0, or the rio window's
    pub chan: u32,    // its channels: windows on it have them too
    pub r: Rect,      // its rectangle, inside rio's border
    next: u32,
    pub opaque: u32,
    layer: Option<(u32, u32)>, // onlayer: the screen drawn under its name and the screen of the window over it
    pub raw: u64,  // bytes of pixels cload_at was given
    pub sent: u64, // and the bytes of its messages
}

/// image(6)'s compression of one message's rows (cload_at)
struct Lz {
    data: Vec<u8>, // the rows so far: copies come from up to NMEM back in it
    out: Vec<u8>,
    last: Vec<u32>, // a hash of 3 bytes: where they were last (+1; 0: nowhere)
}

const NMATCH: usize = 3;
const NRUN: usize = NMATCH + 31;
const NMEM: usize = 1024;
const NDUMP: usize = 128;
const LZHASH: usize = 1 << 12;

impl Lz {
    fn new() -> Lz {
        Lz { data: Vec::new(), out: Vec::new(), last: vec![0; LZHASH] }
    }

    fn hash(b: &[u8]) -> usize {
        ((b[0] as usize) << 8 ^ (b[1] as usize) << 4 ^ b[2] as usize) & (LZHASH - 1)
    }

    /// how many bytes at a and at i are the same, at most max (a < i: the
    /// copy may overlap where it is going, as the decoder copies byte by byte)
    fn same(&self, a: usize, i: usize, max: usize) -> usize {
        let d = &self.data;
        let mut n = 0;
        while n < max && d[a + n] == d[i + n] {
            n += 1;
        }
        n
    }

    fn dump(&mut self, from: usize, to: usize) {
        let mut p = from;
        while p < to {
            let n = (to - p).min(NDUMP);
            self.out.push(0x80 | (n - 1) as u8);
            self.out.extend_from_slice(&self.data[p..p + n]);
            p += n;
        }
    }

    fn note(&mut self, i: usize, end: usize) {
        if i + NMATCH <= end {
            let h = Self::hash(&self.data[i..]);
            self.last[h] = i as u32 + 1;
        }
    }

    /// a row: no literal or copy goes past its end
    fn row(&mut self, row: &[u8]) {
        let start = self.data.len();
        self.data.extend_from_slice(row);
        let end = self.data.len();
        let (mut i, mut lit) = (start, start);
        while i < end {
            let max = (end - i).min(NRUN);
            let mut best = (0, 0); // length, offset
            if max >= NMATCH {
                // the pixel before, the row above, what the hash saw last
                let h = self.last[Self::hash(&self.data[i..])] as usize;
                for a in [i.checked_sub(4), i.checked_sub(row.len()), h.checked_sub(1)].into_iter().flatten() {
                    if a < i && i - a <= NMEM {
                        let n = self.same(a, i, max);
                        if n > best.0 {
                            best = (n, i - a);
                        }
                    }
                }
            }
            if best.0 >= NMATCH {
                self.dump(lit, i);
                let (n, off) = best;
                self.out.push((((n - NMATCH) << 2) | ((off - 1) >> 8)) as u8);
                self.out.push(((off - 1) & 0xFF) as u8);
                for k in i..i + n {
                    self.note(k, end);
                }
                i += n;
                lit = i;
            } else {
                self.note(i, end);
                i += 1;
            }
        }
        self.dump(lit, end);
    }
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
        Self::with(Conn::Files { ctl, data }, &f, true)
    }

    /// wsys' own devdraw (drawcore), its screen
    pub fn open_core() -> io::Result<Display> {
        let ctl = drawcore::Chan::open(drawcore::QNEW, 2).map_err(io::Error::other)?;
        let data = drawcore::Chan::open((ctl.path & !0xF) | drawcore::QDATA, 2).map_err(io::Error::other)?;
        let conn = Conn::Core { ctl, data };
        let mut b = [0u8; 12 * 12 + 1];
        let n = conn.ctlread(&mut b)?;
        let f = info(&b[..n])?;
        Self::with(conn, &f, false)
    }

    fn with(conn: Conn, f: &[String], winname: bool) -> io::Result<Display> {
        let mut b = [0u8; 12 * 12 + 1];
        let mut d = Display {
            conn,
            buf: Vec::with_capacity(MAXMSG),
            screen: 0,
            chan: strtochan(&f[2]).unwrap_or(XRGB32),
            r: Rect::new(int(&f[4]), int(&f[5]), int(&f[6]), int(&f[7])),
            next: 16,
            opaque: 0,
            layer: None,
            raw: 0,
            sent: 0,
        };
        let w = if winname { std::fs::read_to_string("/dev/winname").ok() } else { None };
        if let Some(w) = w {
            let w = w.trim().to_string();
            if !w.is_empty() {
                let id = d.id();
                let mut m = vec![b'n'];
                m.extend_from_slice(&id.to_le_bytes());
                m.push(w.len() as u8);
                m.extend_from_slice(w.as_bytes());
                d.cmd(&m)?;
                d.flush()?;
                let n = d.conn.ctlread(&mut b)?;
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

    /// the window of /dev/winname again (rio's 'r': it was made anew, moved or
    /// resized); the old one let go. False: not in a window (the screen), as it was
    pub fn reattach(&mut self) -> io::Result<bool> {
        let w = std::fs::read_to_string("/dev/winname").unwrap_or_default();
        let w = w.trim();
        if w.is_empty() || self.screen == 0 {
            return Ok(false);
        }
        self.offlayer();
        let id = self.id();
        let mut m = vec![b'n'];
        m.extend_from_slice(&id.to_le_bytes());
        m.push(w.len() as u8);
        m.extend_from_slice(w.as_bytes());
        self.cmd(&m)?;
        self.flush()?;
        let mut b = [0u8; 12 * 12 + 1];
        let n = self.conn.ctlread(&mut b)?;
        let f = info(&b[..n])?;
        let old = std::mem::replace(&mut self.screen, id);
        let _ = self.free(old);
        self.r = Rect::new(int(&f[4]), int(&f[5]), int(&f[6]), int(&f[7]));
        if !w.starts_with("noborder.") {
            self.r = self.r.inset(BORDERWIDTH);
        }
        Ok(true)
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
            return self.conn.write1(m);
        }
        self.buf.extend_from_slice(m);
        Ok(())
    }

    fn flushbuf(&mut self) -> io::Result<()> {
        if !self.buf.is_empty() {
            let r = self.conn.write1(&self.buf);
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
            match self.conn.write1(&m) {
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

    /// a whole screen (/dev/winname noborder.screen.n, no rio): a window of
    /// the screen's size over it, which what is drawn goes into instead
    /// (screen is then its id). Only through a layer does every devdraw show
    /// what is drawn on the screen: one drawn under its name, not as image
    /// 0, it leaves unflushed (drawterm's; 9front's before 9front-ng); and
    /// image 0 stays the old screen after a resize
    pub fn onlayer(&mut self) -> io::Result<()> {
        self.offlayer();
        let w = std::fs::read_to_string("/dev/winname").unwrap_or_default();
        if !w.trim().starts_with("noborder.screen.") || self.screen == 0 {
            return Ok(());
        }
        let base = self.screen;
        let scr = self.allocscreen(base, self.opaque)?;
        let win = self.allocwindow(scr, self.r, 0x0000_00FF)?;
        self.flush()?;
        self.layer = Some((base, scr));
        self.screen = win;
        Ok(())
    }

    /// onlayer's window and screen gone, the screen drawn on again
    fn offlayer(&mut self) {
        if let Some((base, scr)) = self.layer.take() {
            let win = std::mem::replace(&mut self.screen, base);
            let _ = self.free(win);
            let mut m = vec![b'F'];
            m.extend_from_slice(&scr.to_le_bytes());
            let _ = self.cmd(&m);
        }
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

    /// 'r': the pixels of rectangle r of image id (depth bits each), read back
    /// in bands of rows, as libdraw's unloadimage
    pub fn unload(&mut self, id: u32, r: &Rect, depth: i32) -> io::Result<Vec<u8>> {
        self.flushbuf()?;
        let bpl = ((r.x1 * depth + 7) / 8 - (r.x0 * depth) / 8) as usize;
        let mut out = Vec::with_capacity(bpl * r.dy().max(0) as usize);
        let rows = ((MAXMSG / bpl.max(1)) as i32).max(1);
        let mut y = r.y0;
        while y < r.y1 {
            let y1 = (y + rows).min(r.y1);
            let mut m = vec![b'r'];
            m.extend_from_slice(&id.to_le_bytes());
            Self::rect(&mut m, &Rect::new(r.x0, y, r.x1, y1));
            self.conn.write1(&m)?;
            let want = bpl * (y1 - y) as usize;
            let mut got = 0;
            let mut b = vec![0u8; want];
            while got < want {
                let n = self.conn.read(&mut b[got..])?;
                if n == 0 {
                    return Err(io::Error::other("short read of an image"));
                }
                got += n;
            }
            out.extend_from_slice(&b);
            y = y1;
        }
        Ok(out)
    }

    /// 'y': rectangle r of image id from pixels laid out with stride (XRGB32), in
    /// as load_at, compressed ('Y', image(6)'s compression: what memdraw's
    /// cloadmemimage reads): literals of up to 128 bytes, copies of 3 to 34
    /// bytes from up to 1024 back in what the message gave so far, neither
    /// across a row's end. A message a band of rows (the rows of r's columns,
    /// at most 1900 pixels wide: a row's worst fits). For a screen far away
    /// (drawterm): flat colours and repeated rows shrink much
    pub fn cload_at(&mut self, id: u32, r: &Rect, px: &[u8], stride: usize, sx: i32, sy: i32) -> io::Result<()> {
        const HEAD: usize = 1 + 4 + 16;
        const COLS: i32 = 1900;
        let (dx, dy) = (sx - r.x0, sy - r.y0);
        let mut x0 = r.x0;
        while x0 < r.x1 {
            let x1 = (x0 + COLS).min(r.x1);
            let bpl = (x1 - x0) as usize * 4;
            let worst = bpl + bpl.div_ceil(128);
            let room = MAXMSG - HEAD;
            let mut y = r.y0;
            let mut enc = Lz::new();
            let mut band = y;
            while y < r.y1 {
                let o = (y + dy) as usize * stride + (x0 + dx) as usize * 4;
                // a row more would not fit: the band so far is a message
                if enc.out.len() + worst > room && y > band {
                    self.cmsg(id, &Rect::new(x0, band, x1, y), &enc.out)?;
                    enc = Lz::new();
                    band = y;
                }
                enc.row(&px[o..o + bpl]);
                y += 1;
            }
            if y > band {
                self.cmsg(id, &Rect::new(x0, band, x1, y), &enc.out)?;
            }
            self.raw += (r.y1 - r.y0) as u64 * bpl as u64;
            x0 = x1;
        }
        Ok(())
    }

    fn cmsg(&mut self, id: u32, r: &Rect, data: &[u8]) -> io::Result<()> {
        let n = 1 + 4 + 16 + data.len();
        if self.buf.len() + n > MAXMSG {
            self.flushbuf()?;
        }
        self.buf.push(b'Y');
        self.buf.extend_from_slice(&id.to_le_bytes());
        Self::rect(&mut self.buf, r);
        self.buf.extend_from_slice(data);
        self.sent += n as u64;
        Ok(())
    }

    /// messages of at most MAXMSG bytes: bands of rows, or pieces of a row
    /// the pixels for r's top left corner are at (sx, sy) of px. Each message
    /// is made in the buffer of messages itself (no copy of its own: a frame is
    /// some hundred of them)
    pub fn load_at(&mut self, id: u32, r: &Rect, px: &[u8], stride: usize, sx: i32, sy: i32) -> io::Result<()> {
        let (dx, dy) = (sx - r.x0, sy - r.y0); // from r's coordinates to px's
        let room = MAXMSG - 1 - 4 - 16;
        let bpl = r.dx() as usize * 4;
        let mut y = r.y0;
        while y < r.y1 {
            // a band of rows, or a piece of a row too long for a message
            let (band, rows) = if bpl <= room {
                let rows = ((room / bpl) as i32).min(r.y1 - y);
                (Rect::new(r.x0, y, r.x1, y + rows), rows)
            } else {
                (Rect::new(r.x0, y, r.x1, y + 1), 1)
            };
            let mut x = band.x0;
            while x < band.x1 {
                let x1 = if bpl <= room { band.x1 } else { (x + (room / 4) as i32).min(band.x1) };
                let n = (x1 - x) as usize * 4;
                if self.buf.len() + 1 + 4 + 16 + n * rows as usize > MAXMSG {
                    self.flushbuf()?;
                }
                self.buf.push(b'y');
                self.buf.extend_from_slice(&id.to_le_bytes());
                let piece = Rect::new(x, band.y0, x1, band.y1);
                Self::rect(&mut self.buf, &piece);
                for yy in band.y0..band.y1 {
                    let o = (yy + dy) as usize * stride + (x + dx) as usize * 4;
                    self.buf.extend_from_slice(&px[o..o + n]);
                }
                x = x1;
            }
            y += rows;
        }
        Ok(())
    }
}
