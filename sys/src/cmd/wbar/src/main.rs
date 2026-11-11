// SPDX-License-Identifier: MIT
//
// wbar: the desktop's bar, wbar(1). It runs in a window of wsys(4) and makes it
// a bar along the top of the screen (wctl's bar: no border, above the other
// windows, the mouse without the focus). On it: a menu at the start (commands
// from a file), the windows (/dev/wsys: their labels; the current one lit, a
// hidden one paler) and a clock at the end. All through files, as winwatch:
// button 1 on a window makes it current, on the current one hides it, on a
// hidden one brings it back.
//
// usage: wbar [-m menu] [-f font.ttf] [-s size]
//	-m	the menu: lines "label<tab>rc command" ($home/lib/wbar/menu, else /lib/wbar/menu)
//	-f	the font (the theme's uifont, else /lib/font/ttf/Go-Regular.ttf)
//	-s	its size in pixels (15)
// WBARDEBUG=items: where things are on the bar, after each picture, in
// /tmp/wbar.<pid>.items (tests): "menu x0 x1", "win n x0 x1 state label",
// "clock x0 x1 text", "item i y0 y1 label" while the menu is open.

mod text;

use p9win::draw::{Draw, Rect};
use p9win::{Input, Inputs};
use std::io::Read;
use std::sync::mpsc;
use std::time::Duration;

const PAD: i32 = 8; // around a thing's text
const MAXITEM: i32 = 200; // a window's widest place on the bar
const MENU: &str = "menu";

enum Ev {
    In(Input),
    Tick,
}

#[derive(Clone, PartialEq)]
struct Win {
    n: u32,
    label: String,
    current: bool,
    hidden: bool,
}

struct Bar {
    draw: Draw,
    text: text::Text,
    theme: desk::theme::Theme,
    own: u32,
    h: i32, // the bar's height (the window's when the menu is shut)
    wins: Vec<Win>,
    spots: Vec<(u32, i32, i32)>, // each window's place on the bar: n, x0, x1
    menux: (i32, i32),
    clock: String,
    minute: u64,
    menu: Vec<(String, String)>,
    open: Option<Option<usize>>, // the menu open, the item lit
    opened: bool,                // the press that opened it is still held
    press: Option<u32>,          // button 1 went down on this window's place
    buttons: u32,
    px: Vec<u32>,
    pw: usize,
    ph: usize,
    debug: Option<String>,
}

/// one read of a file (wctl: a second read would wait for a change)
fn read1(path: &str) -> Option<String> {
    let mut f = std::fs::File::open(path).ok()?;
    let mut b = [0u8; 512];
    let n = f.read(&mut b).ok()?;
    Some(String::from_utf8_lossy(&b[..n]).into_owned())
}

/// the windows of /dev/wsys, by number
fn windows(own: u32) -> Vec<Win> {
    let mut v = vec![];
    let Ok(d) = std::fs::read_dir("/dev/wsys") else { return v };
    for e in d.flatten() {
        let Ok(n) = e.file_name().to_string_lossy().parse::<u32>() else { continue };
        if n == own {
            continue;
        }
        let Some(w) = read1(&format!("/dev/wsys/{n}/wctl")) else { continue };
        let f: Vec<&str> = w.split_whitespace().collect();
        let label = read1(&format!("/dev/wsys/{n}/label")).unwrap_or_default();
        let label = label.trim();
        let label = if label.is_empty() { format!("window {n}") } else { label.to_string() };
        v.push(Win { n, label, current: f.get(4) == Some(&"current"), hidden: f.get(5) == Some(&"hidden") });
    }
    v.sort_by_key(|w| w.n);
    v
}

/// the clock's text, from date(1) (the local time zone): weekday, day, month, hours:minutes
fn clock() -> String {
    std::process::Command::new("date")
        .args(["-f", "WW D MMM hh:mm"])
        .output()
        .map(|o| String::from_utf8_lossy(&o.stdout).trim().to_string())
        .unwrap_or_default()
}

/// the menu's file: lines "label<tab>command", # a comment
fn readmenu(path: &str) -> Vec<(String, String)> {
    let Ok(s) = std::fs::read_to_string(path) else { return vec![] };
    s.lines()
        .filter(|l| !l.starts_with('#'))
        .filter_map(|l| l.split_once('\t'))
        .map(|(a, b)| (a.trim().to_string(), b.trim().to_string()))
        .filter(|(a, b)| !a.is_empty() && !b.is_empty())
        .collect()
}

/// cmd run by rc, in wbar's name space, not waited for here
fn run(cmd: &str) {
    match std::process::Command::new("rc").args(["-c", cmd]).spawn() {
        Ok(mut c) => {
            std::thread::spawn(move || {
                let _ = c.wait();
            });
        }
        Err(e) => eprintln!("wbar: rc: {e}"),
    }
}

impl Bar {
    fn rowh(&self) -> i32 {
        self.text.height + 6
    }

