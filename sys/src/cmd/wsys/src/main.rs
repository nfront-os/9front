// SPDX-License-Identifier: MIT
//
// wsys: the window system of ../9front-ui (PLAN.md stages 3 and 4), a prototype
// with the processor backend. A 9P file server (wsys(4)), rio(4)'s files and
// attach names included, so that draw(3) programs and window(1) run in it:
//
//	attach "" or none	the tree below
//	attach new [-r x0 y0 x1 y1] [-dx n] [-dy n] [-pid n] [-cd dir]
//				a new window; the tree is its directory
//	attach n		window n's directory
//
//	/mnt/wsys/screen	read: x0 y0 x1 y1, where wsys draws (all of the
//				screen, or inside its rio window)
//	/mnt/wsys/wctl		write: new [options] [cmd] (rio(4))
//	/mnt/wsys/clone		open: a new window; the fid is then its ctl
//	/mnt/wsys/N/		a window: ctl, event (wsys' own), and rio(4)'s
//				cons consctl cursor kbd label mouse snarf text
//				wctl wdir winid winname
//	N/ctl			read: id x0 y0 x1 y1 w h base size focus
//				write: rect x0 y0 x1 y1 | buffer n seg w h stride |
//				unbuffer n | present n [x0 y0 x1 y1] | top |
//				label text | delete
//	N/event			read (blocks): lines m x y buttons msec | k hex-rune |
//				r w h | f 0|1 | b n
//
// A window is a window (layer) of draw(3) on wsys's screen, border and all,
// named as winname gives: a draw(3) program draws into it itself, as in rio.
// A client of wsys's own protocol keeps its pixels in global segments of
// segment(3) it makes, in the range of addresses ctl gives each window (wsys
// attaches them at the same address, so they cannot collide in it); buffer n
// announces one (x8r8g8b8, w x h, stride bytes a line); present n copies a
// rectangle of it into the window and answers with the event b n: buffer n
// is free again (a swap chain of the client's).
//
// Input: /dev/mouse and /dev/kbd (else /dev/cons, raw) to the focused window:
// its mouse file gets the mouse while it is over the window or a button is
// held (r first after a change of the window), its kbd file the keyboard's
// messages when open, else cons the runes (a line at a time unless raw; Del
// interrupts the window's note group). A button pressed on another window
// gives it the focus and raises it; on a border, button 1 moves the window,
// button 3 sweeps it to a new size (an outline until the button comes up).
// A window lives while its files are open (or the clone fid that made it).
// Processes wsys starts (-i, wctl's new) get note groups of their own.
//
// usage: wsys [-s srvname] [-i cmd] [-x]
//	-s	post as /srv/srvname (default wsys.$user.$pid)
//	-i	run cmd (rc) in a name space with /mnt/wsys mounted, $wsys set
//	-x	exit when cmd exits

mod draw;
mod ninep;
mod p9;

use draw::{Display, Rect, XRGB32};
use ninep::{Dir, Qid, R, T};
use p9::{Fd, OREAD, ORDWR, OWRITE, Seg};
use std::collections::{HashMap, VecDeque};
use std::io::Write;
use std::sync::mpsc;
use std::thread;

const BORDER: i32 = 4;
const BG: u32 = 0x777777;
const FOCUSED: u32 = 0x335588;
const UNFOCUSED: u32 = 0xAAAAAA;
// the windows' ranges for their clients' segments: slot i is SEGBASE + i*SEGSIZE
// (slots are given again when free: 0x2000 of them, 32 TB)
const SEGBASE: usize = 0x5000_0000_0000;
const SEGSIZE: usize = 0x1_0000_0000;
const NSLOT: usize = 0x2000;
const MAXBUF: u32 = 8;
const WHITE: u32 = 0xFFFFFF;
const MINSIZE: i32 = 16; // a window swept smaller stays this big
const CORNER: i32 = 20; // on the border this near a corner grabs both its edges
const MAXEVENTS: usize = 512;
const MAXMOUSE: usize = 32;
const MAXTEXT: usize = 1 << 20;

enum Ev {
    Msg(Vec<u8>),
    Hangup,
    Mouse { x: i32, y: i32, b: i32, ms: u32 },
    Kbd(Vec<Vec<u8>>), // kbd(3) messages: k, K or c and runes
    ChildExit(String),
    WinExit(u32), // the process wsys started in a window
}

/// a window's files
#[derive(Clone, Copy, PartialEq, Debug)]
enum F {
    Ctl,
    Event,
    Cons,
    Consctl,
    Cursor,
    Kbd,
    Label,
    Mouse,
    Snarf,
    Text,
    Wctl,
    Wdir,
    Winid,
    Winname,
}

const WINFILES: [(F, &str, u32); 14] = [
    (F::Ctl, "ctl", 0o666),
    (F::Event, "event", 0o444),
    (F::Cons, "cons", 0o666),
    (F::Consctl, "consctl", 0o222),
    (F::Cursor, "cursor", 0o666),
    (F::Kbd, "kbd", 0o666),
    (F::Label, "label", 0o666),
    (F::Mouse, "mouse", 0o666),
    (F::Snarf, "snarf", 0o666),
    (F::Text, "text", 0o666),
    (F::Wctl, "wctl", 0o666),
    (F::Wdir, "wdir", 0o666),
    (F::Winid, "winid", 0o444),
    (F::Winname, "winname", 0o444),
];

#[derive(Clone, Copy, PartialEq, Debug)]
enum Kind {
    Root,
    Screen,
    Clone,
    RootWctl,
    WinDir(u32),
    File(u32, F),
}

struct FidState {
    kind: Kind,
    open: bool,
    dirpos: usize,
    win: Option<u32>, // the window it holds (attached to it, or made through clone)
    owns: bool,       // the clone fid that made the window: clunked, the window goes
    raw: bool,        // a consctl that turned raw mode on
    seen: u64,        // wctl: the window's state last read (0: none yet)
}

/// A client's buffer. Only the main loop touches it: Rust's std on Plan 9 runs
/// threads as procs, and a segment attached after they started is the attaching
/// proc's alone. Each is a segment of wsys's: with the 64 stack segments std
/// makes, some 190 fit (NSEG).
struct Buf {
    seg: Seg,
    w: i32,
    h: i32,
    stride: usize,
}

struct Win {
    r: Rect, // content, in screen coordinates
    slot: usize,
    bufs: HashMap<u32, Buf>,
    img: u32,   // what a client of buffers presented, x8r8g8b8, the window's size
    layer: u32, // its window on wsys's screen in draw(3), border and all
    name: String, // the layer's public name (winname)
    refs: u32,    // fids holding the window
    pinned: bool, // made by wctl's new: kept until its process attached or ended
    pid: Option<u32>, // the note group for interrupt and hangup
    label: String,
    wdir: String,
    text: Vec<u8>, // what was written to cons and text
    events: VecDeque<String>,
    // the files of rio(4)
    raw: bool,
    line: Vec<u8>,       // cons, cooked: the line being typed
    cons: VecDeque<u8>,  // cons: what reads get
    eof: bool,           // cons: ^D on an empty line, a read of 0
    kbd: VecDeque<Vec<u8>>, // kbd: messages, each with its null byte
    kbdopen: u32,
    mouseopen: u32,
    mouse: VecDeque<(i32, i32, i32, u32)>,
    resized: bool, // the next mouse read is an r message
    cursor: Vec<u8>, // cursor(3)'s 72 bytes, or none: the default arrow
    state: u64,      // bumped when wctl's answer changes
}

#[derive(Clone, Copy)]
enum DragKind {
    Move,
    Sweep { left: bool, right: bool, top: bool, bottom: bool },
}

/// a window under the mouse: moved, or swept to a new size
#[derive(Clone, Copy)]
struct Drag {
    win: u32,
    kind: DragKind,
    x0: i32, // where the button went down
    y0: i32,
    orig: Rect,
    cur: Rect,
}

/// a read that waits: answered when the window has something for it
struct Pending {
    tag: u16,
    fid: u32,
    win: u32,
    f: F,
    count: u32,
}

