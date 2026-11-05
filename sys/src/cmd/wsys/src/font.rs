// SPDX-License-Identifier: MIT
//
// Plan 9's bitmap fonts (font(6), image(6)) for wsys' text windows, without
// libdraw: a font file names subfonts by ranges of runes; a subfont is an image
// (compressed ones go to devdraw as they are, 'Y') and the metrics of its
// characters. A character is drawn as libdraw does: a colour through the
// subfont's image as the mask. Subfonts are loaded when a rune of theirs is
// first drawn.

use crate::draw::{strtochan, Display, Rect};
use std::collections::HashMap;
use std::io;

#[derive(Clone, Copy, Default)]
struct Fontchar {
    x: i32,      // left edge of the character's bits in the image
    top: i32,    // first row of them
    bottom: i32, // last row + 1
    left: i32,   // offset of the bits from the pen
    width: i32,  // how far the pen moves
}

struct Subfont {
    img: u32,
    ascent: i32,
    info: Vec<Fontchar>,
}

struct Range {
    min: u32,
    max: u32,
    offset: u32,
    file: String,
}

pub struct Font {
    pub height: i32,
    pub ascent: i32,
    ranges: Vec<Range>,
    subs: HashMap<String, Option<Subfont>>, // None: it did not load
}

/// a character drawn: the subfont's image and where the bits are in it, and
/// where they go from the pen (top of the line)
pub struct Glyph {
    pub img: u32,
    pub src: Rect,
    pub dx: i32,
    pub dy: i32,
    pub width: i32,
}

fn int12(b: &[u8], i: usize) -> Option<i32> {
    std::str::from_utf8(b.get(i * 12..i * 12 + 12)?).ok()?.trim().parse().ok()
}

fn num(s: &str) -> Option<u32> {
    if let Some(h) = s.strip_prefix("0x").or_else(|| s.strip_prefix("0X")) {
        u32::from_str_radix(h, 16).ok()
    } else if s.len() > 1 && s.starts_with('0') {
        u32::from_str_radix(&s[1..], 8).ok()
    } else {
        s.parse().ok()
    }
}

impl Font {
    pub fn open(path: &str) -> io::Result<Font> {
        let text = std::fs::read_to_string(path)?;
        let dir = match path.rfind('/') {
            Some(i) => &path[..i + 1],
            None => "",
        };
        let mut w = text.split_whitespace().peekable();
        let bad = || io::Error::other(format!("{path}: bad font file"));
        let height = w.next().and_then(num).ok_or_else(bad)? as i32;
        let ascent = w.next().and_then(num).ok_or_else(bad)? as i32;
        let mut ranges = vec![];
        while let Some(t) = w.next() {
            let min = num(t).ok_or_else(bad)?;
            let max = w.next().and_then(num).ok_or_else(bad)?;
            let mut offset = 0;
            if let Some(n) = w.peek().and_then(|t| num(t)) {
                offset = n;
                w.next();
            }
            let f = w.next().ok_or_else(bad)?;
            let file = if f.starts_with('/') { f.to_string() } else { format!("{dir}{f}") };
            ranges.push(Range { min, max, offset, file });
        }
        Ok(Font { height, ascent, ranges, subs: HashMap::new() })
    }

    fn load(d: &mut Display, file: &str) -> io::Result<Subfont> {
        let b = std::fs::read(file)?;
        let bad = || io::Error::other(format!("{file}: bad subfont"));
        let (compressed, h) = match b.strip_prefix(b"compressed\n") {
            Some(rest) => (true, rest),
            None => (false, &b[..]),
        };
        let chan = std::str::from_utf8(h.get(..12).ok_or_else(bad)?).map_err(|_| bad())?.trim().to_string();
        let r = Rect::new(int12(h, 1).ok_or_else(bad)?, int12(h, 2).ok_or_else(bad)?, int12(h, 3).ok_or_else(bad)?, int12(h, 4).ok_or_else(bad)?);
        let ch = strtochan(&chan).ok_or_else(bad)?;
        let depth: i32 = chan.as_bytes().chunks(2).map(|p| (p[1] - b'0') as i32).sum();
        let img = d.alloc(r, ch, false, 0)?;
        let mut p = 60;
        let res = (|| -> io::Result<()> {
            if compressed {
                let mut miny = r.y0;
                while miny < r.y1 {
                    let maxy = int12(&h[p..], 0).ok_or_else(bad)?;
                    let nb = int12(&h[p..], 1).ok_or_else(bad)? as usize;
                    p += 24;
                    let block = h.get(p..p + nb).ok_or_else(bad)?;
                    d.loadcompressed(img, &Rect::new(r.x0, miny, r.x1, maxy), block)?;
                    p += nb;
                    miny = maxy;
                }
            } else {
                let bpl = ((r.x1 * depth + 7) / 8 - (r.x0 * depth) / 8) as usize;
                let n = bpl * r.dy() as usize;
                d.loadraw(img, &r, h.get(p..p + n).ok_or_else(bad)?, bpl)?;
                p += n;
            }
            Ok(())
        })();
        if let Err(e) = res {
            let _ = d.free(img);
            return Err(e);
        }
        // the subfont: n height ascent, then n+1 Fontchars of 6 bytes
        let n = int12(&h[p..], 0).ok_or_else(bad)? as usize;
        let ascent = int12(&h[p..], 2).ok_or_else(bad)?;
        p += 36;
        let mut info = Vec::with_capacity(n + 1);
        for i in 0..=n {
            let c = h.get(p + 6 * i..p + 6 * i + 6).ok_or_else(bad)?;
            info.push(Fontchar {
                x: u16::from_le_bytes([c[0], c[1]]) as i32,
                top: c[2] as i32,
                bottom: c[3] as i32,
                left: c[4] as i8 as i32,
                width: c[5] as i32,
            });
        }
        Ok(Subfont { img, ascent, info })
    }

    /// rune c, its subfont loaded if need be; none: the font lacks it
    pub fn glyph(&mut self, d: &mut Display, c: char) -> Option<Glyph> {
        let r = c as u32;
        let rg = self.ranges.iter().find(|g| r >= g.min && r <= g.max)?;
        let (i, file) = ((r - rg.min + rg.offset) as usize, rg.file.clone());
        if !self.subs.contains_key(&file) {
            let s = Self::load(d, &file).inspect_err(|e| eprintln!("wsys: font: {e}")).ok();
            self.subs.insert(file.clone(), s);
        }
        let s = self.subs.get(&file)?.as_ref()?;
        if i + 1 >= s.info.len() {
            return None;
        }
        let (f, next) = (s.info[i], s.info[i + 1]);
        Some(Glyph {
            img: s.img,
            src: Rect::new(f.x, f.top, next.x, f.bottom),
            dx: f.left,
            dy: self.ascent - s.ascent,
            width: f.width,
        })
    }

    pub fn width(&mut self, d: &mut Display, c: char) -> i32 {
        self.glyph(d, c).map(|g| g.width).unwrap_or(0)
    }
}
