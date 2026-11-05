// SPDX-License-Identifier: MIT
//
// wsys: the window system of ../9front-ui (PLAN.md stage 3), first prototype
// with the processor backend. A 9P file server (wsys(4)):
//
//	/mnt/wsys/clone		open: a new window; the fid is then its ctl
//	/mnt/wsys/N/ctl		read: id x0 y0 x1 y1 w h stride seg va generation focus
//				write: rect x0 y0 x1 y1 | present [x0 y0 x1 y1] |
//				top | label text | delete
//	/mnt/wsys/N/event	read (blocks): lines m x y buttons msec | k hex-rune |
//				r w h generation | f 0|1
//
// A window's pixels (XRGB32, stride w*4) are in a global segment of
// segment(3) that wsys makes at an address of its choosing; the client
// attaches it by the name ctl gives (same address). present copies the
// rectangle into the window's image in draw(3) and composes the screen there:
// background, windows bottom to top with a border, through devdraw on the
// processor. Input: /dev/mouse and /dev/cons (raw) to the focused window;
// a button pressed on another window gives it the focus and raises it.
//
// usage: wsys [-s srvname] [-i cmd] [-x]
//	-s	post as /srv/srvname (default wsys.$user)
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
const SEGBASE: usize = 0x5000_0000_0000; // window N's segment at SEGBASE + N*SEGSTEP
const SEGSTEP: usize = 0x1_0000_0000;
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

struct Win {
    r: Rect, // content, in screen coordinates
    w: i32,
    h: i32,
    seg: Option<Seg>,
    img: u32,
    generation: u32,
    label: String,
    events: VecDeque<String>,
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
    pid: u32,
}

fn qid(k: Kind) -> Qid {
    let (ty, path) = match k {
        Kind::Root => (ninep::QTDIR, 0),
        Kind::Clone => (0, 1),
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
                let mut v = vec![Kind::Clone];
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
                    Kind::Event(_) if rw != OREAD as u8 => return e("permission denied"),
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
                    Kind::Ctl(n) => {
                        let s = self.ctltext(n);
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
        let (name, va) = match &w.seg {
            Some(s) => (s.name.as_str(), s.va),
            None => ("none", 0),
        };
        format!(
            "{} {} {} {} {} {} {} {} {} {:#x} {} {}\n",
            n, w.r.x0, w.r.y0, w.r.x1, w.r.y1, w.w, w.h, w.w * 4, name, va, w.generation,
            (self.focus == Some(n)) as i32
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

    /// a segment and a draw image of w x h for window n
    fn buffers(&mut self, n: u32, w: i32, h: i32, generation: u32) -> Result<(Seg, u32), String> {
        let len = (w as usize * h as usize * 4).max(4096);
        if len > SEGSTEP {
            return Err("window too big".into());
        }
        let name = format!("wsys.{}.{}.{}", self.pid, n, generation);
        let seg = Seg::new(&name, SEGBASE + n as usize * SEGSTEP, len).map_err(|e| format!("segment {name}: {e}"))?;
        let img = self.d.alloc(Rect::new(0, 0, w, h), XRGB32, false, 0xFFFF_FFFF).map_err(|e| e.to_string())?;
        Ok((seg, img))
    }

    fn newwin(&mut self) -> Result<u32, String> {
        self.nextwin += 1;
        let n = self.nextwin;
        let s = self.d.r;
        let off = 40 + 30 * ((n as i32 - 1) % 10);
        let r = Rect::new(s.x0 + off, s.y0 + off, s.x0 + off + 400, s.y0 + off + 300).clip(&s.inset(BORDER));
        if r.empty() {
            return Err("screen too small".into());
        }
        let (seg, img) = self.buffers(n, r.dx(), r.dy(), 0)?;
        // white, as the segment is zero
        self.wins.insert(n, Win {
            r, w: r.dx(), h: r.dy(), seg: Some(seg), img, generation: 0, label: String::new(), events: VecDeque::new(),
        });
        self.z.push(n);
        self.setfocus(Some(n));
        self.redraw(r.inset(-BORDER));
        Ok(n)
    }

    fn delwin(&mut self, n: u32) {
        let Some(w) = self.wins.remove(&n) else { return };
        let _ = self.d.free(w.img);
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
            "present" => {
                let w = &self.wins[&n];
                let full = Rect::new(0, 0, w.w, w.h);
                let r = if nums.len() == 4 { Rect::new(nums[0], nums[1], nums[2], nums[3]).clip(&full) } else { full };
                if r.empty() {
                    return Ok(());
                }
                let (img, origin) = (w.img, (w.r.x0, w.r.y0));
                let seg = w.seg.as_ref().ok_or("no buffer")?;
                let stride = w.w as usize * 4;
                // the segment outlives the borrow: wsys keeps it attached
                let px = unsafe { std::slice::from_raw_parts(seg.bytes().as_ptr(), seg.len) };
                self.d.load(img, &r, px, stride).map_err(|e| e.to_string())?;
                self.redraw(r.add(origin.0, origin.1));
                Ok(())
            }
            "rect" => {
                if nums.len() != 4 {
                    return Err("usage: rect x0 y0 x1 y1".into());
                }
                let r = Rect::new(nums[0], nums[1], nums[2], nums[3]);
                if r.dx() < 1 || r.dy() < 1 {
                    return Err("bad rectangle".into());
                }
                let old = self.wins[&n].r;
                if r.dx() != old.dx() || r.dy() != old.dy() {
                    let generation = self.wins[&n].generation + 1;
                    // the old segment goes first: the new one takes its address
                    // (the client detaches the old one at the r event)
                    self.wins.get_mut(&n).unwrap().seg = None;
                    let (seg, img) = self.buffers(n, r.dx(), r.dy(), generation)?;
                    let w = self.wins.get_mut(&n).unwrap();
                    let oldimg = std::mem::replace(&mut w.img, img);
                    w.seg = Some(seg);
                    (w.w, w.h, w.generation) = (r.dx(), r.dy(), generation);
                    let _ = self.d.free(oldimg);
                    self.event(n, format!("r {} {} {}", r.dx(), r.dy(), generation));
                }
                self.wins.get_mut(&n).unwrap().r = r;
                self.redraw(old.inset(-BORDER).union(&r.inset(-BORDER)));
                Ok(())
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
        let pressed = b != 0 && self.buttons == 0;
        if pressed {
            let hit = self.z.iter().rev().copied().find(|n| self.wins[n].r.inset(-BORDER).contains(x, y));
            if let Some(n) = hit {
                if self.focus != Some(n) {
                    self.setfocus(Some(n));
                }
                self.raise(n);
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
    let mut srvname = format!("wsys.{user}");
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
        pid: std::process::id(),
    };
    let colors = (|| -> std::io::Result<(u32, u32, u32)> {
        Ok((ws.d.color(BG)?, ws.d.color(FOCUSED)?, ws.d.color(UNFOCUSED)?))
    })();
    match colors {
        Ok((a, b, c)) => (ws.bg, ws.cfocus, ws.cother) = (a, b, c),
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
    // the windows' segments go with them
    let ids: Vec<u32> = ws.wins.keys().copied().collect();
    for n in ids {
        ws.delwin(n);
    }
}