/// the options of wctl's new, resize, move and set, and of attach's new
#[derive(Default)]
struct Opts {
    r: Option<Rect>,
    dx: Option<i32>,
    dy: Option<i32>,
    minx: Option<(bool, i32)>, // (relative, value)
    miny: Option<(bool, i32)>,
    maxx: Option<(bool, i32)>,
    maxy: Option<(bool, i32)>,
    pid: Option<u32>,
    cd: Option<String>,
    cmd: Vec<String>,
}

fn opts(words: &[&str], cmd: bool) -> Result<Opts, String> {
    let mut o = Opts::default();
    let mut i = 0;
    let num = |i: usize| -> Result<i32, String> {
        words.get(i).and_then(|w| w.parse::<i32>().ok()).ok_or(format!("bad number after {}", words[i - 1]))
    };
    let edge = |i: usize| -> Result<(bool, i32), String> {
        let w = words.get(i).ok_or("missing number")?;
        let rel = w.starts_with('+') || w.starts_with('-');
        let v = w.trim_start_matches('+').parse::<i32>().map_err(|_| format!("bad number {w}"))?;
        Ok((rel, v))
    };
    while i < words.len() {
        let w = words[i];
        if !w.starts_with('-') || w.len() < 2 || w.as_bytes()[1].is_ascii_digit() {
            break;
        }
        match w {
            "-r" => {
                o.r = Some(Rect::new(num(i + 1)?, num(i + 2)?, num(i + 3)?, num(i + 4)?));
                i += 5;
            }
            "-dx" => {
                o.dx = Some(num(i + 1)?);
                i += 2;
            }
            "-dy" => {
                o.dy = Some(num(i + 1)?);
                i += 2;
            }
            "-minx" | "-miny" | "-maxx" | "-maxy" => {
                let v = Some(edge(i + 1)?);
                match w {
                    "-minx" => o.minx = v,
                    "-miny" => o.miny = v,
                    "-maxx" => o.maxx = v,
                    _ => o.maxy = v,
                }
                i += 2;
            }
            "-pid" => {
                o.pid = Some(num(i + 1)? as u32);
                i += 2;
            }
            "-cd" => {
                o.cd = Some(words.get(i + 1).ok_or("missing directory")?.to_string());
                i += 2;
            }
            "-hide" | "-scroll" | "-noscroll" => i += 1,
            _ => return Err(format!("unknown option {w}")),
        }
    }
    if cmd {
        o.cmd = words[i..].iter().map(|s| s.to_string()).collect();
    } else if i < words.len() {
        return Err(format!("unexpected {}", words[i]));
    }
    Ok(o)
}

/// rect r (a window's frame) after the options: move keeps the size
fn place(r: Rect, o: &Opts, mv: bool) -> Rect {
    let mut r = o.r.unwrap_or(r);
    let set = |cur: i32, e: Option<(bool, i32)>| match e {
        Some((true, v)) => cur + v,
        Some((false, v)) => v,
        None => cur,
    };
    if mv {
        let (x, y) = (set(r.x0, o.minx), set(r.y0, o.miny));
        return r.add(x - r.x0, y - r.y0);
    }
    r = Rect::new(set(r.x0, o.minx), set(r.y0, o.miny), set(r.x1, o.maxx), set(r.y1, o.maxy));
    if let Some(dx) = o.dx {
        r.x1 = r.x0 + dx;
    }
    if let Some(dy) = o.dy {
        r.y1 = r.y0 + dy;
    }
    r
}

/// an rc word: quoted
fn rcquote(s: &str) -> String {
    format!("'{}'", s.replace('\'', "''"))
}

struct Wsys {
    srv: Fd,
    srvpath: String,
    msize: u32,
    uid: String,
    fids: HashMap<u32, FidState>,
    wins: HashMap<u32, Win>,
    z: Vec<u32>, // bottom to top
    focus: Option<u32>,
    nextwin: u32,
    pending: Vec<Pending>,
    d: Display,
    bg: u32,
    cfocus: u32,
    cother: u32,
    buttons: i32,
    mx: i32, // the mouse, last seen
    my: i32,
    ms: u32,
    white: u32,
    slots: Vec<bool>, // in use
    drag: Option<Drag>,
    scr: u32,          // the screen in draw(3) the windows are on
    sweep: Vec<u32>,   // the outline of a sweep: four thin windows on top
    snarf: Vec<u8>,
    mousefd: Option<Fd>,  // to move the pointer
    cursorfd: Option<Fd>, // to set the cursor of the focused window
    tx: mpsc::Sender<Ev>,
}

fn qid(k: Kind) -> Qid {
    let (ty, path) = match k {
        Kind::Root => (ninep::QTDIR, 0),
        Kind::Clone => (0, 1),
        Kind::Screen => (0, 5),
        Kind::RootWctl => (0, 6),
        Kind::WinDir(n) => (ninep::QTDIR, (n as u64) << 8 | 0x10),
        Kind::File(n, f) => (0, (n as u64) << 8 | (0x20 + f as u64)),
    };
    Qid { ty, vers: 0, path }
}

fn mousemsg(c: char, x: i32, y: i32, b: i32, ms: u32) -> Vec<u8> {
    format!("{c}{x:11} {y:11} {b:11} {ms:11} ").into_bytes()
}

impl Wsys {
    fn send(&self, m: Vec<u8>) {
        if let Err(e) = self.srv.write1(&m) {
            eprintln!("wsys: srv write: {e}");
        }
    }

    fn dir(&self, k: Kind) -> Dir {
        let (name, mode) = match k {
            Kind::Root => ("/".to_string(), ninep::DMDIR | 0o555),
            Kind::Clone => ("clone".to_string(), 0o666),
            Kind::Screen => ("screen".to_string(), 0o444),
            Kind::RootWctl => ("wctl".to_string(), 0o222),
            Kind::WinDir(n) => (n.to_string(), ninep::DMDIR | 0o555),
            Kind::File(_, f) => {
                let (_, name, mode) = WINFILES.iter().find(|w| w.0 == f).unwrap();
                (name.to_string(), *mode)
            }
        };
        Dir { qid: qid(k), mode, length: 0, name, uid: self.uid.clone(), mtime: 0 }
    }

    fn exists(&self, k: Kind) -> bool {
        match k {
            Kind::WinDir(n) | Kind::File(n, _) => self.wins.contains_key(&n),
            _ => true,
        }
    }

    fn walk1(&self, k: Kind, name: &str) -> Option<Kind> {
        match k {
            Kind::Root => match name {
                "clone" => Some(Kind::Clone),
                "screen" => Some(Kind::Screen),
                "wctl" => Some(Kind::RootWctl),
                ".." => Some(Kind::Root),
                _ => {
                    let n: u32 = name.parse().ok()?;
                    self.wins.contains_key(&n).then_some(Kind::WinDir(n))
                }
            },
            Kind::WinDir(n) => {
                if name == ".." {
                    return Some(Kind::Root);
                }
                WINFILES.iter().find(|w| w.1 == name).map(|w| Kind::File(n, w.0))
            }
            _ => None,
        }
    }

    fn children(&self, k: Kind) -> Vec<Kind> {
        match k {
            Kind::Root => {
                let mut v = vec![Kind::Screen, Kind::Clone, Kind::RootWctl];
                let mut ids: Vec<u32> = self.wins.keys().copied().collect();
                ids.sort();
                v.extend(ids.into_iter().map(Kind::WinDir));
                v
            }
            Kind::WinDir(n) => WINFILES.iter().map(|w| Kind::File(n, w.0)).collect(),
            _ => vec![],
        }
    }

    fn handle(&mut self, m: &[u8]) {
        let Some((tag, t)) = ninep::parse(m) else {
            eprintln!("wsys: bad 9P message of {} bytes", m.len());
            return;
        };
        let r = self.answer(tag, t);
        if let Some(r) = r {
            self.send(r);
        }
    }

