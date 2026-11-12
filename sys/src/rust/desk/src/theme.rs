// SPDX-License-Identifier: MIT
//
// the desktop's theme (../9front-ui PLAN stage 7): its colours and fonts as text,
// a line "name value" each (one starting with # a comment), as /lib/theme/* and the file
// theme hold them (wsys(4)); colours #rrggbb. What a line does not name stays.
// classic is wsys' own (rio's colours); ui and term read the same file.
// scale (../9front-ui PLAN stage 8): the desktop's factor, 1 = 96 dpi;
// "scale auto" lets wsys find it (the monitor's EDID), "scale auto 1.5" is
// what it found, "scale 1.5" a factor given. Programs size what they draw
// by it: scalesize(n) = (n·dpi + 48)/96, dpi = round(96·scale), the same
// formula libdraw's scalesize(2) has (/dev/dpi).

use std::fmt::Write;

#[derive(Clone, Debug, PartialEq)]
pub struct Theme {
    pub back: u32,           // the screen behind the windows
    pub border: u32,         // the current window's border
    pub borderinactive: u32, // the other windows' borders
    pub window: u32,         // a text window's and a menu's background
    pub text: u32,           // their text
    pub select: u32,         // a selection's background
    pub menuhigh: u32,       // a menu's item under the mouse
    pub scroll: u32,         // a text window's scroll bar
    pub hold: u32,           // text and border while holding (Esc), current window
    pub holdinactive: u32,   // the border of a window holding, not current
    pub title: bool,         // windows with a title bar (stage 7, step 3)
    pub font: String,        // wsys' font (font(6)); empty: -f, $font or vga's
    pub uifont: String,      // ui's font (TrueType); empty: its own choice
    pub monofont: String,    // term's (TrueType); empty: Go Mono
    pub scale: f64,          // the factor (1: 96 dpi)
    pub scaleauto: bool,     // wsys finds it (scale above: what it found)
}

impl Default for Theme {
    fn default() -> Theme {
        Theme::classic()
    }
}

/// a colour: #rrggbb (or rrggbb)
fn colour(v: &str) -> Option<u32> {
    let h = v.strip_prefix('#').unwrap_or(v);
    (h.len() == 6).then(|| u32::from_str_radix(h, 16).ok()).flatten()
}

impl Theme {
    /// rio's colours, as wsys had them
    pub fn classic() -> Theme {
        Theme {
            back: 0x777777,
            border: 0x335588,
            borderinactive: 0xAAAAAA,
            window: 0xFFFFFF,
            text: 0x000000,
            select: 0xFFFFAA,
            menuhigh: 0x9EEEEE,
            scroll: 0x999999,
            hold: 0x000099,
            holdinactive: 0x4993DD,
            title: false,
            font: String::new(),
            uifont: String::new(),
            monofont: String::new(),
            scale: 1.0,
            scaleauto: true,
        }
    }

    /// the dots per inch the factor makes: round(96·scale)
    pub fn dpi(&self) -> i32 {
        dpi(self.scale)
    }

    /// n pixels at 96 dpi, at this theme's (libdraw's scalesize)
    pub fn scalesize(&self, n: i32) -> i32 {
        scalesize(self.dpi(), n)
    }

    /// the lines of text applied to this theme; the first line it does not know
    /// is an error, and then nothing is changed
    pub fn apply(&mut self, text: &str) -> Result<(), String> {
        let mut t = self.clone();
        for (i, line) in text.lines().enumerate() {
            // a line that starts with # is a comment (a colour's # is not: "border #335588")
            let mut f = line.split_whitespace();
            let Some(name) = f.next() else { continue };
            if name.starts_with('#') {
                continue;
            }
            let value = f.collect::<Vec<_>>().join(" ");
            let bad = || format!("theme: line {}: {name} {value}", i + 1);
            let c = |v: &str| colour(v).ok_or_else(bad);
            match name {
                "back" => t.back = c(&value)?,
                "border" => t.border = c(&value)?,
                "borderinactive" => t.borderinactive = c(&value)?,
                "window" => t.window = c(&value)?,
                "text" => t.text = c(&value)?,
                "select" => t.select = c(&value)?,
                "menuhigh" => t.menuhigh = c(&value)?,
                "scroll" => t.scroll = c(&value)?,
                "hold" => t.hold = c(&value)?,
                "holdinactive" => t.holdinactive = c(&value)?,
                "title" => t.title = matches!(value.as_str(), "1" | "on" | "yes"),
                "font" => t.font = value,
                "uifont" => t.uifont = value,
                "monofont" => t.monofont = value,
                "scale" => {
                    let mut f = value.split_whitespace();
                    let num = |v: &str| v.parse::<f64>().ok().filter(|s| (0.5..=8.0).contains(s));
                    match (f.next(), f.next(), f.next()) {
                        (Some("auto"), None, None) => t.scaleauto = true,
                        (Some("auto"), Some(v), None) => (t.scaleauto, t.scale) = (true, num(v).ok_or_else(bad)?),
                        (Some(v), None, None) => (t.scaleauto, t.scale) = (false, num(v).ok_or_else(bad)?),
                        _ => return Err(bad()),
                    }
                }
                _ => return Err(format!("theme: line {}: unknown {name}", i + 1)),
            }
        }
        *self = t;
        Ok(())
    }

    /// the theme as its file has it
    pub fn text(&self) -> String {
        let mut s = String::new();
        for (n, v) in [
            ("back", self.back),
            ("border", self.border),
            ("borderinactive", self.borderinactive),
            ("window", self.window),
            ("text", self.text),
            ("select", self.select),
            ("menuhigh", self.menuhigh),
            ("scroll", self.scroll),
            ("hold", self.hold),
            ("holdinactive", self.holdinactive),
        ] {
            let _ = writeln!(s, "{n} #{v:06x}");
        }
        let _ = writeln!(s, "title {}", self.title as u8);
        let _ = writeln!(s, "scale {}{}", if self.scaleauto { "auto " } else { "" }, self.scale);
        for (n, v) in [("font", &self.font), ("uifont", &self.uifont), ("monofont", &self.monofont)] {
            if !v.is_empty() {
                let _ = writeln!(s, "{n} {v}");
            }
        }
        s
    }
}

/// the dots per inch of a factor: round(96·scale)
pub fn dpi(scale: f64) -> i32 {
    (96.0 * scale).round() as i32
}

/// n pixels at 96 dpi at dpi (libdraw's scalesize(2): rounded)
pub fn scalesize(dpi: i32, n: i32) -> i32 {
    if dpi <= 0 { n } else { (n * dpi + 48) / 96 }
}

/// the factor for a screen of px pixels across mm millimetres (an EDID's):
/// 1 below 120 dpi, else dpi/96 down to a quarter
pub fn autoscale(px: i32, mm: i32) -> f64 {
    if px <= 0 || mm <= 0 {
        return 1.0;
    }
    let dpi = px as f64 * 25.4 / mm as f64;
    if dpi < 120.0 { 1.0 } else { ((dpi / 96.0 * 4.0).floor() / 4.0).max(1.0) }
}

/// the theme of the window system the program runs in (its /dev/theme: wsys');
/// classic without one (rio)
pub fn current() -> Theme {
    let mut t = Theme::classic();
    if let Ok(s) = std::fs::read_to_string("/dev/theme") {
        let _ = t.apply(&s);
    }
    t
}
