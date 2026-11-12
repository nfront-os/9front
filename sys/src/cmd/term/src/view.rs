// SPDX-License-Identifier: MIT
//
// term's picture: vt100's screen as pixels (0x00RRGGBB, p9win's), a cell at a
// time; only cells that look different from the last picture are drawn
// again, and the rectangles they cover are what goes to the window. The
// colours are the desktop's theme (desk: /dev/theme; classic without wsys):
// the text window's text and background, its selection, wsys' menus.

use crate::font::{self, Fonts};
use p9win::draw::Rect;

pub const MARGIN: i32 = 4;
const MENUPAD: i32 = 2;

/// xterm's sixteen colours
const PALETTE: [u32; 16] = [
    0x000000, 0xCD0000, 0x00CD00, 0xCDCD00, 0x0000EE, 0xCD00CD, 0x00CDCD, 0xE5E5E5,
    0x7F7F7F, 0xFF0000, 0x00FF00, 0xFFFF00, 0x5C5CFF, 0xFF00FF, 0x00FFFF, 0xFFFFFF,
];

fn idx(i: u8) -> u32 {
    match i {
        0..=15 => PALETTE[i as usize],
        16..=231 => {
            // the 6x6x6 cube
            let i = i - 16;
            let v = |n: u8| if n == 0 { 0 } else { 55 + 40 * n as u32 };
            v(i / 36) << 16 | v(i / 6 % 6) << 8 | v(i % 6)
        }
        _ => {
            let g = 8 + 10 * (i - 232) as u32;
            g << 16 | g << 8 | g
        }
    }
}

/// what a cell looks like: drawn again when it changes
#[derive(Clone, PartialEq, Default)]
struct Look {
    text: String,
    fg: u32,
    bg: u32,
    style: u8,
    underline: bool,
    cells: u8, // 1, 2 (wide), 0: the right half of a wide one (drawn with it)
    cursor: u8, // 0, 1 a block, 2 a box (the window is not current)
}

pub struct View {
    pub w: usize,
    pub h: usize,
    pub px: Vec<u32>,
    pub rows: u16,
    pub cols: u16,
    prev: Vec<Option<Look>>,
    pub fresh: bool, // all of it to the window (the margins too): after a resize
    sel: u32,        // a selection's background
    mborder: u32,    // a menu's border (its background: bg), the item under the pointer
    mhigh: u32,
    extra: Vec<Rect>, // drawn on besides the cells (the menu, where it was)
    fg: u32,
    bg: u32,
    pub margin: i32, // around the cells; and the menu's, at the window system's scale
    menupad: i32,
    menugap: i32,
    menuline: i32,
}

/// a selection: from (row, col) to (row, col), the end's cell included
#[derive(Clone, Copy, PartialEq)]
pub struct Sel {
    pub a: (u16, u16),
    pub b: (u16, u16),
}

impl Sel {
    fn ordered(&self) -> ((u16, u16), (u16, u16)) {
        if self.a <= self.b { (self.a, self.b) } else { (self.b, self.a) }
    }
    fn has(&self, r: u16, c: u16) -> bool {
        let (a, b) = self.ordered();
        (r, c) >= a && (r, c) <= b
    }
    /// vt100's contents_between: the end exclusive
    pub fn text(&self, s: &vt100::Screen) -> String {
        let (a, b) = self.ordered();
        s.contents_between(a.0, a.1, b.0, b.1 + 1)
    }
}

impl View {
    /// w x h pixels in the theme's colours; reverse: text and background swapped
    pub fn new(w: usize, h: usize, f: &Fonts, t: &desk::theme::Theme, reverse: bool) -> View {
        let (fg, bg) = if reverse { (t.window, t.text) } else { (t.text, t.window) };
        let mut v = View {
            w: 0, h: 0, px: vec![], rows: 0, cols: 0, prev: vec![], fresh: true, extra: vec![], fg, bg,
            sel: t.select, mborder: t.border, mhigh: t.menuhigh,
            margin: MARGIN, menupad: MENUPAD, menugap: 4, menuline: 1,
        };
        v.setdpi(p9win::dpi());
        v.resize(w, h, f);
        v
    }

    /// the theme's colours (it changed): everything drawn again at the next size
    pub fn colours(&mut self, t: &desk::theme::Theme, reverse: bool) {
        (self.fg, self.bg) = if reverse { (t.window, t.text) } else { (t.text, t.window) };
        (self.sel, self.mborder, self.mhigh) = (t.select, t.border, t.menuhigh);
    }

    /// sizes at the window system's dpi (libdraw's scalesize): the margins,
    /// the menu's; resize after it
    pub fn setdpi(&mut self, dpi: i32) {
        let k = |n| p9win::draw::scalesize(dpi, n);
        (self.margin, self.menupad, self.menugap, self.menuline) = (k(MARGIN), k(MENUPAD), k(4), k(1));
    }

