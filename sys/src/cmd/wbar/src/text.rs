// SPDX-License-Identifier: MIT
//
// wbar's text: a proportional TrueType font (the theme's uifont, else Go
// Regular) rasterised by ab_glyph into coverage maps, once per character (a
// cache), drawn into 0x00RRGGBB pixels.

use ab_glyph::{Font, FontVec, PxScale, ScaleFont, point};
use std::collections::HashMap;
use std::io;

struct Glyph {
    x: i32, // placed relative to the pen on the line's top
    y: i32,
    w: usize,
    h: usize,
    adv: f32,
    cov: Vec<u8>,
}

pub struct Text {
    font: FontVec,
    scale: PxScale,
    ascent: f32,
    pub height: i32, // a line's
    cache: HashMap<char, Glyph>,
}

/// a mixed with b by cov (0..255), each 0x00RRGGBB
pub fn mix(a: u32, b: u32, cov: u32) -> u32 {
    let ch = |s: u32| -> u32 {
        let (x, y) = ((a >> s) & 0xFF, (b >> s) & 0xFF);
        ((x * (255 - cov) + y * cov + 127) / 255) << s
    };
    ch(16) | ch(8) | ch(0)
}

impl Text {
    pub fn open(path: &str, size: f32) -> io::Result<Text> {
        let data = std::fs::read(path)?;
        let font = FontVec::try_from_vec(data).map_err(|_| io::Error::other(format!("{path}: no TrueType font")))?;
        let scale = PxScale::from(size);
        let f = font.as_scaled(scale);
        let height = (f.ascent() - f.descent() + f.line_gap()).ceil().max(1.0) as i32;
        let ascent = f.ascent() + f.line_gap() / 2.0;
        Ok(Text { font, scale, ascent, height, cache: HashMap::new() })
    }

    fn glyph(&mut self, c: char) -> &Glyph {
        if !self.cache.contains_key(&c) {
            let sf = self.font.as_scaled(self.scale);
            let id = self.font.glyph_id(c);
            let adv = sf.h_advance(id);
            let g = id.with_scale_and_position(self.scale, point(0.0, self.ascent));
            let gl = match self.font.outline_glyph(g) {
                Some(og) => {
                    let b = og.px_bounds();
                    let (w, h) = (b.width() as usize, b.height() as usize);
                    let mut cov = vec![0u8; w * h];
                    og.draw(|x, y, v| {
                        let (x, y) = (x as usize, y as usize);
                        if x < w && y < h {
                            cov[y * w + x] = (v.clamp(0.0, 1.0) * 255.0).round() as u8;
                        }
                    });
                    Glyph { x: b.min.x as i32, y: b.min.y as i32, w, h, adv, cov }
                }
                None => Glyph { x: 0, y: 0, w: 0, h: 0, adv, cov: vec![] },
            };
            self.cache.insert(c, gl);
        }
        &self.cache[&c]
    }

    /// s's width in pixels
    pub fn width(&mut self, s: &str) -> i32 {
        s.chars().map(|c| self.glyph(c).adv).sum::<f32>().ceil() as i32
    }

    /// s as much as fits in w pixels: cut with an ellipsis if it does not
    pub fn fit(&mut self, s: &str, w: i32) -> String {
        if self.width(s) <= w {
            return s.to_string();
        }
        let dots = self.width("…");
        let mut out = String::new();
        let mut x = 0.0;
        for c in s.chars() {
            let a = self.glyph(c).adv;
            if (x + a).ceil() as i32 + dots > w {
                break;
            }
            x += a;
            out.push(c);
        }
        out.push('…');
        out
    }

    /// s in colour col into px (stride pixels a row, rows high), the line's top
    /// left at x, y; nothing right of xmax
    pub fn draw(&mut self, px: &mut [u32], stride: usize, rows: usize, x: i32, y: i32, s: &str, col: u32, xmax: i32) {
        let mut pen = x as f32;
        for c in s.chars() {
            let g = self.glyph(c);
            let gx = pen.round() as i32 + g.x;
            for j in 0..g.h {
                let py = y + g.y + j as i32;
                if py < 0 || py as usize >= rows {
                    continue;
                }
                for i in 0..g.w {
                    let pxx = gx + i as i32;
                    let cov = g.cov[j * g.w + i] as u32;
                    if cov == 0 || pxx < 0 || pxx >= xmax || pxx as usize >= stride {
                        continue;
                    }
                    let p = &mut px[py as usize * stride + pxx as usize];
                    *p = mix(*p, col, cov);
                }
            }
            pen += g.adv;
        }
    }
}
