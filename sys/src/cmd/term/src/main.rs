// SPDX-License-Identifier: MIT
//
// term: a terminal in a window of wsys or rio, term(1). Its program runs on a
// pty of ptyfs(4) (rc -i by default): Plan 9's programs read and write
// /dev/cons as in a window of rio, POSIX programs find a terminal (termios,
// TERM=xterm-256color). The screen is the vt100 crate's (xterm's sequences,
// colours, the alternate screen, scrollback); term draws it with a TrueType
// font through draw(3) (p9win), on the processor.
//
// usage: term [-r] [-f font.ttf] [-s size] [cmd [arg ...]]
//	-r	white on black
//	-f	the font (Go Mono: /lib/font/ttf/Go-Mono.ttf)
//	-s	its line height in pixels (16)
// Mouse, as in rio: button 1 selects, button 2's menu snarfs the selection,
// pastes the snarf buffer or sends (the selection or the snarf buffer typed,
// with a newline); the wheel or Shift-PgUp/PgDn scroll back; a program that
// asked for the mouse gets it unless Shift is held. Del
// interrupts where a shell reads lines (as in rio), else it is xterm's delete.
// TERMDEBUG=screen: the screen's text in /tmp/term.<pid>.screen after each
// picture, the cells' place in /tmp/term.<pid>.geom (x y of the first, width,
// height, rows, columns) at each size (tests).

mod font;
mod keys;
mod pty;
mod view;

use p9win::draw::Draw;
use p9win::{Input, Inputs};
use std::sync::mpsc;
use view::{Sel, View};
use vt100::{MouseProtocolEncoding, MouseProtocolMode};

const SCROLLBACK: usize = 10000;
/// button 2's menu (rio's, but for cut: what a terminal shows cannot be cut)
const MENU2: &[&str] = &["snarf", "paste", "send"];

enum Ev {
    In(Input),
    Out(Vec<u8>),
    End,
}

/// what the screen's program asks of the terminal itself
#[derive(Default)]
struct Replies {
    out: Vec<u8>,
    title: Option<String>,
}

impl vt100::Callbacks for Replies {
    fn unhandled_csi(&mut self, s: &mut vt100::Screen, i1: Option<u8>, _i2: Option<u8>, params: &[&[u16]], c: char) {
        let p0 = params.first().and_then(|p| p.first()).copied().unwrap_or(0);
        match (i1, c) {
            // device status: fine; where the cursor is
            (None, 'n') if p0 == 5 => self.out.extend_from_slice(b"\x1b[0n"),
            (None, 'n') if p0 == 6 => {
                let (r, c) = s.cursor_position();
                self.out.extend_from_slice(format!("\x1b[{};{}R", r + 1, c + 1).as_bytes());
            }
            // device attributes: a VT220 with colours; the second ones
            (None, 'c') if p0 == 0 => self.out.extend_from_slice(b"\x1b[?62;22c"),
            (Some(b'>'), 'c') if p0 == 0 => self.out.extend_from_slice(b"\x1b[>1;10;0c"),
            _ => {}
        }
    }

    fn set_window_title(&mut self, _: &mut vt100::Screen, title: &[u8]) {
        self.title = Some(String::from_utf8_lossy(title).into_owned());
    }
}

fn usage() -> ! {
    eprintln!("usage: term [-r] [-f font.ttf] [-s size] [cmd [arg ...]]");
    std::process::exit(1);
}

struct Term {
    draw: Draw,
    fonts: font::Fonts,
    view: View,
    vt: vt100::Parser<Replies>,
    pty: pty::Pty,
    sel: Option<Sel>,
    selecting: bool,
    buttons: u32,
    menu: Option<(p9win::draw::Rect, Option<usize>)>, // button 2's, open: where, the item lit
    shift: bool,
    current: bool,
    debug: Option<String>,
}

impl Term {
    fn send(&self, b: &[u8]) {
        if !b.is_empty() {
            let _ = self.pty.master.write(b);
        }
    }

