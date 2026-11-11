// SPDX-License-Identifier: MIT
//
// the desktop's theme (../9front-ui PLAN stage 7): its colours and fonts as text,
// a line "name value" each (one starting with # a comment), as /lib/theme/* and the file
// theme hold them (wsys(4)); colours #rrggbb. What a line does not name stays.
// classic is wsys' own (rio's colours); ui and term read the same file.

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
        }
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
        for (n, v) in [("font", &self.font), ("uifont", &self.uifont), ("monofont", &self.monofont)] {
            if !v.is_empty() {
                let _ = writeln!(s, "{n} {v}");
            }
        }
        s
    }
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
