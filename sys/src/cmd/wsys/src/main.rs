// SPDX-License-Identifier: MIT
//
// wsys: the window system of ../9front-ui (PLAN.md stage 3), first prototype
// with the processor backend. A 9P file server (wsys(4)):
//
//	/mnt/wsys/screen	read: x0 y0 x1 y1, where wsys draws (all of the
//				screen, or inside its rio window)
//	/mnt/wsys/clone		open: a new window; the fid is then its ctl
//	/mnt/wsys/N/ctl		read: id x0 y0 x1 y1 w h base size focus
//				write: rect x0 y0 x1 y1 | buffer n seg w h stride |
//				unbuffer n | present n [x0 y0 x1 y1] | top |
//				label text | delete
//	/mnt/wsys/N/event	read (blocks): lines m x y buttons msec | k hex-rune |
//				r w h | f 0|1 | b n
//
// The pixels are the client's: global segments of segment(3) it makes, at
// addresses inside the window's range (base, size: wsys gives each window one
// of its own, so that its segments cannot collide in wsys, where segment(3)
// attaches them at the same address). buffer n announces one (x8r8g8b8, w x h,
// stride bytes a line); present n copies a rectangle of it into the window's
// image in draw(3), composes the screen there and answers with the event
// b n: buffer n is free again (a swap chain of the client's). A new size
// keeps the window's image until the client presents a buffer of it.
// Composition: background, windows bottom to top with a border, through
// devdraw on the processor. Input: /dev/mouse and /dev/cons (raw) to the
// focused window; a button pressed on another window gives it the focus and
// raises it. On a window's border button 1 moves it, button 3 sweeps it to a
// new size (an outline until the button comes up).
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
use p9::{Fd, OREAD, OWRITE, Seg};
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

enum Ev {
    Msg(Vec<u8>),
    Hangup,
    Mouse { x: i32, y: i32, b: i32, ms: u32 },
    Kbd(String),
    ChildExit(String),
}

#[derive(Clone, Copy, PartialEq, Debug)]
enum Kind {
    Root,
    Screen,
    Clone,
    WinDir(u32),
    Ctl(u32),
    Event(u32),
}

