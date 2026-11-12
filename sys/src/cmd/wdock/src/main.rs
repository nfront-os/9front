// SPDX-License-Identifier: MIT
//
// wdock: the desktop's dock, wdock(1). It runs in a window of wsys(4) and makes
// it a bar along the bottom of the screen (wctl's bar: no border, above the
// other windows, the mouse without the focus; a window maximized leaves it
// free). On it, in the middle: starters, each a command from a file and the
// windows that are its own (/dev/wsys: those whose label starts with the
// starter's word), marked below the label when there are any. All through
// files, as wbar and winwatch:
//	button 1: none of its windows: the command runs; else one of them comes
//		up (made current, back if hidden); on its current one, the next of
//		them, or, the only one, it is hidden
//	button 2: the command runs, another one
//
// usage: wdock [-d dock] [-f font.ttf] [-s size]
//	-d	the starters: lines "label<tab>rc command[<tab>word]" ($home/lib/wdock/dock,
//		else /lib/wdock/dock)
//	-f	the font (the theme's uifont, else /lib/font/ttf/Go-Regular.ttf)
//	-s	its size in pixels (15)
// WDOCKDEBUG=items: where things are on the dock, after each picture, in
// /tmp/wdock.<pid>.items (tests): "dock x0 y0 x1 y1" (its window), then
// "item i x0 x1 nwin state label" (state: current, open, none).

use desk::text;
use desk::wins::{self, Win};
use p9win::draw::{Draw, Rect};
use p9win::{Input, Inputs};
use std::sync::mpsc;
use std::time::Duration;

const PAD: i32 = 14; // around a starter's label
const GAP: i32 = 4; // between starters
const MARK: i32 = 3; // the mark of its windows: this high, below the label

enum Ev {
    In(Input),
    Tick,
}

#[derive(Clone)]
struct Starter {
    label: String,
    cmd: String,
    word: String, // its windows' labels start with it
}

struct Dock {
    draw: Draw,
    text: text::Text,
    theme: desk::theme::Theme,
    own: u32,
    h: i32,
    starters: Vec<Starter>,
    wins: Vec<Win>,
    spots: Vec<(usize, i32, i32)>, // each starter's place: i, x0, x1
    press: Option<(usize, u32)>,   // a button went down on starter i
    buttons: u32,
    px: Vec<u32>,
    debug: Option<String>,
    dpi: i32,         // the window system's (p9win::dpi): what sizes are scaled by
    fontpath: String, // the font, its size at 96 dpi
    size: f32,
}

/// the dock's file: lines "label<tab>command[<tab>word]", # a comment
fn readdock(path: &str) -> Vec<Starter> {
    let Ok(s) = std::fs::read_to_string(path) else { return vec![] };
    s.lines()
        .filter(|l| !l.starts_with('#'))
        .filter_map(|l| {
            let f: Vec<&str> = l.split('\t').map(|f| f.trim()).collect();
            let (label, cmd) = (*f.first()?, *f.get(1)?);
            if label.is_empty() || cmd.is_empty() {
                return None;
            }
            let word = f.get(2).filter(|w| !w.is_empty()).unwrap_or(&label);
            Some(Starter { label: label.to_string(), cmd: cmd.to_string(), word: word.to_string() })
        })
        .collect()
}

impl Dock {
    /// starter i's windows, by number
    fn mine(&self, i: usize) -> Vec<&Win> {
        let w = &self.starters[i].word;
        self.wins.iter().filter(|x| x.label.starts_with(w.as_str())).collect()
    }