    fn newfid(&mut self, fid: u32, kind: Kind, win: Option<u32>) {
        if let Some(n) = win {
            if let Some(w) = self.wins.get_mut(&n) {
                w.refs += 1;
                w.pinned = false;
            }
        }
        self.fids.insert(fid, FidState { kind, open: false, dirpos: 0, win, owns: false, raw: false, seen: 0 });
    }

    /// the reply, or None when it comes later (a read that waits)
    fn answer(&mut self, tag: u16, t: T) -> Option<Vec<u8>> {
        let e = |s: &str| Some(ninep::error(tag, s));
        match t {
            T::Version { msize, version } => {
                self.msize = msize.min(65536);
                let fids: Vec<u32> = self.fids.keys().copied().collect();
                for f in fids {
                    self.clunk(f);
                }
                let v = if version.starts_with("9P2000") { "9P2000" } else { "unknown" };
                Some(R::new(101, tag).u32(self.msize).str(v).done())
            }
            T::Auth => e("wsys: no authentication required"),
            T::Attach { fid, _uname: _, aname } => {
                if self.fids.contains_key(&fid) {
                    return e("fid in use");
                }
                let words: Vec<&str> = aname.split_whitespace().collect();
                let (kind, win) = match words.first().copied() {
                    None | Some("none") => (Kind::Root, None),
                    Some("new") => match opts(&words[1..], false).and_then(|o| self.newwin(&o)) {
                        Ok(n) => (Kind::WinDir(n), Some(n)),
                        Err(err) => return e(&err),
                    },
                    Some(w) => match w.parse::<u32>() {
                        Ok(n) if self.wins.contains_key(&n) => (Kind::WinDir(n), Some(n)),
                        _ => return e("no such window"),
                    },
                };
                self.newfid(fid, kind, win);
                Some(R::new(105, tag).qid(qid(kind)).done())
            }
            T::Flush { oldtag } => {
                self.pending.retain(|p| p.tag != oldtag);
                Some(R::new(109, tag).done())
            }
            T::Walk { fid, newfid, names } => {
                let Some(f) = self.fids.get(&fid) else { return e("unknown fid") };
                if f.open {
                    return e("walk of an open fid");
                }
                if newfid != fid && self.fids.contains_key(&newfid) {
                    return e("fid in use");
                }
                let (mut k, win) = (f.kind, f.win);
                let mut qids = vec![];
                for n in &names {
                    match self.walk1(k, n) {
                        Some(k2) => {
                            k = k2;
                            qids.push(qid(k));
                        }
                        None => break,
                    }
                }
                if !names.is_empty() && qids.is_empty() {
                    return e("file does not exist");
                }
                if qids.len() == names.len() {
                    if newfid == fid {
                        self.fids.get_mut(&fid).unwrap().kind = k;
                    } else {
                        self.newfid(newfid, k, win);
                    }
                }
                let mut r = R::new(111, tag).u16(qids.len() as u16);
                for q in qids {
                    r = r.qid(q);
                }
                Some(r.done())
            }
            T::Open { fid, mode } => {
                let Some(f) = self.fids.get(&fid) else { return e("unknown fid") };
                if f.open {
                    return e("already open");
                }
                let k = f.kind;
                if !self.exists(k) {
                    return e("window deleted");
                }
                let rw = mode & 3;
                let reading = rw == OREAD as u8 || rw == 2;
                let writing = rw == OWRITE as u8 || rw == 2;
                let k = match k {
                    Kind::Root | Kind::WinDir(_) if rw != OREAD as u8 => return e("is a directory"),
                    Kind::Screen if writing => return e("permission denied"),
                    Kind::RootWctl if reading => return e("permission denied"),
                    Kind::File(_, F::Event | F::Winid | F::Winname) if writing => return e("permission denied"),
                    Kind::File(_, F::Consctl) if reading => return e("permission denied"),
                    Kind::Clone => match self.newwin(&Opts::default()) {
                        Ok(n) => {
                            let f = self.fids.get_mut(&fid).unwrap();
                            (f.owns, f.win) = (true, Some(n));
                            self.wins.get_mut(&n).unwrap().refs += 1;
                            Kind::File(n, F::Ctl)
                        }
                        Err(err) => return e(&format!("new window: {err}")),
                    },
                    k => k,
                };
                match k {
                    Kind::File(n, F::Kbd) => self.wins.get_mut(&n).unwrap().kbdopen += 1,
                    Kind::File(n, F::Mouse) => self.wins.get_mut(&n).unwrap().mouseopen += 1,
                    _ => {}
                }
                let f = self.fids.get_mut(&fid).unwrap();
                f.kind = k;
                f.open = true;
                Some(R::new(113, tag).qid(qid(k)).u32(self.msize - ninep::IOHDRSZ).done())
            }
            T::Create => e("cannot create in wsys"),
            T::Remove { fid } => {
                self.clunk(fid);
                e("cannot remove in wsys")
            }
            T::Wstat => e("wstat not allowed"),
            T::Stat { fid } => {
                let Some(f) = self.fids.get(&fid) else { return e("unknown fid") };
                let s = ninep::stat(&self.dir(f.kind));
                Some(R::new(125, tag).u16(s.len() as u16).bytes(&s).done())
            }
            T::Clunk { fid } => {
                self.clunk(fid);
                Some(R::new(121, tag).done())
            }
            T::Read { fid, offset, count } => {
                let count = count.min(self.msize - ninep::IOHDRSZ);
                let Some(f) = self.fids.get(&fid) else { return e("unknown fid") };
                if !f.open {
                    return e("not open");
                }
                let k = f.kind;
                if !self.exists(k) {
                    return e("window deleted");
                }
                let text = |s: &[u8]| {
                    let o = (offset as usize).min(s.len());
                    let b = &s[o..(o + count as usize).min(s.len())];
                    Some(R::new(117, tag).u32(b.len() as u32).bytes(b).done())
                };
                match k {
                    Kind::Root | Kind::WinDir(_) => {
                        let pos = if offset == 0 { 0 } else { f.dirpos };
                        let mut out = vec![];
                        let mut n = pos;
                        for c in self.children(k).into_iter().skip(pos) {
                            let s = ninep::stat(&self.dir(c));
                            if out.len() + s.len() > count as usize {
                                break;
                            }
                            out.extend(s);
                            n += 1;
                        }
                        self.fids.get_mut(&fid).unwrap().dirpos = n;
                        Some(R::new(117, tag).u32(out.len() as u32).bytes(&out).done())
                    }
                    Kind::Screen => {
                        let r = self.d.r;
                        text(format!("{} {} {} {}\n", r.x0, r.y0, r.x1, r.y1).as_bytes())
                    }
                    Kind::File(n, F::Ctl) => text(self.ctltext(n).as_bytes()),
                    Kind::File(n, F::Label) => text(self.wins[&n].label.as_bytes()),
                    Kind::File(n, F::Wdir) => text(self.wins[&n].wdir.as_bytes()),
                    Kind::File(n, F::Text) => text(&self.wins[&n].text),
                    Kind::File(n, F::Cursor) => text(&self.wins[&n].cursor),
                    Kind::File(n, F::Winname) => text(self.wins[&n].name.as_bytes()),
                    Kind::File(n, F::Winid) => text(format!("{n:11} ").as_bytes()),
                    Kind::File(_, F::Snarf) => text(&self.snarf),
                    Kind::File(n, f @ (F::Event | F::Mouse | F::Cons | F::Kbd | F::Wctl)) => {
                        match self.take(fid, n, f, count) {
                            Some(b) => Some(R::new(117, tag).u32(b.len() as u32).bytes(&b).done()),
                            None => {
                                self.pending.push(Pending { tag, fid, win: n, f, count });
                                None
                            }
                        }
                    }
                    _ => e("permission denied"),
                }
            }
            T::Write { fid, offset, data } => {
                let Some(f) = self.fids.get(&fid) else { return e("unknown fid") };
                if !f.open {
                    return e("not open");
                }
                let k = f.kind;
                if !self.exists(k) {
                    return e("window deleted");
                }
                let ok = Some(R::new(119, tag).u32(data.len() as u32).done());
                let s = String::from_utf8_lossy(&data).into_owned();
                match k {
                    Kind::File(n, F::Ctl) => {
                        for line in s.lines() {
                            if let Err(err) = self.ctl(n, line.trim()) {
                                return e(&err);
                            }
                        }
                        ok
                    }
                    Kind::RootWctl | Kind::File(_, F::Wctl) => {
                        let n = if let Kind::File(n, _) = k { Some(n) } else { None };
                        for line in s.lines() {
                            if let Err(err) = self.wctl(n, line.trim()) {
                                return e(&err);
                            }
                        }
                        ok
                    }
                    Kind::File(n, F::Cons | F::Text) => {
                        let t = &mut self.wins.get_mut(&n).unwrap().text;
                        t.extend_from_slice(&data);
                        if t.len() > MAXTEXT {
                            t.drain(..t.len() - MAXTEXT);
                        }
                        ok
                    }
                    Kind::File(n, F::Consctl) => {
                        for w in s.split_whitespace() {
                            match w {
                                "rawon" | "rawoff" => {
                                    let on = w == "rawon";
                                    self.fids.get_mut(&fid).unwrap().raw = on;
                                    self.setraw(n, on);
                                }
                                "holdon" | "holdoff" => {}
                                _ => return e("unknown control message"),
                            }
                        }
                        ok
                    }
                    Kind::File(_, F::Mouse) => {
                        // m x y: the pointer goes there
                        let v: Vec<i32> = s[1..].split_whitespace().filter_map(|x| x.parse().ok()).collect();
                        if s.starts_with('m') && v.len() >= 2 {
                            if let Some(fd) = &self.mousefd {
                                let _ = fd.write(format!("m{} {}", v[0], v[1]).as_bytes());
                            }
                        }
                        ok
                    }
                    Kind::File(n, F::Cursor) => {
                        let c = if data.len() >= 72 { data[..72].to_vec() } else { vec![] };
                        self.wins.get_mut(&n).unwrap().cursor = c;
                        if self.focus == Some(n) {
                            self.setcursor();
                        }
                        ok
                    }
                    Kind::File(n, F::Label | F::Wdir) => {
                        let w = self.wins.get_mut(&n).unwrap();
                        let v = if matches!(k, Kind::File(_, F::Label)) { &mut w.label } else { &mut w.wdir };
                        if offset == 0 {
                            v.clear();
                        }
                        v.push_str(&s);
                        ok
                    }
                    Kind::File(_, F::Snarf) => {
                        if offset == 0 {
                            self.snarf.clear();
                        }
                        self.snarf.extend_from_slice(&data);
                        ok
                    }
                    _ => e("permission denied"),
                }
            }
        }
    }