    /// the window's size again (after a resize, at the start)
    fn size(&mut self) {
        let c = self.draw.content();
        self.view.resize(c.dx().max(1) as usize, c.dy().max(1) as usize, &self.fonts);
        let (rows, cols) = (self.view.rows, self.view.cols);
        self.vt.screen_mut().set_size(rows, cols);
        self.pty.winsize(rows, cols, c.dx(), c.dy());
        self.sel = None;
        if let Some(path) = &self.debug {
            // where the cells are on the screen: the first one's top left, a cell's size, how many
            let g = format!("{} {} {} {} {} {}\n", c.x0 + view::MARGIN, c.y0 + view::MARGIN, self.fonts.cw, self.fonts.ch, rows, cols);
            let _ = std::fs::write(path.replace(".screen", ".geom"), g);
        }
    }

    fn picture(&mut self) {
        let s = self.vt.screen();
        let cursor = (s.scrollback() == 0 && !s.hide_cursor()).then(|| s.cursor_position());
        let damage = self.view.draw(s, &mut self.fonts, self.sel, cursor, self.current);
        if self.view.fresh {
            // no rectangles: all of it
            self.view.fresh = false;
            let _ = self.draw.put(&self.view.px, self.view.w, self.view.h, &[]);
        } else if !damage.is_empty() {
            let _ = self.draw.put(&self.view.px, self.view.w, self.view.h, &damage);
        }
        if let Some(path) = &self.debug {
            let _ = std::fs::write(path, s.contents());
        }
    }

    fn scroll(&mut self, n: i32) {
        let s = self.vt.screen_mut();
        let now = s.scrollback() as i32;
        s.set_scrollback((now - n).max(0) as usize);
        self.sel = None;
    }

    fn key(&mut self, c: char) {
        let s = self.vt.screen();
        match keys::key(c, s.application_cursor(), self.shift) {
            keys::Key::Scroll(n) => {
                let rows = self.view.rows as i32;
                self.scroll(n * (rows / 2).max(1));
            }
            keys::Key::Del => match self.pty.interrupt() {
                Some(intr) => self.send(&[intr]),
                None => self.send(b"\x1b[3~"),
            },
            keys::Key::Bytes(b) => {
                // typing goes to the program's screen: back from the scrollback
                if self.vt.screen().scrollback() != 0 {
                    self.vt.screen_mut().set_scrollback(0);
                }
                self.send(&b);
            }
        }
    }

    /// the mouse as the program asked for it (xterm's protocols); false if it did not
    fn report(&self, b: u32, prev: u32, r: u16, c: u16) -> bool {
        let s = self.vt.screen();
        let mode = s.mouse_protocol_mode();
        if mode == MouseProtocolMode::None || self.shift {
            return false;
        }
        let enc = s.mouse_protocol_encoding();
        let ev = |code: u32, release: bool| -> Vec<u8> {
            match enc {
                MouseProtocolEncoding::Sgr => format!("\x1b[<{};{};{}{}", code, c + 1, r + 1, if release { 'm' } else { 'M' }).into_bytes(),
                _ => {
                    let code = if release { 3 } else { code };
                    let mut v = b"\x1b[M".to_vec();
                    for n in [code + 32, c as u32 + 33, r as u32 + 33] {
                        if enc == MouseProtocolEncoding::Utf8 {
                            let mut t = [0u8; 4];
                            v.extend_from_slice(char::from_u32(n).unwrap_or(' ').encode_utf8(&mut t).as_bytes());
                        } else {
                            v.push(n.min(255) as u8);
                        }
                    }
                    v
                }
            }
        };
        let mut out = vec![];
        let pressed = b & !prev;
        let released = prev & !b;
        for (bit, code) in [(1, 0), (2, 1), (4, 2)] {
            if pressed & bit != 0 {
                out.extend(ev(code, false));
            }
            if released & bit != 0 && mode != MouseProtocolMode::Press {
                out.extend(ev(code, true));
            }
        }
        for (bit, code) in [(8, 64), (16, 65)] {
            if pressed & bit != 0 {
                out.extend(ev(code, false));
            }
        }
        if pressed == 0 && released == 0 {
            // motion: with a button held (ButtonMotion) or always (AnyMotion)
            let held = [(1, 0), (2, 1), (4, 2)].iter().find(|(bit, _)| b & bit != 0).map(|x| x.1);
            match (mode, held) {
                (MouseProtocolMode::ButtonMotion | MouseProtocolMode::AnyMotion, Some(code)) => out.extend(ev(code + 32, false)),
                (MouseProtocolMode::AnyMotion, None) => out.extend(ev(35, false)),
                _ => {}
            }
        }
        self.send(&out);
        true
    }