    fn picture(&mut self) {
        let c = self.draw.content();
        let (w, h) = (c.dx().max(1) as usize, c.dy().max(1) as usize);
        if self.px.len() != w * h {
            self.px = vec![0; w * h];
        }
        let t = self.theme.clone();
        let fill = |px: &mut Vec<u32>, r: Rect, col: u32| {
            for y in r.y0.max(0)..r.y1.min(h as i32) {
                for x in r.x0.max(0)..r.x1.min(w as i32) {
                    px[y as usize * w + x as usize] = col;
                }
            }
        };
        // sizes at the window system's scale (libdraw's scalesize)
        let dpi = self.dpi;
        let k = move |n: i32| p9win::draw::scalesize(dpi, n);
        let (pad, gap, mark, line) = (k(PAD), k(GAP), k(MARK), k(2));
        let (bw, bh) = (w as i32, h as i32);
        fill(&mut self.px, Rect::new(0, 0, bw, bh), t.window);
        fill(&mut self.px, Rect::new(0, 0, bw, line), t.border);
        let ty = line + (bh - 2 * line - mark - self.text.height) / 2;
        let mut dbg = format!("dock {} {} {} {}\n", c.x0, c.y0, c.x1, c.y1);
        // the starters in the middle, as many as fit
        let widths: Vec<i32> = (0..self.starters.len()).map(|i| self.text.width(&self.starters[i].label.clone()) + 2 * pad).collect();
        let mut n = widths.len();
        while n > 0 && widths[..n].iter().sum::<i32>() + gap * (n as i32 - 1) > bw {
            n -= 1;
        }
        let total = widths[..n].iter().sum::<i32>() + gap * (n as i32 - 1).max(0);
        let mut x = (bw - total) / 2;
        self.spots.clear();
        for i in 0..n {
            let iw = widths[i];
            let mine: Vec<Win> = self.mine(i).into_iter().cloned().collect();
            let current = mine.iter().any(|w| w.current);
            if current {
                fill(&mut self.px, Rect::new(x, 2 * line, x + iw, bh - line), t.select);
            }
            let label = self.starters[i].label.clone();
            let ink = if !mine.is_empty() && mine.iter().all(|w| w.hidden) { text::mix(t.window, t.text, 110) } else { t.text };
            self.text.draw(&mut self.px, w, h, x + pad, ty, &label, ink, x + iw);
            if !mine.is_empty() {
                // the mark: one dash per window, three at most
                let nd = mine.len().min(3) as i32;
                let (dw, dg, up) = (k(8), k(4), k(1));
                let mx = x + (iw - (nd * dw + (nd - 1) * dg)) / 2;
                for j in 0..nd {
                    let x0 = mx + j * (dw + dg);
                    fill(&mut self.px, Rect::new(x0, bh - line - mark - up, x0 + dw, bh - line - up), t.border);
                }
            }
            self.spots.push((i, x, x + iw));
            let state = if current { "current" } else if mine.is_empty() { "none" } else { "open" };
            dbg += &format!("item {i} {x} {} {} {state} {label}\n", x + iw, mine.len());
            x += iw + gap;
        }
        let _ = self.draw.put(&self.px, w, h, &[]);
        if let Some(p) = &self.debug {
            let _ = std::fs::write(p, dbg);
        }
    }

    /// the windows again; true if they changed
    fn update(&mut self) -> bool {
        let wins = wins::list(self.own);
        let changed = wins != self.wins;
        self.wins = wins;
        changed
    }

    /// another scale (the window system's dpi): the font and the dock's height anew
    fn rescale(&mut self) {
        let dpi = p9win::dpi();
        if dpi == self.dpi {
            return;
        }
        match text::Text::open(&self.fontpath, self.size * dpi as f32 / 96.0) {
            Ok(t) => self.text = t,
            Err(e) => eprintln!("wdock: font: {e}"),
        }
        self.dpi = dpi;
        self.h = height(&self.text, dpi);
    }

    /// the window along the bottom of the screen, the screen's width
    fn place(&mut self) {
        let s = self.draw.screen();
        let want = Rect::new(s.x0, s.y1 - self.h, s.x1, s.y1);
        if self.draw.content() != want {
            let _ = p9win::wctl(&format!("resize -r {} {} {} {}", want.x0, want.y0, want.x1, want.y1));
        }
    }

    fn mouse(&mut self, x: i32, y: i32, b: u32) {
        let c = self.draw.content();
        let x = x - c.x0;
        let inside = y >= c.y0 && y < c.y1;
        let prev = self.buttons;
        self.buttons = b;
        let (pressed, released) = (b & !prev, prev & !b);
        let at = || self.spots.iter().find(|s| inside && x >= s.1 && x < s.2).map(|s| s.0);
        if pressed & 3 != 0 && prev == 0 {
            self.press = at().map(|i| (i, pressed & 3));
        }
        if released != 0 && b == 0 {
            if let Some((i, but)) = self.press.take().filter(|p| Some(p.0) == at()) {
                self.choose(i, but);
            }
        }
    }