    fn clunk(&mut self, fid: u32) {
        let Some(f) = self.fids.remove(&fid) else { return };
        self.pending.retain(|p| p.fid != fid);
        let Some(n) = f.win.or(match f.kind {
            Kind::File(n, _) | Kind::WinDir(n) => Some(n),
            _ => None,
        }) else {
            return;
        };
        if let Some(w) = self.wins.get_mut(&n) {
            if f.open {
                match f.kind {
                    Kind::File(_, F::Kbd) => w.kbdopen = w.kbdopen.saturating_sub(1),
                    Kind::File(_, F::Mouse) => w.mouseopen = w.mouseopen.saturating_sub(1),
                    _ => {}
                }
                if f.raw {
                    self.setraw(n, false);
                }
            }
        }
        if f.win == Some(n) {
            if f.owns {
                self.delwin(n, false);
                return;
            }
            if let Some(w) = self.wins.get_mut(&n) {
                w.refs = w.refs.saturating_sub(1);
                if w.refs == 0 && !w.pinned {
                    self.delwin(n, false);
                }
            }
        }
    }

    fn setraw(&mut self, n: u32, on: bool) {
        let w = self.wins.get_mut(&n).unwrap();
        w.raw = on;
        if on && !w.line.is_empty() {
            // what was typed so far goes as it is
            let l = std::mem::take(&mut w.line);
            w.cons.extend(l);
            self.wake(n);
        }
    }

    fn ctltext(&self, n: u32) -> String {
        let w = &self.wins[&n];
        format!(
            "{} {} {} {} {} {} {} {:#x} {:#x} {}\n",
            n, w.r.x0, w.r.y0, w.r.x1, w.r.y1, w.r.dx(), w.r.dy(),
            SEGBASE + w.slot * SEGSIZE, SEGSIZE, (self.focus == Some(n)) as i32
        )
    }

    /// what a read of window n's file f gets now, or None: it waits
    fn take(&mut self, fid: u32, n: u32, f: F, count: u32) -> Option<Vec<u8>> {
        let (mx, my, ms, focus) = (self.mx, self.my, self.ms, self.focus);
        let seen = self.fids.get(&fid).map(|f| f.seen).unwrap_or(0);
        let w = self.wins.get_mut(&n)?;
        match f {
            F::Event => {
                if w.events.is_empty() {
                    return None;
                }
                let mut out = String::new();
                while let Some(l) = w.events.front() {
                    if !out.is_empty() && out.len() + l.len() > count as usize {
                        break;
                    }
                    out.push_str(l);
                    w.events.pop_front();
                }
                out.truncate(count as usize);
                Some(out.into_bytes())
            }
            F::Mouse => {
                if w.resized {
                    w.resized = false;
                    return Some(mousemsg('r', mx, my, 0, ms));
                }
                let (x, y, b, t) = w.mouse.pop_front()?;
                Some(mousemsg('m', x, y, b, t))
            }
            F::Cons => {
                if w.cons.is_empty() {
                    if w.eof {
                        w.eof = false;
                        return Some(vec![]);
                    }
                    return None;
                }
                // whole runes
                let mut k = (count as usize).min(w.cons.len());
                while k < w.cons.len() && k > 0 && (w.cons[k] & 0xC0) == 0x80 {
                    k -= 1;
                }
                Some(w.cons.drain(..k).collect())
            }
            F::Kbd => {
                let mut out = vec![];
                while let Some(m) = w.kbd.front() {
                    if !out.is_empty() && out.len() + m.len() > count as usize {
                        break;
                    }
                    out.extend_from_slice(m);
                    w.kbd.pop_front();
                }
                if out.is_empty() { None } else { Some(out) }
            }
            F::Wctl => {
                // the first read answers at once, a later one when something changed
                if seen != 0 && seen >= w.state {
                    return None;
                }
                let state = w.state.max(1);
                w.state = state;
                let f = w.r.inset(-BORDER);
                let cur = if focus == Some(n) { "current" } else { "notcurrent" };
                let s = format!("{:11} {:11} {:11} {:11} {:>11} {:>11} ", f.x0, f.y0, f.x1, f.y1, cur, "visible");
                if let Some(fs) = self.fids.get_mut(&fid) {
                    fs.seen = state;
                }
                Some(s.into_bytes())
            }
            _ => None,
        }
    }

    /// answer the reads that wait on window n, as far as it has something
    fn wake(&mut self, n: u32) {
        let mut i = 0;
        while i < self.pending.len() {
            let p = &self.pending[i];
            if p.win != n {
                i += 1;
                continue;
            }
            let (tag, fid, f, count) = (p.tag, p.fid, p.f, p.count);
            match self.take(fid, n, f, count) {
                Some(b) => {
                    self.pending.remove(i);
                    self.send(R::new(117, tag).u32(b.len() as u32).bytes(&b).done());
                }
                None => i += 1,
            }
        }
    }

    /// something wctl reports changed
    fn changed(&mut self, n: u32) {
        if let Some(w) = self.wins.get_mut(&n) {
            w.state += 1;
            self.wake(n);
        }
    }