    fn mouse(&mut self, x: i32, y: i32, b: u32) {
        let c = self.draw.content();
        let (row, col) = self.view.cell(x - c.x0, y - c.y0, &self.fonts);
        let prev = self.buttons;
        self.buttons = b;
        if self.report(b, prev, row, col) {
            return;
        }
        let pressed = b & !prev;
        if pressed & 8 != 0 || pressed & 16 != 0 {
            let n = if pressed & 8 != 0 { -3 } else { 3 };
            if self.vt.screen().alternate_screen() {
                // a full-screen program without the mouse: the cursor keys, as other terminals
                let k = if n < 0 { p9win::keys::UP } else { p9win::keys::DOWN };
                for _ in 0..3 {
                    self.key(char::from_u32(k).unwrap());
                }
            } else {
                self.scroll(n);
            }
            return;
        }
        // button 2's menu, as rio's: held, the item under the pointer when it is let go
        if let Some((r, _)) = self.menu {
            let hi = self.view.menuitem(r, MENU2.len(), x - c.x0, y - c.y0, &self.fonts);
            if b & 2 != 0 {
                if self.menu.unwrap().1 != hi {
                    self.menu = Some((r, hi));
                    self.view.menu(r, MENU2, hi, &mut self.fonts);
                }
                return;
            }
            self.menu = None;
            self.view.forget(r, &self.fonts);
            match hi.map(|i| MENU2[i]) {
                Some("snarf") => self.snarf(),
                Some("paste") => self.paste(),
                Some("send") => self.sendsel(),
                _ => {}
            }
            return;
        }
        if pressed & 2 != 0 && !self.selecting {
            let r = self.view.menurect(MENU2, x - c.x0, y - c.y0, &self.fonts);
            let hi = self.view.menuitem(r, MENU2.len(), x - c.x0, y - c.y0, &self.fonts);
            self.menu = Some((r, hi));
            self.view.menu(r, MENU2, hi, &mut self.fonts);
            return;
        }
        // button 1 selects (the snarf buffer only through the menu)
        if pressed & 1 != 0 {
            self.sel = Some(Sel { a: (row, col), b: (row, col) });
            self.selecting = true;
        } else if b & 1 != 0 && self.selecting {
            if let Some(s) = &mut self.sel {
                s.b = (row, col);
            }
        } else if self.selecting && b & 1 == 0 {
            self.selecting = false;
            if self.sel.is_some_and(|s| s.a == s.b) {
                self.sel = None;
            }
        }
    }

    /// the selection into the snarf buffer
    fn snarf(&mut self) {
        if let Some(s) = self.sel {
            let _ = p9win::set_snarf(&s.text(self.vt.screen()));
        }
    }

    /// rio's send: the selection (else the snarf buffer) typed, with a newline at the end
    fn sendsel(&mut self) {
        self.snarf();
        let Ok(mut t) = p9win::snarf() else { return };
        if !t.ends_with('\n') {
            t.push('\n');
        }
        self.typed(&t);
        self.sel = None;
    }

    /// the snarf buffer typed
    fn paste(&mut self) {
        let Ok(t) = p9win::snarf() else { return };
        self.typed(&t);
    }

    /// text as typed: newlines as Enter's carriage return; bracketed when the program asked
    fn typed(&mut self, t: &str) {
        let t = t.replace("\r\n", "\r").replace('\n', "\r");
        if self.vt.screen().scrollback() != 0 {
            self.vt.screen_mut().set_scrollback(0);
        }
        if self.vt.screen().bracketed_paste() {
            self.send(b"\x1b[200~");
            self.send(t.as_bytes());
            self.send(b"\x1b[201~");
        } else {
            self.send(t.as_bytes());
        }
    }
}