    /// the window's new size: everything drawn again
    pub fn resize(&mut self, w: usize, h: usize, f: &Fonts) {
        self.w = w.max(1);
        self.h = h.max(1);
        self.cols = ((self.w as i32 - 2 * self.margin) / f.cw).clamp(1, 1000) as u16;
        self.rows = ((self.h as i32 - 2 * self.margin) / f.ch).clamp(1, 1000) as u16;
        self.px = vec![self.bg; self.w * self.h];
        self.prev = vec![None; self.rows as usize * self.cols as usize];
        self.fresh = true;
    }

    /// the cell at x y in the window's contents, clamped to the grid
    pub fn cell(&self, x: i32, y: i32, f: &Fonts) -> (u16, u16) {
        let c = ((x - self.margin) / f.cw).clamp(0, self.cols as i32 - 1);
        let r = ((y - self.margin) / f.ch).clamp(0, self.rows as i32 - 1);
        (r as u16, c as u16)
    }

    fn colour(&self, c: vt100::Color, default: u32) -> u32 {
        match c {
            vt100::Color::Default => default,
            vt100::Color::Idx(i) => idx(i),
            vt100::Color::Rgb(r, g, b) => (r as u32) << 16 | (g as u32) << 8 | b as u32,
        }
    }

    fn look(&self, s: &vt100::Screen, r: u16, c: u16, sel: Option<Sel>, cursor: u8) -> Look {
        let Some(cell) = s.cell(r, c) else { return Look { bg: self.bg, cells: 1, ..Default::default() } };
        if cell.is_wide_continuation() {
            return Look { cells: 0, ..Default::default() };
        }
        let mut fg = self.colour(cell.fgcolor(), self.fg);
        let mut bg = self.colour(cell.bgcolor(), self.bg);
        if cell.inverse() {
            std::mem::swap(&mut fg, &mut bg);
        }
        if cell.dim() {
            fg = mix(fg, bg);
        }
        if sel.is_some_and(|x| x.has(r, c)) {
            bg = self.sel;
            if fg == self.sel {
                fg = self.fg;
            }
        }
        if cursor == 1 {
            std::mem::swap(&mut fg, &mut bg);
        }
        Look {
            text: cell.contents().to_string(),
            fg,
            bg,
            style: font::style(cell.bold(), cell.italic()),
            underline: cell.underline(),
            cells: if cell.is_wide() { 2 } else { 1 },
            cursor,
        }
    }

    /// the screen drawn where it changed; the rectangles drawn on. cursor: where
    /// the cursor is to be shown (None: not), current: the window is the current one
    pub fn draw(&mut self, s: &vt100::Screen, f: &mut Fonts, sel: Option<Sel>, cursor: Option<(u16, u16)>, current: bool) -> Vec<Rect> {
        let mut damage = vec![];
        for r in 0..self.rows {
            let (mut x0, mut x1) = (i32::MAX, i32::MIN);
            for c in 0..self.cols {
                let cur = match cursor {
                    Some(p) if p == (r, c) => if current { 1 } else { 2 },
                    _ => 0,
                };
                let l = self.look(s, r, c, sel, cur);
                let i = r as usize * self.cols as usize + c as usize;
                if self.prev[i].as_ref() == Some(&l) {
                    continue;
                }
                if l.cells > 0 {
                    let cells = l.cells.min((self.cols - c) as u8) as i32;
                    let (x, y) = (self.margin + c as i32 * f.cw, self.margin + r as i32 * f.ch);
                    self.paint(&l, x, y, cells, f);
                    x0 = x0.min(x);
                    x1 = x1.max(x + cells * f.cw);
                    // the wide one's right half is drawn again with it
                    if cells == 2 && (c + 1) < self.cols {
                        self.prev[i + 1] = None;
                    }
                }
                self.prev[i] = Some(l);
            }
            if x0 < x1 {
                let y = self.margin + r as i32 * f.ch;
                damage.push(Rect::new(x0, y, x1, y + f.ch));
            }
        }
        damage.append(&mut self.extra);
        damage
    }

    /// r drawn again from the screen at the next draw (what a menu covered)
    pub fn forget(&mut self, r: Rect, f: &Fonts) {
        let r = r.clip(&Rect::new(0, 0, self.w as i32, self.h as i32));
        if r.empty() {
            return;
        }
        for yy in r.y0..r.y1 {
            let row = yy as usize * self.w;
            self.px[row + r.x0 as usize..row + r.x1 as usize].fill(self.bg);
        }
        let (r0, c0) = self.cell(r.x0, r.y0, f);
        let (r1, c1) = self.cell(r.x1 - 1, r.y1 - 1, f);
        for row in r0..=r1 {
            // a wide character's left half may be left of the rectangle
            for col in c0.saturating_sub(1)..=c1 {
                self.prev[row as usize * self.cols as usize + col as usize] = None;
            }
        }
        self.extra.push(r);
    }

    /// the menu's rectangle for items, its first item's middle at x y, inside the window
    pub fn menurect(&self, items: &[&str], x: i32, y: i32, f: &Fonts) -> Rect {
        let n = items.iter().map(|s| s.chars().count()).max().unwrap_or(1) as i32;
        let w = n * f.cw + 2 * (self.menupad + self.menugap);
        let ih = f.ch + 2 * self.menupad;
        let h = items.len() as i32 * ih + 2 * self.menupad;
        let x0 = (x - w / 2).clamp(0, (self.w as i32 - w).max(0));
        let y0 = (y - self.menupad - ih / 2).clamp(0, (self.h as i32 - h).max(0));
        Rect::new(x0, y0, x0 + w, y0 + h)
    }