    fn event(&mut self, n: u32, line: String) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        if w.events.len() >= MAXEVENTS {
            w.events.pop_front();
        }
        w.events.push_back(line + "\n");
        self.wake(n);
    }

    /// a white image of w x h for a window
    fn image(&mut self, w: i32, h: i32) -> Result<u32, String> {
        let img = self.d.alloc(Rect::new(0, 0, w, h), XRGB32, false, 0xFFFF_FFFF).map_err(|e| e.to_string())?;
        Ok(img)
    }

    /// a new window, where the options put its frame (else by itself)
    fn newwin(&mut self, o: &Opts) -> Result<u32, String> {
        let slot = self.slots.iter().skip(1).position(|&u| !u).map(|i| i + 1).ok_or("too many windows")?;
        let n = self.nextwin + 1;
        let s = self.d.r;
        let off = 40 + 30 * ((n as i32 - 1) % 10);
        let auto = Rect::new(s.x0 + off, s.y0 + off, s.x0 + off + 400, s.y0 + off + 300).inset(-BORDER);
        let f = place(auto, o, false);
        let r = f.inset(BORDER).clip(&s.inset(BORDER));
        if r.dx() < 1 || r.dy() < 1 {
            return Err("window outside the screen".into());
        }
        let img = self.image(r.dx(), r.dy())?;
        let layer = match self.d.allocwindow(self.scr, r.inset(-BORDER), 0xFFFF_FFFF) {
            Ok(l) => l,
            Err(e) => {
                let _ = self.d.free(img);
                return Err(e.to_string());
            }
        };
        let name = format!("wsys.{}.{}.{}", std::process::id(), n, layer);
        if let Err(e) = self.d.name(layer, &name, true) {
            let _ = self.d.free(layer);
            let _ = self.d.free(img);
            return Err(e.to_string());
        }
        self.nextwin = n;
        self.slots[slot] = true;
        self.wins.insert(n, Win {
            r, slot, bufs: HashMap::new(), img, layer, name, refs: 0, pinned: false, pid: o.pid,
            label: String::new(), wdir: o.cd.clone().unwrap_or_default(), text: vec![],
            events: VecDeque::new(), raw: false, line: vec![], cons: VecDeque::new(), eof: false,
            kbd: VecDeque::new(), kbdopen: 0, mouseopen: 0, mouse: VecDeque::new(), resized: false,
            cursor: vec![], state: 1,
        });
        self.z.push(n);
        self.setfocus(Some(n));
        self.paint(n, None);
        Ok(n)
    }

    /// window n goes; hangup: its note group gets the note (a delete by wctl)
    fn delwin(&mut self, n: u32, hangup: bool) {
        let Some(w) = self.wins.remove(&n) else { return };
        if hangup {
            if let Some(pid) = w.pid {
                postnote(pid, "hangup");
            }
        }
        let _ = self.d.name(w.layer, &w.name, false);
        let _ = self.d.free(w.layer);
        let _ = self.d.free(w.img);
        self.slots[w.slot] = false; // its buffers go with w
        self.z.retain(|&i| i != n);
        for p in self.pending.iter().filter(|p| p.win == n) {
            self.send(ninep::error(p.tag, "window deleted"));
        }
        self.pending.retain(|p| p.win != n);
        if self.drag.map(|g| g.win) == Some(n) {
            self.drag = None;
            self.outline(None);
        }
        if self.focus == Some(n) {
            self.focus = None;
            let top = self.z.last().copied();
            self.setfocus(top);
        }
        let _ = self.d.flush();
    }

    fn setfocus(&mut self, n: Option<u32>) {
        if self.focus == n {
            return;
        }
        let old = self.focus;
        self.focus = n;
        for (id, v) in [(old, 0), (n, 1)] {
            if let Some(id) = id {
                if self.wins.contains_key(&id) {
                    self.event(id, format!("f {v}"));
                    self.paintborder(id);
                    self.changed(id);
                }
            }
        }
        self.setcursor();
    }

    /// the cursor of the focused window (none: the default)
    fn setcursor(&mut self) {
        let c = self.focus.and_then(|n| self.wins.get(&n)).map(|w| w.cursor.clone()).unwrap_or_default();
        if let Some(fd) = &self.cursorfd {
            let _ = fd.write(&c);
        }
    }

    fn restack(&mut self, n: u32, top: bool) {
        self.z.retain(|&i| i != n);
        if top {
            self.z.push(n);
        } else {
            self.z.insert(0, n);
        }
        let l = self.wins[&n].layer;
        let r = if top { self.d.top(&[l]) } else { self.d.bottom(&[l]) };
        if let Err(e) = r.and_then(|_| self.d.flush()) {
            eprintln!("wsys: draw: {e}");
        }
    }

    fn raise(&mut self, n: u32) {
        if self.z.last() != Some(&n) {
            self.restack(n, true);
        }
    }

    /// wctl (rio(4)): of window n, or of wsys (only new)
    fn wctl(&mut self, n: Option<u32>, line: &str) -> Result<(), String> {
        let words: Vec<&str> = line.split_whitespace().collect();
        let Some(&cmd) = words.first() else { return Ok(()) };
        if cmd == "new" {
            let o = opts(&words[1..], true)?;
            return self.spawn(o);
        }
        let n = n.ok_or("wsys' wctl only makes new windows")?;
        match cmd {
            "resize" | "move" => {
                let o = opts(&words[1..], false)?;
                let f = place(self.wins[&n].r.inset(-BORDER), &o, cmd == "move");
                self.setrect(n, f.inset(BORDER))
            }
            "top" => {
                self.raise(n);
                Ok(())
            }
            "bottom" => {
                self.restack(n, false);
                Ok(())
            }
            "current" => {
                self.raise(n);
                self.setfocus(Some(n));
                Ok(())
            }
            "delete" => {
                self.delwin(n, true);
                Ok(())
            }
            "set" => {
                let o = opts(&words[1..], false)?;
                if let Some(pid) = o.pid {
                    self.wins.get_mut(&n).unwrap().pid = Some(pid);
                }
                Ok(())
            }
            "hide" | "unhide" | "scroll" | "noscroll" => Ok(()),
            _ => Err(format!("unknown wctl message: {cmd}")),
        }
    }

    /// wctl's new: a window, and cmd (default rc -i) run in it in a name space
    /// of its own with the window's files on /dev and /mnt/wsys
    fn spawn(&mut self, o: Opts) -> Result<(), String> {
        let n = self.newwin(&o)?;
        self.wins.get_mut(&n).unwrap().pinned = true;
        let cmd = if o.cmd.is_empty() { "rc -i".to_string() } else { o.cmd.join(" ") };
        let mut script = format!(
            "rfork ns; mount {srv} /mnt/wsys {n} || exit mount; bind -b /mnt/wsys /dev; wsys={srv}; ",
            srv = rcquote(&self.srvpath)
        );
        if let Some(d) = &o.cd {
            script += &format!("cd {} >[2]/dev/null; ", rcquote(d));
        }
        script += &format!("exec {cmd} </dev/cons >/dev/cons >[2]/dev/cons");
        let child = std::process::Command::new("/bin/rc").arg("-c").arg(&script).spawn();
        match child {
            Ok(mut c) => {
                let w = self.wins.get_mut(&n).unwrap();
                if w.pid.is_none() {
                    w.pid = Some(c.id());
                }
                let tx = self.tx.clone();
                thread::spawn(move || {
                    let _ = c.wait();
                    let _ = tx.send(Ev::WinExit(n));
                });
                Ok(())
            }
            Err(e) => {
                self.delwin(n, false);
                Err(format!("new: {e}"))
            }
        }
    }

    fn ctl(&mut self, n: u32, line: &str) -> Result<(), String> {
        let mut f = line.split_whitespace();
        let cmd = f.next().unwrap_or("");
        let nums: Vec<i32> = line.split_whitespace().skip(1).filter_map(|s| s.parse().ok()).collect();
        match cmd {
            "" => Ok(()),
            "buffer" => {
                let a: Vec<&str> = line.split_whitespace().collect();
                let num = |i: usize| a.get(i).and_then(|x| x.parse::<i64>().ok());
                let (Some(b), Some(name), Some(bw), Some(bh), Some(stride)) = (num(1), a.get(2), num(3), num(4), num(5)) else {
                    return Err("usage: buffer n seg w h stride".into());
                };
                if !(0..MAXBUF as i64).contains(&b) || bw < 1 || bh < 1 || bw > 16384 || bh > 16384 || stride < bw * 4 {
                    return Err("bad buffer".into());
                }
                let seg = Seg::attach(name).map_err(|e| format!("buffer {b}: {e}"))?;
                let w = self.wins.get_mut(&n).unwrap();
                let base = SEGBASE + w.slot * SEGSIZE;
                if seg.va < base || seg.va + seg.len > base + SEGSIZE {
                    return Err(format!("buffer {b}: segment {name} at {:#x} is outside the window's range", seg.va));
                }
                if (stride * (bh - 1) + bw * 4) as usize > seg.len {
                    return Err(format!("buffer {b}: segment {name} too small"));
                }
                w.bufs.insert(b as u32, Buf { seg, w: bw as i32, h: bh as i32, stride: stride as usize });
                Ok(())
            }
            "unbuffer" => {
                let b = *nums.first().ok_or("usage: unbuffer n")? as u32;
                self.wins.get_mut(&n).unwrap().bufs.remove(&b).ok_or("no such buffer")?;
                Ok(())
            }
            "present" => {
                let b = *nums.first().ok_or("usage: present n [x0 y0 x1 y1]")? as u32;
                let w = &self.wins[&n];
                let buf = w.bufs.get(&b).ok_or("no such buffer")?;
                // what of the buffer the window shows: its top left corner
                let full = Rect::new(0, 0, buf.w.min(w.r.dx()), buf.h.min(w.r.dy()));
                let r = if nums.len() == 5 { Rect::new(nums[1], nums[2], nums[3], nums[4]).clip(&full) } else { full };
                let img = w.img;
                if !r.empty() {
                    self.d.load(img, &r, buf.seg.bytes(), buf.stride).map_err(|e| e.to_string())?;
                    self.paint(n, Some(r));
                }
                // copied: the client may write it again
                self.event(n, format!("b {b}"));
                Ok(())
            }
            "rect" => {
                if nums.len() != 4 {
                    return Err("usage: rect x0 y0 x1 y1".into());
                }
                self.setrect(n, Rect::new(nums[0], nums[1], nums[2], nums[3]))
            }
            "top" => {
                self.raise(n);
                self.setfocus(Some(n));
                Ok(())
            }
            "label" => {
                self.wins.get_mut(&n).unwrap().label = line[5..].trim().to_string();
                Ok(())
            }
            "delete" => {
                self.delwin(n, true);
                Ok(())
            }
            _ => Err(format!("unknown ctl message: {cmd}")),
        }
    }

    /// move window n to r; a new size gives it a new window in draw(3) that keeps
    /// what it showed (white beyond it), and its client an r event; a draw(3)
    /// program reads r from its mouse file either way
    fn setrect(&mut self, n: u32, r: Rect) -> Result<(), String> {
        if r.dx() < 1 || r.dy() < 1 || r.dx() > 16384 || r.dy() > 16384 {
            return Err("bad rectangle".into());
        }
        let old = self.wins[&n].r;
        if r == old {
            return Ok(());
        }
        if r.dx() != old.dx() || r.dy() != old.dy() {
            let img = self.image(r.dx(), r.dy())?;
            let w = &self.wins[&n];
            let (oldimg, oldlayer, oldname) = (w.img, w.layer, w.name.clone());
            let all = Rect::new(0, 0, r.dx(), r.dy());
            let (white, scr) = (self.white, self.scr);
            let d = &mut self.d;
            let made = d
                .draw(img, &all, white, 0, 0)
                .and_then(|_| d.draw(img, &all, oldimg, 0, 0))
                .and_then(|_| d.allocwindow(scr, r.inset(-BORDER), 0xFFFF_FFFF));
            let layer = match made {
                Ok(l) => l,
                Err(e) => {
                    let _ = self.d.free(img);
                    return Err(e.to_string());
                }
            };
            // what the old window showed (a draw(3) program's picture too)
            let keep = Rect::new(r.x0, r.y0, r.x0 + old.dx().min(r.dx()), r.y0 + old.dy().min(r.dy()));
            let name = format!("wsys.{}.{}.{}", std::process::id(), n, layer);
            let res = self
                .d
                .draw(layer, &r, white, 0, 0)
                .and_then(|_| self.d.draw(layer, &keep, oldlayer, old.x0, old.y0))
                .and_then(|_| self.d.name(oldlayer, &oldname, false))
                .and_then(|_| self.d.name(layer, &name, true));
            if let Err(e) = res {
                eprintln!("wsys: draw: {e}");
            }
            let _ = self.d.free(oldimg);
            let w = self.wins.get_mut(&n).unwrap();
            (w.img, w.layer, w.r, w.name) = (img, layer, r, name);
            // the stack as it was: those above go on top again
            let above: Vec<u32> = self.z.iter().skip_while(|&&i| i != n).skip(1).map(|i| self.wins[i].layer).collect();
            let _ = self.d.free(oldlayer);
            if !above.is_empty() {
                let _ = self.d.top(&above);
            }
            self.paintborder(n);
            self.event(n, format!("r {} {}", r.dx(), r.dy()));
        } else {
            let l = self.wins[&n].layer;
            self.wins.get_mut(&n).unwrap().r = r;
            let f = r.inset(-BORDER);
            if let Err(e) = self.d.origin(l, (f.x0, f.y0), (f.x0, f.y0)).and_then(|_| self.d.flush()) {
                eprintln!("wsys: draw: {e}");
            }
        }
        self.wins.get_mut(&n).unwrap().resized = true;
        self.changed(n);
        Ok(())
    }

    /// the frame of r on top of all (a sweep in progress): four thin windows
    fn outline(&mut self, r: Option<Rect>) {
        let res = (|| -> std::io::Result<()> {
            for l in std::mem::take(&mut self.sweep) {
                self.d.free(l)?;
            }
            if let Some(r) = r {
                let f = r.inset(-BORDER);
                let col = FOCUSED << 8 | 0xFF;
                for side in [
                    Rect::new(f.x0, f.y0, f.x1, r.y0),
                    Rect::new(f.x0, r.y1, f.x1, f.y1),
                    Rect::new(f.x0, r.y0, r.x0, r.y1),
                    Rect::new(r.x1, r.y0, f.x1, r.y1),
                ] {
                    let l = self.d.allocwindow(self.scr, side, col)?;
                    self.sweep.push(l);
                }
            }
            self.d.flush()
        })();
        if let Err(e) = res {
            eprintln!("wsys: draw: {e}");
        }
    }

    /// window n's border, in the colour of its focus
    fn paintborder(&mut self, n: u32) {
        let w = &self.wins[&n];
        let (l, r) = (w.layer, w.r);
        let f = r.inset(-BORDER);
        let col = if self.focus == Some(n) { self.cfocus } else { self.cother };
        let res = (|| -> std::io::Result<()> {
            for side in [
                Rect::new(f.x0, f.y0, f.x1, r.y0),
                Rect::new(f.x0, r.y1, f.x1, f.y1),
                Rect::new(f.x0, r.y0, r.x0, r.y1),
                Rect::new(r.x1, r.y0, f.x1, r.y1),
            ] {
                self.d.draw(l, &side, col, 0, 0)?;
            }
            self.d.flush()
        })();
        if let Err(e) = res {
            eprintln!("wsys: draw: {e}");
        }
    }

    /// what window n shows, all of it or rectangle r (window coordinates), from
    /// its image into its window in draw(3); devdraw shows what is not hidden
    fn paint(&mut self, n: u32, r: Option<Rect>) {
        let w = &self.wins[&n];
        let all = Rect::new(0, 0, w.r.dx(), w.r.dy());
        let whole = r.is_none();
        let r = r.unwrap_or(all).clip(&all);
        let (l, img, o) = (w.layer, w.img, (w.r.x0, w.r.y0));
        if whole {
            self.paintborder(n);
        }
        let res = self.d.draw(l, &r.add(o.0, o.1), img, r.x0, r.y0).and_then(|_| self.d.flush());
        if let Err(e) = res {
            eprintln!("wsys: draw: {e}");
        }
    }

    fn mouse(&mut self, x: i32, y: i32, b: i32, ms: u32) {
        (self.mx, self.my, self.ms) = (x, y, ms);
        if let Some(g) = self.drag {
            self.dragto(g, x, y, b);
            self.buttons = b;
            return;
        }
        let pressed = b != 0 && self.buttons == 0;
        if pressed {
            let hit = self.z.iter().rev().copied().find(|n| self.wins[n].r.inset(-BORDER).contains(x, y));
            if let Some(n) = hit {
                if self.focus != Some(n) {
                    self.setfocus(Some(n));
                }
                self.raise(n);
                // on the border: button 1 moves the window, button 3 sweeps it
                let r = self.wins[&n].r;
                if !r.contains(x, y) && (b == 1 || b == 4) {
                    let kind = if b == 1 {
                        DragKind::Move
                    } else {
                        let near = |v: i32, a: i32| (v - a).abs() < CORNER;
                        let (top, bottom) = (y < r.y0, y >= r.y1);
                        let (left, right) = (x < r.x0, x >= r.x1);
                        DragKind::Sweep {
                            left: left || ((top || bottom) && near(x, r.x0)),
                            right: right || ((top || bottom) && near(x, r.x1)),
                            top: top || ((left || right) && near(y, r.y0)),
                            bottom: bottom || ((left || right) && near(y, r.y1)),
                        }
                    };
                    self.drag = Some(Drag { win: n, kind, x0: x, y0: y, orig: r, cur: r });
                    self.buttons = b;
                    return;
                }
            }
        }
        let held = b != 0 || self.buttons != 0;
        self.buttons = b;
        if let Some(n) = self.focus {
            let r = self.wins[&n].r;
            if held || r.contains(x, y) {
                self.event(n, format!("m {} {} {} {}", x - r.x0, y - r.y0, b, ms));
                let w = self.wins.get_mut(&n).unwrap();
                if w.mouseopen > 0 {
                    // a move replaces the last one queued with the same buttons
                    if w.mouse.len() >= MAXMOUSE || w.mouse.back().is_some_and(|m| m.2 == b && w.mouse.len() > 1) {
                        w.mouse.pop_back();
                    }
                    w.mouse.push_back((x, y, b, ms));
                    self.wake(n);
                }
            }
        }
    }

    /// a window moved or swept with the mouse: no events for the client but the
    /// new size at the end of a sweep
    fn dragto(&mut self, mut g: Drag, x: i32, y: i32, b: i32) {
        if !self.wins.contains_key(&g.win) {
            self.drag = None;
            return;
        }
        let (dx, dy) = (x - g.x0, y - g.y0);
        match g.kind {
            DragKind::Move => {
                let _ = self.setrect(g.win, g.orig.add(dx, dy));
                if b == 0 {
                    self.drag = None;
                }
            }
            DragKind::Sweep { left, right, top, bottom } => {
                let o = g.orig;
                let mut r = Rect::new(
                    if left { o.x0 + dx } else { o.x0 },
                    if top { o.y0 + dy } else { o.y0 },
                    if right { o.x1 + dx } else { o.x1 },
                    if bottom { o.y1 + dy } else { o.y1 },
                );
                if r.dx() < MINSIZE {
                    if left { r.x0 = r.x1 - MINSIZE } else { r.x1 = r.x0 + MINSIZE }
                }
                if r.dy() < MINSIZE {
                    if top { r.y0 = r.y1 - MINSIZE } else { r.y1 = r.y0 + MINSIZE }
                }
                g.cur = r;
                if b == 0 {
                    self.drag = None;
                    self.outline(None);
                    let _ = self.setrect(g.win, r);
                } else {
                    self.drag = Some(g);
                    self.outline(Some(r));
                }
            }
        }
    }

    /// kbd(3) messages to the focused window: all of them to its kbd file when
    /// open, else the runes of the c messages to cons (and wsys' k events)
    fn kbd(&mut self, msgs: Vec<Vec<u8>>) {
        let Some(n) = self.focus else { return };
        let Some(w) = self.wins.get_mut(&n) else { return };
        if w.kbdopen > 0 {
            for mut m in msgs {
                m.push(0);
                w.kbd.push_back(m);
            }
            self.wake(n);
            return;
        }
        for m in msgs {
            if m.first() != Some(&b'c') {
                continue;
            }
            let s = String::from_utf8_lossy(&m[1..]).into_owned();
            for c in s.chars() {
                self.event(n, format!("k {:x}", c as u32));
                self.typed(n, c);
            }
        }
        self.wake(n);
    }

    /// a rune typed into window n's cons: raw as it comes, else a line at a time
    fn typed(&mut self, n: u32, c: char) {
        let w = self.wins.get_mut(&n).unwrap();
        let mut b = [0u8; 4];
        let bytes = c.encode_utf8(&mut b).as_bytes();
        if w.raw {
            w.cons.extend(bytes);
            return;
        }
        match c {
            '\u{7f}' => {
                // Del: interrupt, the line goes
                w.line.clear();
                if let Some(pid) = w.pid {
                    postnote(pid, "interrupt");
                }
            }
            '\u{8}' => {
                // backspace: the last rune
                while let Some(x) = w.line.pop() {
                    if x & 0xC0 != 0x80 {
                        break;
                    }
                }
            }
            '\u{15}' => w.line.clear(), // ^U: the line
            '\u{4}' => {
                // ^D: the line as it is; on an empty one, end of file
                if w.line.is_empty() {
                    w.eof = true;
                }
                let l = std::mem::take(&mut w.line);
                w.cons.extend(l);
            }
            '\n' => {
                let mut l = std::mem::take(&mut w.line);
                l.push(b'\n');
                w.cons.extend(l);
            }
            _ => w.line.extend_from_slice(bytes),
        }
    }
}