    /// the menu's place below the bar, in the window
    fn menurect(&mut self) -> Rect {
        let labels: Vec<String> = self.menu.iter().map(|m| m.0.clone()).collect();
        let w = labels.iter().map(|l| self.text.width(l)).max().unwrap_or(0) + 2 * PAD + 4;
        Rect::new(self.menux.0, self.h, self.menux.0 + w, self.h + self.menu.len() as i32 * self.rowh() + 4)
    }

    fn picture(&mut self) {
        let c = self.draw.content();
        let (w, h) = (c.dx().max(1) as usize, c.dy().max(1) as usize);
        if self.px.len() != w * h {
            self.px = vec![0; w * h];
        }
        (self.pw, self.ph) = (w, h);
        let t = self.theme.clone();
        let ink = t.text;
        let pale = text::mix(t.window, t.text, 110);
        let fill = |px: &mut Vec<u32>, r: Rect, col: u32| {
            for y in r.y0.max(0)..r.y1.min(h as i32) {
                for x in r.x0.max(0)..r.x1.min(w as i32) {
                    px[y as usize * w + x as usize] = col;
                }
            }
        };
        let (bw, bh) = (w as i32, self.h);
        fill(&mut self.px, Rect::new(0, 0, bw, h as i32), t.window);
        fill(&mut self.px, Rect::new(0, bh - 2, bw, bh), t.border);
        let ty = (bh - 2 - self.text.height) / 2;
        let mut dbg = String::new();
        // the menu's place
        let mw = self.text.width(MENU) + 2 * PAD;
        self.menux = (0, mw);
        if self.open.is_some() {
            fill(&mut self.px, Rect::new(0, 0, mw, bh - 2), t.menuhigh);
        }
        self.text.draw(&mut self.px, w, h, PAD, ty, MENU, ink, mw);
        dbg += &format!("menu 0 {mw}\n");
        // the clock
        let cw = self.text.width(&self.clock);
        let cx = bw - cw - PAD;
        let clock = self.clock.clone();
        self.text.draw(&mut self.px, w, h, cx, ty, &clock, ink, bw);
        dbg += &format!("clock {cx} {bw} {clock}\n");
        // the windows, between
        let mut x = mw + PAD;
        self.spots.clear();
        let wins = self.wins.clone();
        for win in &wins {
            let room = (cx - PAD - x).min(MAXITEM);
            if room < 3 * PAD {
                break;
            }
            let label = self.text.fit(&win.label, room - 2 * PAD);
            let iw = self.text.width(&label) + 2 * PAD;
            if win.current {
                fill(&mut self.px, Rect::new(x, 2, x + iw, bh - 4), t.select);
            }
            self.text.draw(&mut self.px, w, h, x + PAD, ty, &label, if win.hidden { pale } else { ink }, x + iw);
            self.spots.push((win.n, x, x + iw));
            let state = if win.current { "current" } else if win.hidden { "hidden" } else { "visible" };
            dbg += &format!("win {} {} {} {} {}\n", win.n, x, x + iw, state, win.label);
            x += iw + 2;
        }
        // the menu, open below the bar
        if let Some(lit) = self.open {
            let r = self.menurect();
            fill(&mut self.px, r, t.border);
            fill(&mut self.px, r.inset(2), t.window);
            let rh = self.rowh();
            let items = self.menu.clone();
            for (i, (label, _)) in items.iter().enumerate() {
                let y = r.y0 + 2 + i as i32 * rh;
                if lit == Some(i) {
                    fill(&mut self.px, Rect::new(r.x0 + 2, y, r.x1 - 2, y + rh), t.menuhigh);
                }
                self.text.draw(&mut self.px, w, h, r.x0 + 2 + PAD, y + 3, label, ink, r.x1 - 2);
                dbg += &format!("item {i} {y} {} {label}\n", y + rh);
            }
        }
        let _ = self.draw.put(&self.px, w, h, &[]);
        if let Some(p) = &self.debug {
            let _ = std::fs::write(p, dbg);
        }
    }

