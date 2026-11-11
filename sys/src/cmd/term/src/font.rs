// SPDX-License-Identifier: MIT
//
// term's glyphs: a monospaced TrueType font in four styles (regular, bold,
// italic, bold italic: Go Mono's from /lib/font/ttf by default), rasterised by
// ab_glyph into coverage maps, once each (a cache); a character a style lacks
// comes from the regular one, then from the other fonts in /lib/font/ttf.

use ab_glyph::{Font, FontVec, GlyphId, PxScale, ScaleFont, point};
use std::collections::HashMap;
use std::io;

/// a glyph's coverage (0..255), placed relative to its cell's top left
pub struct Glyph {
    pub x: i32,
    pub y: i32,
    pub w: usize,
    pub h: usize,
    pub cov: Vec<u8>,
}

pub struct Fonts {
    styles: [Option<FontVec>; 4], // regular, bold, italic, bold italic
    fallback: Vec<FontVec>,
    scale: PxScale,
    pub cw: i32, // a cell: its width and height in pixels
    pub ch: i32,
    pub ascent: f32,
    cache: HashMap<(char, u8), Option<Glyph>>,
}

fn load(path: &str) -> Option<FontVec> {
    FontVec::try_from_vec(std::fs::read(path).ok()?).ok()
}

/// the style's index: bold 1, italic 2
pub fn style(bold: bool, italic: bool) -> u8 {
    bold as u8 | (italic as u8) << 1
}

impl Fonts {
    /// the font at path (its bold and italic beside it: name-Bold.ttf, name-Italic.ttf,
    /// name-Bold-Italic.ttf), size in pixels
    pub fn open(path: &str, size: f32) -> io::Result<Fonts> {
        let regular = load(path).ok_or_else(|| io::Error::other(format!("{path}: no TrueType font")))?;
        let base = path.strip_suffix(".ttf").unwrap_or(path);
        let styles = [Some(regular), load(&format!("{base}-Bold.ttf")), load(&format!("{base}-Italic.ttf")), load(&format!("{base}-Bold-Italic.ttf"))];
        let mut fallback = vec![];
        if let Ok(d) = std::fs::read_dir("/lib/font/ttf") {
            let mut names: Vec<String> = d.filter_map(|e| e.ok()).map(|e| e.path().to_string_lossy().into_owned()).filter(|p| p.ends_with(".ttf") && !p.starts_with(base)).collect();
            names.sort();
            fallback.extend(names.iter().filter_map(|p| load(p)));
        }
        let scale = PxScale::from(size);
        let f = styles[0].as_ref().unwrap().as_scaled(scale);
        let cw = f.h_advance(f.glyph_id('M')).round().max(1.0) as i32;
        let ch = (f.ascent() - f.descent() + f.line_gap()).ceil().max(1.0) as i32;
        let ascent = f.ascent() + f.line_gap() / 2.0;
        Ok(Fonts { styles, fallback, scale, cw, ch, ascent, cache: HashMap::new() })
    }

    /// the font that has c in style s (the regular one, then the others), its glyph
    fn find(&self, c: char, s: u8) -> Option<(&FontVec, GlyphId)> {
        let own = self.styles[s as usize].as_ref().into_iter();
        let regular = self.styles[0].as_ref().into_iter();
        own.chain(regular).chain(self.fallback.iter()).find_map(|f| {
            let g = f.glyph_id(c);
            (g.0 != 0).then_some((f, g))
        })
    }

    /// c's glyph in style s for a cell (cells wide: 2 for a wide character); None: nothing to draw
    pub fn glyph(&mut self, c: char, s: u8, cells: i32) -> Option<&Glyph> {
        let key = (c, s | (cells as u8 - 1) << 2);
        if !self.cache.contains_key(&key) {
            let g = self.raster(c, s, cells);
            self.cache.insert(key, g);
        }
        self.cache.get(&key).unwrap().as_ref()
    }

    fn raster(&self, c: char, s: u8, cells: i32) -> Option<Glyph> {
        if c == ' ' || c.is_control() {
            return None;
        }
        let Some((font, id)) = self.find(c, s) else {
            // no font has it: a box, so that one sees something is there
            return Some(self.missing(cells));
        };
        let sf = font.as_scaled(self.scale);
        // centred in its cells (a fallback font's advance is not the cell's)
        let adv = sf.h_advance(id);
        let x0 = ((self.cw * cells) as f32 - adv).max(0.0) / 2.0;
        let g = id.with_scale_and_position(self.scale, point(x0, self.ascent));
        let og = font.outline_glyph(g)?;
        let b = og.px_bounds();
        let (w, h) = (b.width() as usize, b.height() as usize);
        if w == 0 || h == 0 {
            return None;
        }
        let mut cov = vec![0u8; w * h];
        og.draw(|x, y, v| {
            let (x, y) = (x as usize, y as usize);
            if x < w && y < h {
                cov[y * w + x] = (v.clamp(0.0, 1.0) * 255.0).round() as u8;
            }
        });
        Some(Glyph { x: b.min.x as i32, y: b.min.y as i32, w, h, cov })
    }

    /// an empty box inside the cells
    fn missing(&self, cells: i32) -> Glyph {
        let (x, y) = (1, self.ch / 8 + 1);
        let w = (self.cw * cells - 2 * x).max(2) as usize;
        let h = (self.ch - 2 * y).max(2) as usize;
        let mut cov = vec![0u8; w * h];
        for i in 0..w {
            cov[i] = 255;
            cov[(h - 1) * w + i] = 255;
        }
        for j in 0..h {
            cov[j * w] = 255;
            cov[j * w + w - 1] = 255;
        }
        Glyph { x, y, w, h, cov }
    }
}