/// a note to the note group of process pid (proc(3))
fn postnote(pid: u32, note: &str) {
    if let Ok(f) = Fd::open(&format!("/proc/{pid}/notepg"), OWRITE) {
        let _ = f.write(note.as_bytes());
    }
}

fn reader(fd: i32, size: usize, tx: mpsc::Sender<Ev>) {
    let f = std::mem::ManuallyDrop::new(Fd(fd));
    let mut b = vec![0u8; size];
    loop {
        match f.read(&mut b) {
            Ok(0) | Err(_) => {
                let _ = tx.send(Ev::Hangup);
                return;
            }
            Ok(n) => {
                if tx.send(Ev::Msg(b[..n].to_vec())).is_err() {
                    return;
                }
            }
        }
    }
}

/// the mouse: fd is /dev/mouse, open once (the kernel's takes one opener; wsys
/// writes to it too, to move the pointer)
fn mousereader(fd: i32, tx: mpsc::Sender<Ev>) {
    let f = std::mem::ManuallyDrop::new(Fd(fd));
    let mut b = [0u8; 49];
    while let Ok(n) = f.read(&mut b) {
        if n < 49 || b[0] != b'm' {
            continue; // 'r': the screen was resized
        }
        let v: Vec<i64> = String::from_utf8_lossy(&b[1..n]).split_whitespace().filter_map(|s| s.parse().ok()).collect();
        if v.len() == 4 && tx.send(Ev::Mouse { x: v[0] as i32, y: v[1] as i32, b: v[2] as i32, ms: v[3] as u32 }).is_err() {
            return;
        }
    }
}