    /// the item at x y in the menu at r, if any
    pub fn menuitem(&self, r: Rect, n: usize, x: i32, y: i32, f: &Fonts) -> Option<usize> {
        if !r.contains(x, y) {
            return None;
        }
        let i = ((y - r.y0 - self.menupad) / (f.ch + 2 * self.menupad)).max(0) as usize;
        (i < n).then_some(i)
    }

    /// the menu at r, item hi lit
    pub fn menu(&mut self, r: Rect, items: &[&str], hi: Option<usize>, f: &mut Fonts) {
        let fill = |v: &mut View, q: Rect, c: u32| {
            let q = q.clip(&Rect::new(0, 0, v.w as i32, v.h as i32));
            for yy in q.y0..q.y1 {
                let row = yy as usize * v.w;
                v.px[row + q.x0 as usize..row + q.x1 as usize].fill(c);
            }
        };
        let (mborder, mback, mhigh, mtext) = (self.mborder, self.bg, self.mhigh, self.fg);
        let (mp, ml) = (self.menupad, self.menuline);
        fill(self, r, mborder);
        fill(self, r.inset(ml), mback);
        let ih = f.ch + 2 * mp;
        for (i, s) in items.iter().enumerate() {
            let iy = r.y0 + mp + i as i32 * ih;
            let lit = hi == Some(i);
            if lit {
                fill(self, Rect::new(r.x0 + mp, iy, r.x1 - mp, iy + ih), mhigh);
            }
            // centred, as rio's
            let tx = r.x0 + (r.dx() - s.chars().count() as i32 * f.cw) / 2;
            // as wsys' menus: the text as it is, the item under the pointer on menuhigh
            let fg = mtext;
            let cw = f.cw;
            for (k, ch) in s.chars().enumerate() {
                let Some(g) = f.glyph(ch, 0, 1) else { continue };
                let (gx0, gy0) = (tx + k as i32 * cw + g.x, iy + mp + g.y);
                for gy in 0..g.h {
                    for gx in 0..g.w {
                        let (px, py) = (gx0 + gx as i32, gy0 + gy as i32);
                        let a = g.cov[gy * g.w + gx];
                        if a == 0 || px < 0 || py < 0 || px >= self.w as i32 || py >= self.h as i32 {
                            continue;
                        }
                        let p = &mut self.px[py as usize * self.w + px as usize];
                        *p = blend(fg, *p, a);
                    }
                }
            }
        }
        self.extra.push(r);
    }

    fn paint(&mut self, l: &Look, x: i32, y: i32, cells: i32, f: &mut Fonts) {
        let (w, h) = (cells * f.cw, f.ch);
        let clip = Rect::new(x, y, x + w, y + h).clip(&Rect::new(0, 0, self.w as i32, self.h as i32));
        for yy in clip.y0..clip.y1 {
            let row = yy as usize * self.w;
            self.px[row + clip.x0 as usize..row + clip.x1 as usize].fill(l.bg);
        }
        for ch in l.text.chars() {
            let Some(g) = f.glyph(ch, l.style, cells) else { continue };
            for gy in 0..g.h {
                let py = y + g.y + gy as i32;
                if py < clip.y0 || py >= clip.y1 {
                    continue;
                }
                for gx in 0..g.w {
                    let a = g.cov[gy * g.w + gx];
                    let px = x + g.x + gx as i32;
                    if a == 0 || px < clip.x0 || px >= clip.x1 {
                        continue;
                    }
                    let p = &mut self.px[py as usize * self.w + px as usize];
                    *p = blend(l.fg, *p, a);
                }
            }
        }
        if l.underline {
            let uy = (y + f.ascent as i32 + 2).min(y + h - 1);
            if uy >= clip.y0 && uy < clip.y1 {
                let row = uy as usize * self.w;
                self.px[row + clip.x0 as usize..row + clip.x1 as usize].fill(l.fg);
            }
        }
        if l.cursor == 2 {
            // a box: the window is not the current one
            for xx in clip.x0..clip.x1 {
                self.px[clip.y0 as usize * self.w + xx as usize] = l.fg;
                self.px[(clip.y1 - 1) as usize * self.w + xx as usize] = l.fg;
            }
            for yy in clip.y0..clip.y1 {
                self.px[yy as usize * self.w + clip.x0 as usize] = l.fg;
                self.px[yy as usize * self.w + (clip.x1 - 1) as usize] = l.fg;
            }
        }
    }
}

/// fg over bg with coverage a
fn blend(fg: u32, bg: u32, a: u8) -> u32 {
    let a = a as u32;
    let ch = |s: u32| (((fg >> s) & 0xFF) * a + ((bg >> s) & 0xFF) * (255 - a) + 127) / 255;
    ch(16) << 16 | ch(8) << 8 | ch(0)
}

fn mix(a: u32, b: u32) -> u32 {
    blend(a, b, 128)
}
