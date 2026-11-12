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
// /tmp/wbar.<pid>.items (tests): "menu x0 x1 open|shut", "win n x0 x1 state
// label", "clock x0 x1 text", "sys x0 x1 text".
// Before the clock the state of the machine, as stats(8) reads it: the load of
// the processors (/dev/sysstat: what is not idle), memory in use (/dev/swap),
// the battery where there is one (stats' files).
// The menu is wsys': its window's menu file (wsys(4)) shows it below the bar
// and says which item was chosen.

use desk::text;
use desk::wins::{self, Win};
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
    Chosen(i32), // the menu's item, -1 none
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
    sys: String,                       // the state of the machine (sysstate), before the clock
    sysat: Option<std::time::Instant>, // when it was read
    menu: Vec<(String, String)>,
    open: bool,         // wsys shows the menu
    press: Option<u32>, // button 1 went down on this window's place
    buttons: u32,
    px: Vec<u32>,
    pw: usize,
    ph: usize,
    debug: Option<String>,
    tx: mpsc::Sender<Ev>,
}

/// the clock's text, from date(1) (the local time zone): weekday, day, month, hours:minutes
fn clock() -> String {
    std::process::Command::new("date")
        .args(["-f", "WW D MMM hh:mm"])
        .output()
        .map(|o| String::from_utf8_lossy(&o.stdout).trim().to_string())
        .unwrap_or_default()
}