/// the keyboard: kbd(3)'s messages from /dev/kbd (open, kbdfs gives cons
/// nothing: nobody else takes keys); without it /dev/cons, raw, as c messages
fn kbdreader(tx: mpsc::Sender<Ev>) {
    let mut b = [0u8; 1024];
    if let Ok(f) = Fd::open("/dev/kbd", OREAD) {
        while let Ok(n) = f.read(&mut b) {
            if n == 0 {
                break;
            }
            let msgs: Vec<Vec<u8>> = b[..n].split(|&c| c == 0).filter(|m| !m.is_empty()).map(|m| m.to_vec()).collect();
            if tx.send(Ev::Kbd(msgs)).is_err() {
                break;
            }
        }
        return;
    }
    let (Ok(ctl), Ok(f)) = (Fd::open("/dev/consctl", OWRITE), Fd::open("/dev/cons", OREAD)) else {
        eprintln!("wsys: no /dev/kbd or /dev/cons: {}", p9::err());
        return;
    };
    let _ = ctl.write(b"rawon");
    let mut part = vec![];
    while let Ok(n) = f.read(&mut b) {
        if n == 0 {
            break;
        }
        part.extend_from_slice(&b[..n]);
        // whole runes only
        let ok = match std::str::from_utf8(&part) {
            Ok(_) => part.len(),
            Err(e) => e.valid_up_to(),
        };
        let s = String::from_utf8_lossy(&part[..ok]).into_owned();
        part.drain(..ok);
        let msgs: Vec<Vec<u8>> = s.chars().map(|c| format!("c{c}").into_bytes()).collect();
        if !msgs.is_empty() && tx.send(Ev::Kbd(msgs)).is_err() {
            break;
        }
    }
    drop(ctl);
}