    /// the windows and the clock again; true if they changed
    fn update(&mut self) -> bool {
        let wins = windows(self.own);
        let now = std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).map_or(0, |d| d.as_secs() / 60);
        let mut changed = wins != self.wins;
        self.wins = wins;
        if now != self.minute {
            self.minute = now;
            let c = clock();
            changed |= c != self.clock;
            self.clock = c;
        }
        changed
    }

    /// the window's height: the bar's, or with the menu below it
    fn resize(&mut self) {
        let s = self.draw.screen();
        let h = if self.open.is_some() { self.menurect().y1 } else { self.h };
        let _ = p9win::wctl(&format!("resize -r {} {} {} {}", s.x0, s.y0, s.x1, s.y0 + h));
    }

    fn setmenu(&mut self, open: bool) {
        if open == self.open.is_some() {
            return;
        }
        self.open = if open { Some(None) } else { None };
        self.resize();
        self.picture();
    }

    fn mouse(&mut self, x: i32, y: i32, b: u32) {
        let c = self.draw.content();
        let (x, y) = (x - c.x0, y - c.y0);
        let prev = self.buttons;
        self.buttons = b;
        let pressed = b & !prev;
        let released = prev & !b;
        let onmenu = y < self.h && x >= self.menux.0 && x < self.menux.1;
        if self.open.is_some() {
            let r = self.menurect();
            let lit = (r.inset(2).contains(x, y)).then(|| ((y - r.y0 - 2) / self.rowh()) as usize).filter(|&i| i < self.menu.len());
            if self.open != Some(lit) {
                self.open = Some(lit);
                self.picture();
            }
            if released & 1 != 0 {
                if let Some(i) = lit {
                    let cmd = self.menu[i].1.clone();
                    self.setmenu(false);
                    run(&cmd);
                } else if !(onmenu && self.opened) {
                    self.setmenu(false); // let go elsewhere: shut (a click on menu keeps it open)
                }
                self.opened = false;
            } else if pressed & 1 != 0 && lit.is_none() {
                self.setmenu(false);
            }
            return;
        }
        if pressed & 1 != 0 {
            if onmenu {
                self.opened = true;
                self.setmenu(true);
                return;
            }
            self.press = self.spots.iter().find(|s| x >= s.1 && x < s.2 && y < self.h).map(|s| s.0);
        }
        if released & 1 != 0 {
            let at = self.spots.iter().find(|s| x >= s.1 && x < s.2 && y < self.h).map(|s| s.0);
            if let Some(n) = self.press.take().filter(|&n| Some(n) == at) {
                self.choose(n);
            }
        }
    }

    /// button 1 on window n's place: current, hidden when it was, back when hidden
    fn choose(&mut self, n: u32) {
        let Some(w) = self.wins.iter().find(|w| w.n == n) else { return };
        let msgs: &[&str] = if w.hidden {
            &["unhide", "current"]
        } else if w.current {
            &["hide"]
        } else {
            &["current"]
        };
        for m in msgs {
            if let Err(e) = std::fs::write(format!("/dev/wsys/{n}/wctl"), m) {
                eprintln!("wbar: window {n}: {m}: {e}");
            }
        }
        if self.update() {
            self.picture();
        }
    }
}

fn usage() -> ! {
    eprintln!("usage: wbar [-m menu] [-f font.ttf] [-s size]");
    std::process::exit(1);
}

fn main() {
    let theme = desk::theme::current();
    let home = std::env::var("home").unwrap_or_default();
    let mine = format!("{home}/lib/wbar/menu");
    let mut menupath = if std::path::Path::new(&mine).exists() { mine } else { "/lib/wbar/menu".to_string() };
    let mut fontpath = if theme.uifont.is_empty() { "/lib/font/ttf/Go-Regular.ttf".to_string() } else { theme.uifont.clone() };
    let mut size = 15.0f32;
    let mut args = std::env::args().skip(1);
    while let Some(a) = args.next() {
        match a.as_str() {
            "-m" => menupath = args.next().unwrap_or_else(|| usage()),
            "-f" => fontpath = args.next().unwrap_or_else(|| usage()),
            "-s" => size = args.next().and_then(|s| s.parse().ok()).unwrap_or_else(|| usage()),
            _ => usage(),
        }
    }
    let die = |what: &str, e: std::io::Error| -> ! {
        eprintln!("wbar: {what}: {e}");
        std::process::exit(1);
    };
    let text = text::Text::open(&fontpath, size).unwrap_or_else(|e| die("font", e));
    let draw = Draw::open().unwrap_or_else(|e| die("draw", e));
    let own = read1("/dev/winid").and_then(|s| s.trim().parse().ok()).unwrap_or(0);
    let h = text.height + 8;
    let debug = std::env::var("WBARDEBUG").is_ok_and(|v| v.contains("items")).then(|| format!("/tmp/wbar.{}.items", std::process::id()));
    let mut bar = Bar {
        draw, text, theme, own, h, wins: vec![], spots: vec![], menux: (0, 0), clock: String::new(), minute: 0,
        menu: readmenu(&menupath), open: None, opened: false, press: None, buttons: 0, px: vec![], pw: 0, ph: 0, debug,
    };
    let _ = p9win::set_label("wbar");
    // a bar along the top of the screen (rio has no bars: there it is a window)
    let _ = p9win::wctl("bar");
    bar.resize();

    let (tx, rx) = mpsc::channel();
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
    if let Err(e) = bar.draw.attach() {
        die("attach", e);
    }
    bar.update();
    bar.picture();
    while let Ok(ev) = rx.recv() {
        match ev {
            Ev::Tick => {
                if bar.update() {
                    bar.picture();
                }
            }
            Ev::In(Input::Closed) => break,
            Ev::In(Input::Mouse { x, y, buttons, resized }) => {
                if resized {
                    if let Err(e) = bar.draw.attach() {
                        die("attach", e);
                    }
                    // the window changed, or the desktop's theme (wsys says either so)
                    let th = desk::theme::current();
                    if th != bar.theme {
                        bar.theme = th;
                    }
                    bar.picture();
                }
                bar.mouse(x, y, buttons);
            }
            Ev::In(_) => {}
        }
    }
}