fn main() {
    let mut args: Vec<String> = std::env::args().skip(1).collect();
    let (mut reverse, mut fontpath, mut size) = (false, "/lib/font/ttf/Go-Mono.ttf".to_string(), 16.0f32);
    while let Some(a) = args.first().cloned() {
        if !a.starts_with('-') || a == "-" {
            break;
        }
        args.remove(0);
        match a.as_str() {
            "--" => break,
            "-r" => reverse = true,
            "-f" if !args.is_empty() => fontpath = args.remove(0),
            "-s" if !args.is_empty() => size = args.remove(0).parse().unwrap_or_else(|_| usage()),
            _ => usage(),
        }
    }
    let die = |what: &str, e: std::io::Error| -> ! {
        eprintln!("term: {what}: {e}");
        std::process::exit(1);
    };
    if let Err(e) = pty::ownns() {
        die("rfork", e);
    }
    let fonts = font::Fonts::open(&fontpath, size).unwrap_or_else(|e| die("font", e));
    let draw = Draw::open().unwrap_or_else(|e| die("draw", e));
    let c = draw.content();
    let view = View::new(c.dx().max(1) as usize, c.dy().max(1) as usize, &fonts, reverse);
    let pty = pty::Pty::open().unwrap_or_else(|e| die("pty", e));
    let vt = vt100::Parser::new_with_callbacks(view.rows, view.cols, SCROLLBACK, Replies::default());
    let debug = std::env::var("TERMDEBUG").is_ok_and(|v| v.contains("screen")).then(|| format!("/tmp/term.{}.screen", std::process::id()));
    let mut t = Term { draw, fonts, view, vt, pty, sel: None, selecting: false, buttons: 0, menu: None, shift: false, current: true, debug };
    t.size();
    let _ = p9win::set_label("term");

    let (tx, rx) = mpsc::channel();
    let itx = tx.clone();
    let _inputs = Inputs::start(move |i| {
        let _ = itx.send(Ev::In(i));
    })
    .unwrap_or_else(|e| die("input", e));
    let mut child = t.pty.spawn(&args).unwrap_or_else(|e| die("rc", e));
    pty::reader(t.pty.master.clone(), move |o| {
        let _ = tx.send(match o {
            Some(b) => Ev::Out(b),
            None => Ev::End,
        });
    });
    t.picture();

    let mut end = false;
    while !end {
        let Ok(first) = rx.recv() else { break };
        // what came meanwhile too, then one picture
        let mut evs = vec![first];
        while let Ok(e) = rx.try_recv() {
            evs.push(e);
            if evs.len() > 256 {
                break;
            }
        }
        for ev in evs {
            match ev {
                Ev::Out(b) => {
                    t.vt.process(&b);
                    let r = std::mem::take(&mut t.vt.callbacks_mut().out);
                    t.send(&r);
                    if let Some(title) = t.vt.callbacks_mut().title.take() {
                        let _ = p9win::set_label(&title);
                    }
                    // what was selected has moved
                    if t.sel.is_some() && !t.selecting {
                        t.sel = None;
                    }
                }
                Ev::End => end = true,
                Ev::In(Input::Closed) => end = true,
                Ev::In(Input::Mouse { x, y, buttons, resized }) => {
                    if resized {
                        if let Err(e) = t.draw.attach() {
                            die("attach", e);
                        }
                        t.size();
                    }
                    t.mouse(x, y, buttons);
                }
                Ev::In(Input::Keys(k)) => t.shift = k.contains(&char::from_u32(p9win::keys::SHIFT).unwrap()),
                Ev::In(Input::Char(c)) => t.key(c),
                Ev::In(Input::Wctl(s)) => {
                    let cur = s.split_whitespace().any(|w| w == "current");
                    if cur != t.current {
                        t.current = cur;
                    }
                }
            }
        }
        t.picture();
    }
    // the master goes with term: its program gets hangup
    let _ = child.try_wait();
    std::process::exit(0);
}