fn usage() -> ! {
    eprintln!("usage: wsys [-s srvname] [-i cmd] [-x]");
    std::process::exit(1);
}

fn main() {
    let mut args = std::env::args().skip(1);
    let user = std::env::var("user").unwrap_or_else(|_| "none".into());
    // with its pid as rio: a wsys that died leaves its entry, another starts all the same
    let mut srvname = format!("wsys.{user}.{}", std::process::id());
    let (mut init, mut exitwith) = (None, false);
    while let Some(a) = args.next() {
        match a.as_str() {
            "-s" => srvname = args.next().unwrap_or_else(|| usage()),
            "-i" => init = Some(args.next().unwrap_or_else(|| usage())),
            "-x" => exitwith = true,
            _ => usage(),
        }
    }
    // a note group of wsys' own (as rio): it goes as a whole at the end
    if let Err(e) = p9::rfork(p9::RFNOTEG) {
        eprintln!("wsys: rfork: {e}");
    }
    let d = match Display::open() {
        Ok(d) => d,
        Err(e) => {
            eprintln!("wsys: draw: {e}");
            std::process::exit(1);
        }
    };

    // the connection: one end posted in /srv, wsys serves the other
    let (p0, p1) = p9::pipe().unwrap_or_else(|e| {
        eprintln!("wsys: pipe: {e}");
        std::process::exit(1);
    });
    let srvpath = format!("/srv/{srvname}");
    let post = Fd::create(&srvpath, OWRITE, 0o600).unwrap_or_else(|e| {
        eprintln!("wsys: post {srvpath}: {e}");
        std::process::exit(1);
    });
    if let Err(e) = post.write1(p0.0.to_string().as_bytes()) {
        eprintln!("wsys: post {srvpath}: {e}");
        std::process::exit(1);
    }
    post.leak(); // the entry stays while wsys runs
    drop(p0);

    let (tx, rx) = mpsc::channel();
    let srvfd = p1.0;
    {
        let tx = tx.clone();
        thread::spawn(move || reader(srvfd, 65536 + ninep::IOHDRSZ as usize, tx));
    }
    let mousefd = Fd::open("/dev/mouse", ORDWR).inspect_err(|e| eprintln!("wsys: /dev/mouse: {e}")).ok();
    if let Some(fd) = &mousefd {
        let (tx, fd) = (tx.clone(), fd.0);
        thread::spawn(move || mousereader(fd, tx));
    }
    {
        let tx = tx.clone();
        thread::spawn(move || kbdreader(tx));
    }
    let mut ws = Wsys {
        srv: p1,
        srvpath: srvpath.clone(),
        msize: 8192 + ninep::IOHDRSZ,
        uid: user,
        fids: HashMap::new(),
        wins: HashMap::new(),
        z: vec![],
        focus: None,
        nextwin: 0,
        pending: vec![],
        d,
        bg: 0,
        cfocus: 0,
        cother: 0,
        buttons: 0,
        mx: 0,
        my: 0,
        ms: 0,
        white: 0,
        slots: vec![false; NSLOT],
        drag: None,
        scr: 0,
        sweep: vec![],
        snarf: vec![],
        mousefd,
        cursorfd: Fd::open("/dev/cursor", OWRITE).ok(),
        tx: tx.clone(),
    };
    let colors = (|| -> std::io::Result<(u32, u32, u32, u32)> {
        Ok((ws.d.color(BG)?, ws.d.color(FOCUSED)?, ws.d.color(UNFOCUSED)?, ws.d.color(WHITE)?))
    })();
    match colors {
        Ok((a, b, c, w)) => (ws.bg, ws.cfocus, ws.cother, ws.white) = (a, b, c, w),
        Err(e) => {
            eprintln!("wsys: colours: {e}");
            std::process::exit(1);
        }
    }
    let all = ws.d.r;
    let made = (|| -> std::io::Result<u32> {
        let (img, bg) = (ws.d.screen, ws.bg);
        ws.d.draw(img, &all, bg, 0, 0)?;
        let scr = ws.d.allocscreen(img, bg)?;
        ws.d.flush()?;
        Ok(scr)
    })();
    match made {
        Ok(scr) => ws.scr = scr,
        Err(e) => {
            eprintln!("wsys: screen: {e}");
            std::process::exit(1);
        }
    }
    println!("wsys: {srvpath} {} {} {} {}", all.x0, all.y0, all.x1, all.y1);
    let _ = std::io::stdout().flush();

    if let Some(cmd) = init {
        let tx = tx.clone();
        let script = format!("rfork ns; mount -c {srvpath} /mnt/wsys || exit mount; wsys={srvpath}; {cmd}");
        thread::spawn(move || {
            let st = std::process::Command::new("/bin/rc").arg("-c").arg(&script).status();
            let s = match st {
                Ok(s) if s.success() => String::new(),
                Ok(s) => format!("{s}"),
                Err(e) => e.to_string(),
            };
            let _ = tx.send(Ev::ChildExit(s));
        });
    }
    drop(tx);

    // at the end the windows' processes get hangup and wsys serves them a little
    // longer, until they let go of their windows (rio exits on hangup, but a note
    // like a write on wsys' closed pipe would break it)
    let mut quit: Option<std::time::Instant> = None;
    loop {
        let ev = match quit {
            None => match rx.recv() {
                Ok(ev) => ev,
                Err(_) => break,
            },
            Some(t) => {
                let left = std::time::Duration::from_secs(2).saturating_sub(t.elapsed());
                if ws.wins.values().all(|w| w.pid.is_none()) || left.is_zero() {
                    break;
                }
                match rx.recv_timeout(left) {
                    Ok(ev) => ev,
                    Err(_) => break,
                }
            }
        };
        match ev {
            Ev::Msg(m) => ws.handle(&m),
            Ev::Hangup => break,
            Ev::Mouse { x, y, b, ms } => ws.mouse(x, y, b, ms),
            Ev::Kbd(m) => ws.kbd(m),
            Ev::WinExit(n) => {
                // its process is gone: the window too, unless files of it are open
                if let Some(w) = ws.wins.get_mut(&n) {
                    w.pinned = false;
                    if w.refs == 0 {
                        ws.delwin(n, false);
                    }
                }
            }
            Ev::ChildExit(s) => {
                if exitwith && quit.is_none() {
                    if !s.is_empty() {
                        eprintln!("wsys: -i: {s}");
                    }
                    for w in ws.wins.values() {
                        if let Some(pid) = w.pid {
                            postnote(pid, "hangup");
                        }
                    }
                    quit = Some(std::time::Instant::now());
                }
            }
        }
    }
    let _ = p9::remove(&srvpath);
    // the windows let go of their clients' segments; their processes hang up
    let ids: Vec<u32> = ws.wins.keys().copied().collect();
    for n in ids {
        ws.delwin(n, true);
    }
    // std's threads are procs of their own, blocked in reads of the mouse and the
    // keyboard: they go with wsys' note group (windows and -i have groups of their own)
    postnote(std::process::id(), "kill");
}