struct FidState {
    kind: Kind,
    open: bool,
    dirpos: usize,
    owns: Option<u32>, // the clone fid that made the window: clunked, the window goes
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
    img: u32, // what the window shows, its size
    label: String,
    events: VecDeque<String>,
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

struct Pending {
    tag: u16,
    fid: u32,
    win: u32,
    count: u32,
}

struct Wsys {
    srv: Fd,
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
    white: u32,
    slots: Vec<bool>, // in use
    drag: Option<Drag>,
}

fn qid(k: Kind) -> Qid {
    let (ty, path) = match k {
        Kind::Root => (ninep::QTDIR, 0),
        Kind::Clone => (0, 1),
        Kind::Screen => (0, 5),
        Kind::WinDir(n) => (ninep::QTDIR, (n as u64) << 4 | 2),
        Kind::Ctl(n) => (0, (n as u64) << 4 | 3),
        Kind::Event(n) => (0, (n as u64) << 4 | 4),
    };
    Qid { ty, vers: 0, path }
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
            Kind::WinDir(n) => (n.to_string(), ninep::DMDIR | 0o555),
            Kind::Ctl(_) => ("ctl".to_string(), 0o666),
            Kind::Event(_) => ("event".to_string(), 0o444),
        };
        Dir { qid: qid(k), mode, length: 0, name, uid: self.uid.clone(), mtime: 0 }
    }

    fn exists(&self, k: Kind) -> bool {
        match k {
            Kind::WinDir(n) | Kind::Ctl(n) | Kind::Event(n) => self.wins.contains_key(&n),
            _ => true,
        }
    }

    fn walk1(&self, k: Kind, name: &str) -> Option<Kind> {
        let n = match k {
            Kind::Root => {
                if name == "clone" {
                    return Some(Kind::Clone);
                }
                if name == "screen" {
                    return Some(Kind::Screen);
                }
                if name == ".." {
                    return Some(Kind::Root);
                }
                let n: u32 = name.parse().ok()?;
                return self.wins.contains_key(&n).then_some(Kind::WinDir(n));
            }
            Kind::WinDir(n) => n,
            _ => return None,
        };
        match name {
            "ctl" => Some(Kind::Ctl(n)),
            "event" => Some(Kind::Event(n)),
            ".." => Some(Kind::Root),
            _ => None,
        }
    }

    fn children(&self, k: Kind) -> Vec<Kind> {
        match k {
            Kind::Root => {
                let mut v = vec![Kind::Screen, Kind::Clone];
                let mut ids: Vec<u32> = self.wins.keys().copied().collect();
                ids.sort();
                v.extend(ids.into_iter().map(Kind::WinDir));
                v
            }
            Kind::WinDir(n) => vec![Kind::Ctl(n), Kind::Event(n)],
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

    /// the reply, or None when it comes later (a read that waits for events)
    fn answer(&mut self, tag: u16, t: T) -> Option<Vec<u8>> {
        let e = |s: &str| Some(ninep::error(tag, s));
        match t {
            T::Version { msize, version } => {
                self.msize = msize.min(65536);
                self.fids.clear();
                let v = if version.starts_with("9P2000") { "9P2000" } else { "unknown" };
                Some(R::new(101, tag).u32(self.msize).str(v).done())
            }
            T::Auth => e("wsys: no authentication required"),
            T::Attach { fid, _uname: _ } => {
                if self.fids.contains_key(&fid) {
                    return e("fid in use");
                }
                self.fids.insert(fid, FidState { kind: Kind::Root, open: false, dirpos: 0, owns: None });
                Some(R::new(105, tag).qid(qid(Kind::Root)).done())
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
                let mut k = f.kind;
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
                    self.fids.insert(newfid, FidState { kind: k, open: false, dirpos: 0, owns: None });
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
                let k = match k {
                    Kind::Root | Kind::WinDir(_) if rw != OREAD as u8 => return e("is a directory"),
                    Kind::Event(_) | Kind::Screen if rw != OREAD as u8 => return e("permission denied"),
                    Kind::Clone => match self.newwin() {
                        Ok(n) => {
                            let f = self.fids.get_mut(&fid).unwrap();
                            f.owns = Some(n);
                            Kind::Ctl(n)
                        }
                        Err(err) => return e(&format!("new window: {err}")),
                    },
                    k => k,
                };
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
                    Kind::Ctl(_) | Kind::Screen => {
                        let s = match k {
                            Kind::Ctl(n) => self.ctltext(n),
                            _ => format!("{} {} {} {}\n", self.d.r.x0, self.d.r.y0, self.d.r.x1, self.d.r.y1),
                        };
                        let b = s.as_bytes();
                        let o = (offset as usize).min(b.len());
                        let b = &b[o..(o + count as usize).min(b.len())];
                        Some(R::new(117, tag).u32(b.len() as u32).bytes(b).done())
                    }
                    Kind::Event(n) => {
                        if self.wins[&n].events.is_empty() {
                            self.pending.push(Pending { tag, fid, win: n, count });
                            None
                        } else {
                            Some(self.eventreply(tag, n, count))
                        }
                    }
                    Kind::Clone => e("clone not opened"),
                }
            }
            T::Write { fid, _offset: _, data } => {
                let Some(f) = self.fids.get(&fid) else { return e("unknown fid") };
                if !f.open {
                    return e("not open");
                }
                let Kind::Ctl(n) = f.kind else { return e("permission denied") };
                if !self.wins.contains_key(&n) {
                    return e("window deleted");
                }
                let s = String::from_utf8_lossy(&data).into_owned();
                for line in s.lines() {
                    if let Err(err) = self.ctl(n, line.trim()) {
                        return e(&err);
                    }
                }
                Some(R::new(119, tag).u32(data.len() as u32).done())
            }
        }
    }

    fn clunk(&mut self, fid: u32) {
        if let Some(f) = self.fids.remove(&fid) {
            self.pending.retain(|p| p.fid != fid);
            if let Some(n) = f.owns {
                self.delwin(n);
            }
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

    fn eventreply(&mut self, tag: u16, n: u32, count: u32) -> Vec<u8> {
        let w = self.wins.get_mut(&n).unwrap();
        let mut out = String::new();
        while let Some(l) = w.events.front() {
            if !out.is_empty() && out.len() + l.len() > count as usize {
                break;
            }
            out.push_str(l);
            w.events.pop_front();
        }
        let b = &out.as_bytes()[..out.len().min(count as usize)];
        R::new(117, tag).u32(b.len() as u32).bytes(b).done()
    }

    fn event(&mut self, n: u32, line: String) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        if w.events.len() >= MAXEVENTS {
            w.events.pop_front();
        }
        w.events.push_back(line + "\n");
        if let Some(i) = self.pending.iter().position(|p| p.win == n) {
            let p = self.pending.remove(i);
            let r = self.eventreply(p.tag, n, p.count);
            self.send(r);
        }
    }

    /// a white image of w x h for a window
    fn image(&mut self, w: i32, h: i32) -> Result<u32, String> {
        let img = self.d.alloc(Rect::new(0, 0, w, h), XRGB32, false, 0xFFFF_FFFF).map_err(|e| e.to_string())?;
        Ok(img)
    }

    fn newwin(&mut self) -> Result<u32, String> {
        let slot = self.slots.iter().skip(1).position(|&u| !u).map(|i| i + 1).ok_or("too many windows")?;
        self.nextwin += 1;
        let n = self.nextwin;
        let s = self.d.r;
        let off = 40 + 30 * ((n as i32 - 1) % 10);
        let r = Rect::new(s.x0 + off, s.y0 + off, s.x0 + off + 400, s.y0 + off + 300).clip(&s.inset(BORDER));
        if r.empty() {
            return Err("screen too small".into());
        }
        let img = self.image(r.dx(), r.dy())?;
        self.slots[slot] = true;
        self.wins.insert(n, Win { r, slot, bufs: HashMap::new(), img, label: String::new(), events: VecDeque::new() });
        self.z.push(n);
        self.setfocus(Some(n));
        self.redraw(r.inset(-BORDER));
        Ok(n)
    }

    fn delwin(&mut self, n: u32) {
        let Some(w) = self.wins.remove(&n) else { return };
        let _ = self.d.free(w.img);
        self.slots[w.slot] = false; // its buffers go with w
        self.z.retain(|&i| i != n);
        for p in self.pending.iter().filter(|p| p.win == n) {
            self.send(ninep::error(p.tag, "window deleted"));
        }
        self.pending.retain(|p| p.win != n);
        if self.focus == Some(n) {
            self.focus = None;
            let top = self.z.last().copied();
            self.setfocus(top);
        }
        self.redraw(w.r.inset(-BORDER));
    }

    fn setfocus(&mut self, n: Option<u32>) {
        if self.focus == n {
            return;
        }
        let old = self.focus;
        self.focus = n;
        for (id, v) in [(old, 0), (n, 1)] {
            if let Some(id) = id {
                self.event(id, format!("f {v}"));
                if let Some(w) = self.wins.get(&id) {
                    let r = w.r.inset(-BORDER);
                    self.redraw(r);
                }
            }
        }
    }

    fn raise(&mut self, n: u32) {
        if self.z.last() != Some(&n) {
            self.z.retain(|&i| i != n);
            self.z.push(n);
            let r = self.wins[&n].r.inset(-BORDER);
            self.redraw(r);
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
                let (img, origin) = (w.img, (w.r.x0, w.r.y0));
                if !r.empty() {
                    self.d.load(img, &r, buf.seg.bytes(), buf.stride).map_err(|e| e.to_string())?;
                    self.redraw(r.add(origin.0, origin.1));
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
                self.delwin(n);
                Ok(())
            }
            _ => Err(format!("unknown ctl message: {cmd}")),
        }
    }

    /// move window n to r; a new size gives it a new image that keeps what it
    /// showed (white beyond it) and the client an r event
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
            let oldimg = self.wins[&n].img;
            let all = Rect::new(0, 0, r.dx(), r.dy());
            let (white, d) = (self.white, &mut self.d);
            let res = d.draw(img, &all, white, 0, 0).and_then(|_| d.draw(img, &all, oldimg, 0, 0));
            let _ = d.free(oldimg);
            res.map_err(|e| e.to_string())?;
            self.wins.get_mut(&n).unwrap().img = img;
            self.event(n, format!("r {} {}", r.dx(), r.dy()));
        }
        self.wins.get_mut(&n).unwrap().r = r;
        self.redraw(old.inset(-BORDER).union(&r.inset(-BORDER)));
        Ok(())
    }

    /// the frame of r drawn on the screen (a sweep in progress)
    fn outline(&mut self, r: Rect) {
        let f = r.inset(-BORDER);
        let (scr, col) = (self.d.screen, self.cfocus);
        let sides = [
            Rect::new(f.x0, f.y0, f.x1, r.y0),
            Rect::new(f.x0, r.y1, f.x1, f.y1),
            Rect::new(f.x0, r.y0, r.x0, r.y1),
            Rect::new(r.x1, r.y0, f.x1, r.y1),
        ];
        let res = (|| -> std::io::Result<()> {
            for side in sides {
                self.d.draw(scr, &side.clip(&self.d.r), col, 0, 0)?;
            }
            self.d.flush()
        })();
        if let Err(e) = res {
            eprintln!("wsys: draw: {e}");
        }
    }

    /// compose rectangle r of the screen: background, then the windows bottom to top
    fn redraw(&mut self, r: Rect) {
        let r = r.clip(&self.d.r);
        if r.empty() {
            return;
        }
        let res = (|| -> std::io::Result<()> {
            let scr = self.d.screen;
            self.d.draw(scr, &r, self.bg, 0, 0)?;
            for &n in &self.z {
                let w = &self.wins[&n];
                let col = if self.focus == Some(n) { self.cfocus } else { self.cother };
                let frame = w.r.inset(-BORDER).clip(&r);
                self.d.draw(scr, &frame, col, 0, 0)?;
                let c = w.r.clip(&r);
                self.d.draw(scr, &c, w.img, c.x0 - w.r.x0, c.y0 - w.r.y0)?;
            }
            self.d.flush()
        })();
        if let Err(e) = res {
            eprintln!("wsys: draw: {e}");
        }
    }

    fn mouse(&mut self, x: i32, y: i32, b: i32, ms: u32) {
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
                let prev = g.cur;
                g.cur = r;
                self.redraw(prev.inset(-BORDER));
                if b == 0 {
                    self.drag = None;
                    let _ = self.setrect(g.win, r);
                } else {
                    self.drag = Some(g);
                    self.outline(r);
                }
            }
        }
    }

    fn kbd(&mut self, s: &str) {
        if let Some(n) = self.focus {
            for c in s.chars() {
                self.event(n, format!("k {:x}", c as u32));
            }
        }
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

fn mousereader(tx: mpsc::Sender<Ev>) {
    let Ok(f) = Fd::open("/dev/mouse", OREAD) else {
        eprintln!("wsys: no /dev/mouse: {}", p9::err());
        return;
    };
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

fn kbdreader(tx: mpsc::Sender<Ev>) {
    let (Ok(ctl), Ok(f)) = (Fd::open("/dev/consctl", OWRITE), Fd::open("/dev/cons", OREAD)) else {
        eprintln!("wsys: no /dev/cons: {}", p9::err());
        return;
    };
    let _ = ctl.write(b"rawon");
    let mut b = [0u8; 256];
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
        if !s.is_empty() && tx.send(Ev::Kbd(s)).is_err() {
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
    {
        let tx = tx.clone();
        thread::spawn(move || mousereader(tx));
    }
    {
        let tx = tx.clone();
        thread::spawn(move || kbdreader(tx));
    }
    let mut ws = Wsys {
        srv: p1,
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
        white: 0,
        slots: vec![false; NSLOT],
        drag: None,
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
    ws.redraw(all);
    println!("wsys: {srvpath} {} {} {} {}", all.x0, all.y0, all.x1, all.y1);
    let _ = std::io::stdout().flush();

    if let Some(cmd) = init {
        let tx = tx.clone();
        let script = format!("rfork n; mount -c {srvpath} /mnt/wsys || exit mount; wsys={srvpath}; {cmd}");
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

    for ev in rx {
        match ev {
            Ev::Msg(m) => ws.handle(&m),
            Ev::Hangup => break,
            Ev::Mouse { x, y, b, ms } => ws.mouse(x, y, b, ms),
            Ev::Kbd(s) => ws.kbd(&s),
            Ev::ChildExit(s) => {
                if exitwith {
                    if !s.is_empty() {
                        eprintln!("wsys: -i: {s}");
                    }
                    break;
                }
            }
        }
    }
    let _ = p9::remove(&srvpath);
    // the windows let go of their clients' segments
    let ids: Vec<u32> = ws.wins.keys().copied().collect();
    for n in ids {
        ws.delwin(n);
    }
}