/// the state of the machine: the load of the processors (what /dev/sysstat
/// says is not idle, their mean), the memory in use (/dev/swap: the user
/// pages, in pages of its pagesize), the battery's charge where there is one
fn sysstate() -> String {
    let mut out = vec![];
    if let Ok(s) = std::fs::read_to_string("/dev/sysstat") {
        let idle: Vec<u64> = s.lines().filter_map(|l| l.split_whitespace().nth(8)?.parse().ok()).collect();
        if !idle.is_empty() {
            let busy = 100u64.saturating_sub(idle.iter().sum::<u64>() / idle.len() as u64);
            out.push(format!("cpu {busy}%"));
        }
    }
    if let Ok(s) = std::fs::read_to_string("/dev/swap") {
        let field = |name: &str| s.lines().find(|l| l.split_whitespace().nth(1) == Some(name)).and_then(|l| l.split_whitespace().next().map(|v| v.to_string()));
        let pagesize: u64 = field("pagesize").and_then(|v| v.parse().ok()).unwrap_or(4096);
        if let Some((used, max)) = field("user").as_deref().and_then(|u| u.split_once('/')) {
            let (used, max): (u64, u64) = (used.parse().unwrap_or(0), max.parse().unwrap_or(0));
            let g = |p: u64| p as f64 * pagesize as f64 / (1u64 << 30) as f64;
            out.push(format!("mem {:.1}/{:.1}G", g(used), g(max)));
        }
    }
    for f in ["/dev/battery", "/mnt/pm/battery", "/mnt/apm/battery", "/shr/usb/battery"] {
        if let Ok(s) = std::fs::read_to_string(f) {
            let v: Vec<u64> = s.split_whitespace().filter_map(|x| x.parse().ok()).collect();
            if v.len() >= 3 && v[2] > 0 {
                out.push(format!("bat {}%", 100 * v[1] / v[2]));
                break;
            }
        }
    }
    out.join("  ")
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

impl Bar {
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
        if self.open {
            fill(&mut self.px, Rect::new(0, 0, mw, bh - 2), t.menuhigh);
        }
        self.text.draw(&mut self.px, w, h, PAD, ty, MENU, ink, mw);
        dbg += &format!("menu 0 {mw} {}\n", if self.open { "open" } else { "shut" });
        // the clock
        let cw = self.text.width(&self.clock);
        let cx = bw - cw - PAD;
        let clock = self.clock.clone();
        self.text.draw(&mut self.px, w, h, cx, ty, &clock, ink, bw);
        dbg += &format!("clock {cx} {bw} {clock}\n");
        // the state of the machine, before the clock
        let sw = self.text.width(&self.sys);
        let sx = cx - 2 * PAD - sw;
        let sys = self.sys.clone();
        let cx = if sw > 0 && sx > mw + 4 * PAD {
            self.text.draw(&mut self.px, w, h, sx, ty, &sys, pale, cx);
            dbg += &format!("sys {sx} {} {sys}\n", sx + sw);
            sx
        } else {
            cx
        };
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
        let _ = self.draw.put(&self.px, w, h, &[]);
        if let Some(p) = &self.debug {
            let _ = std::fs::write(p, dbg);
        }
    }

    /// the windows and the clock again; true if they changed
    fn update(&mut self) -> bool {
        let wins = wins::list(self.own);
        let now = std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).map_or(0, |d| d.as_secs() / 60);
        let mut changed = wins != self.wins;
        self.wins = wins;
        if now != self.minute {
            self.minute = now;
            let c = clock();
            changed |= c != self.clock;
            self.clock = c;
        }
        if self.sysat.is_none_or(|t| t.elapsed() >= Duration::from_secs(2)) {
            self.sysat = Some(std::time::Instant::now());
            let st = sysstate();
            changed |= st != self.sys;
            self.sys = st;
        }
        changed
    }

    /// the window: the screen's width, the bar's height
    fn resize(&mut self) {
        let s = self.draw.screen();
        let _ = p9win::wctl(&format!("resize -r {} {} {} {}", s.x0, s.y0, s.x1, s.y0 + self.h));
    }

    /// the menu below its place on the bar, by wsys (its window's menu file:
    /// written, then read for the item chosen; read in a thread of its own)
    fn openmenu(&mut self) {
        let c = self.draw.content();
        let mut req = format!("{} {}\n", c.x0 + self.menux.0, c.y0 + self.h);
        for (label, _) in &self.menu {
            req += label;
            req.push('\n');
        }
        let tx = self.tx.clone();
        let ask = move || -> std::io::Result<i32> {
            let mut f = std::fs::OpenOptions::new().read(true).write(true).open("/dev/menu")?;
            std::io::Write::write_all(&mut f, req.as_bytes())?;
            let mut b = [0u8; 32];
            let n = f.read(&mut b)?;
            Ok(String::from_utf8_lossy(&b[..n]).trim().parse().unwrap_or(-1))
        };
        self.open = true;
        self.picture();
        std::thread::spawn(move || {
            let i = ask().unwrap_or_else(|e| {
                eprintln!("wbar: menu: {e}");
                -1
            });
            let _ = tx.send(Ev::Chosen(i));
        });
    }

    /// the menu's item i chosen (-1: none)
    fn chosen(&mut self, i: i32) {
        self.open = false;
        self.picture();
        if let Some((_, cmd)) = usize::try_from(i).ok().and_then(|i| self.menu.get(i)) {
            wins::run(&cmd.clone(), "wbar");
        }
    }

    fn mouse(&mut self, x: i32, y: i32, b: u32) {
        let c = self.draw.content();
        let (x, y) = (x - c.x0, y - c.y0);
        let prev = self.buttons;
        self.buttons = b;
        let pressed = b & !prev;
        let released = prev & !b;
        let onmenu = y < self.h && x >= self.menux.0 && x < self.menux.1;
        if pressed & 1 != 0 {
            if onmenu {
                if !self.open && !self.menu.is_empty() {
                    self.openmenu();
                }
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
        wins::wctl(n, msgs, "wbar");
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
    let (tx, rx) = mpsc::channel();
    let own = wins::own();
    let h = text.height + 8;
    let debug = std::env::var("WBARDEBUG").is_ok_and(|v| v.contains("items")).then(|| format!("/tmp/wbar.{}.items", std::process::id()));
    let mut bar = Bar {
        draw, text, theme, own, h, wins: vec![], spots: vec![], menux: (0, 0), clock: String::new(), minute: 0, sys: String::new(), sysat: None,
        menu: readmenu(&menupath), open: false, press: None, buttons: 0, px: vec![], pw: 0, ph: 0, debug, tx: tx.clone(),
    };
    let _ = p9win::set_label("wbar");
    // a bar along the top of the screen (rio has no bars: there it is a window)
    let _ = p9win::wctl("bar");
    bar.resize();

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
            Ev::Chosen(i) => bar.chosen(i),
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