    /// button but (1 or 2) on starter i
    fn choose(&mut self, i: usize, but: u32) {
        let mine: Vec<Win> = self.mine(i).into_iter().cloned().collect();
        if but & 2 != 0 || mine.is_empty() {
            wins::run(&self.starters[i].cmd.clone(), "wdock");
        } else if let Some(k) = mine.iter().position(|w| w.current) {
            if mine.len() == 1 {
                wins::wctl(mine[0].n, &["hide"], "wdock");
            } else {
                let next = &mine[(k + 1) % mine.len()];
                up(next);
            }
        } else {
            // the first not hidden, else the first
            up(mine.iter().find(|w| !w.hidden).unwrap_or(&mine[0]));
        }
        if self.update() {
            self.picture();
        }
    }
}

/// the dock's height: the text, its line along the top, room around, the marks
fn height(text: &text::Text, dpi: i32) -> i32 {
    let k = |n| p9win::draw::scalesize(dpi, n);
    text.height + k(2) + 2 * k(6) + k(MARK)
}

/// window w current, on top, back if hidden
fn up(w: &Win) {
    if w.hidden {
        wins::wctl(w.n, &["unhide", "current", "top"], "wdock");
    } else {
        wins::wctl(w.n, &["current", "top"], "wdock");
    }
}

fn usage() -> ! {
    eprintln!("usage: wdock [-d dock] [-f font.ttf] [-s size]");
    std::process::exit(1);
}

fn main() {
    let theme = desk::theme::current();
    let home = std::env::var("home").unwrap_or_default();
    let mine = format!("{home}/lib/wdock/dock");
    let mut dockpath = if std::path::Path::new(&mine).exists() { mine } else { "/lib/wdock/dock".to_string() };
    let mut fontpath = if theme.uifont.is_empty() { "/lib/font/ttf/Go-Regular.ttf".to_string() } else { theme.uifont.clone() };
    let mut size = 15.0f32;
    let mut args = std::env::args().skip(1);
    while let Some(a) = args.next() {
        match a.as_str() {
            "-d" => dockpath = args.next().unwrap_or_else(|| usage()),
            "-f" => fontpath = args.next().unwrap_or_else(|| usage()),
            "-s" => size = args.next().and_then(|s| s.parse().ok()).unwrap_or_else(|| usage()),
            _ => usage(),
        }
    }
    let die = |what: &str, e: std::io::Error| -> ! {
        eprintln!("wdock: {what}: {e}");
        std::process::exit(1);
    };
    let starters = readdock(&dockpath);
    if starters.is_empty() {
        eprintln!("wdock: {dockpath}: no starters");
        std::process::exit(1);
    }
    let dpi = p9win::dpi();
    let text = text::Text::open(&fontpath, size * dpi as f32 / 96.0).unwrap_or_else(|e| die("font", e));
    let draw = Draw::open().unwrap_or_else(|e| die("draw", e));
    let (tx, rx) = mpsc::channel();
    let h = height(&text, dpi);
    let debug = std::env::var("WDOCKDEBUG").is_ok_and(|v| v.contains("items")).then(|| format!("/tmp/wdock.{}.items", std::process::id()));
    let mut dock = Dock { draw, text, theme, own: wins::own(), h, starters, wins: vec![], spots: vec![], press: None, buttons: 0, px: vec![], debug, dpi, fontpath, size };
    let _ = p9win::set_label("wdock");
    let _ = p9win::wctl("bar");
    dock.place();

    let itx = tx.clone();
    let _inputs = Inputs::start(move |i| {
        let _ = itx.send(Ev::In(i));
    })
    .unwrap_or_else(|e| die("input", e));
    std::thread::spawn(move || {
        while tx.send(Ev::Tick).is_ok() {
            std::thread::sleep(Duration::from_millis(500));
        }
    });
    if let Err(e) = dock.draw.attach() {
        die("attach", e);
    }
    dock.update();
    dock.picture();
    while let Ok(ev) = rx.recv() {
        match ev {
            Ev::Tick => {
                if dock.update() {
                    dock.picture();
                }
            }
            Ev::In(Input::Closed) => break,
            Ev::In(Input::Mouse { x, y, buttons, resized }) => {
                if resized {
                    if let Err(e) = dock.draw.attach() {
                        die("attach", e);
                    }
                    // the screen changed (back to its bottom), the theme, the scale
                    dock.rescale();
                    dock.place();
                    dock.theme = desk::theme::current();
                    dock.picture();
                }
                dock.mouse(x, y, buttons);
            }
            Ev::In(_) => {}
        }
    }
}
