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
//	/mnt/wsys/screen	read: an image(6) of where wsys draws (all of the
//				screen, or inside its rio window), as rio's
//	/mnt/wsys/wctl		write: new [options] [cmd] (rio(4))
//	/mnt/wsys/clone		open: a new window; the fid is then its ctl
//	/mnt/wsys/N/		a window: ctl, event (wsys' own), and rio(4)'s
//				cons consctl cursor kbd label mouse screen snarf
//				text wctl wdir window winid winname
//	N/ctl			read: id x0 y0 x1 y1 w h base size focus
//				write: rect x0 y0 x1 y1 | buffer n seg w h stride [off] |
//				unbuffer n | present n [x0 y0 x1 y1] | top |
//				label text | delete
//	N/event			read (blocks): lines m x y buttons msec | k hex-rune |
//				r w h | f 0|1 | b n
//
// A window is a window (layer) of draw(3) on wsys's screen, border and all,
// named as winname gives: a draw(3) program draws into it itself, as in rio.
// A client of wsys's own protocol keeps its pixels in global segments of
// segment(3) it makes, in the range of addresses ctl gives each window (wsys
// attaches them at the same address, so they cannot collide in it) or where
// the kernel put them (segment(3)'s "va 0 len": unique anyway); buffer n
// announces one (x8r8g8b8, w x h, stride bytes a line, off bytes into the
// segment; several buffers may share one); present n copies a
// rectangle of it into the window and answers with the event b n: buffer n
// is free again (a swap chain of the client's).
//
// Input: /dev/mouse and /dev/kbd (else /dev/cons, raw) to the focused window:
// its mouse file gets the mouse while it is over the window or a button is
// held (r first after a change of the window), its kbd file the keyboard's
// messages when open, else cons the runes (a line at a time unless raw; Del
// interrupts the window's note group). A button pressed on another window
// gives it the focus and raises it; on a border, as in rio, button 1 (or 2)
// sweeps it to a new size (the corner within 20 pixels of it, else the edge; an
// outline until the button comes up), button 3 moves the window.
// A window made by new is a text window, as rio's: while no program has its
// mouse open, wsys shows the end of what was written to its cons (and text) and
// the line being typed, in a font of font(6); the wheel scrolls it. Button 1
// selects text; button 2's menu cuts, pastes, snarfs (the snarf buffer is
// /dev/snarf's, rio's when wsys runs in rio) and sends. Button 3's menu, on the
// background or a text window, as rio's: New (sweep, a shell in it), Resize
// (sweep from the window), Move, Delete, Hide, and the hidden windows to bring
// back; a cross for the cursor while a command waits.
// A window lives while its files are open (or the clone fid that made it).
// Processes wsys starts (-i, wctl's new) get note groups of their own.
//
// usage: wsys [-gpOx] [-f font] [-s srvname] [-i cmd]
//	-g	the GPU composes the screen (the default, where it can): why not if it cannot,
//		and what it does
//	-p	the processor composes the screen
//	-f	the font of text windows (default $font, else /lib/font/bit/vga/unicode.font)
//	-s	post as /srv/srvname (default wsys.$user.$pid, and wsys.$user: the newest
//		wsys of the user, where programs that outlive one find the next)
//	-i	run cmd (rc) in a name space with /mnt/wsys mounted, $wsys set
//	-x	exit when cmd exits
//	-O	no overlay plane: every window's pixels in wsys' screen (tests read them back)

#[cfg(feature = "gpu")]
mod comp;
// without the feature (no Vulkan linked) -g says so
#[cfg(not(feature = "gpu"))]
mod comp {
    pub type Key = (u32, u32);
    #[derive(Clone, PartialEq)]
    pub struct Shown {
        pub key: Key,
        pub at: (i32, i32),
        pub rects: Vec<crate::draw::Rect>,
    }
    pub enum Done {
        Composed(Vec<Key>),
        Failed(String),
    }
    pub struct Comp;
    impl Comp {
        pub fn start(_: Box<dyn Fn(Done) + Send>, _: bool) -> Result<Comp, String> {
            Err("built without the GPU (feature gpu)".into())
        }
        pub fn import(&self, _: Key, _: &str, _: i32, _: i32, _: usize) -> Result<(), String> {
            Err("no GPU".into())
        }
        pub fn forget(&self, _: Key) {}
        pub fn scene(&self, _: Vec<Shown>) -> bool {
            false
        }
        pub fn snapshot(&self, _: crate::draw::Rect) -> Result<Vec<u8>, String> {
            Err("no GPU".into())
        }
    }
}
mod draw;
mod font;
mod gpu;
mod ninep;
mod p9;

use draw::{Display, Rect, XRGB32};
use ninep::{Dir, Qid, R, T};
use p9::{Fd, OREAD, ORDWR, OWRITE, Seg};
use std::collections::{HashMap, VecDeque};
use std::rc::{Rc, Weak};
use std::io::Write;
use std::sync::mpsc;
use std::thread;

const BORDER: i32 = 4;
const BG: u32 = 0x777777;
const FOCUSED: u32 = 0x335588;
const UNFOCUSED: u32 = 0xAAAAAA;
// the windows' ranges for their clients' segments: slot i is SEGBASE + i*SEGSIZE
// (p9::window_region; slots are given again when free)
const SEGBASE: usize = p9::window_region().0;
const SEGSIZE: usize = p9::window_region().1;
const NSLOT: usize = p9::window_region().2;
const MAXBUF: u32 = 8;
const WHITE: u32 = 0xFFFFFF;
const MINSIZE: i32 = 16; // a window swept smaller stays this big
const CORNER: i32 = 20; // on the border this near a corner grabs both its edges
const MAXEVENTS: usize = 512;
const MAXMOUSE: usize = 32;
const MAXTEXT: usize = 1 << 20;
const TEXTSHOWN: usize = 64 << 10; // of a text window's text, laid out to show its end
// a text window as rio's: the scroll bar inside the border, then a gap, then the text
const SCROLLWID: i32 = 12;
const SCROLLGAP: i32 = 4;
const SCROLLBG: u32 = 0x999999; // the scroll bar; the part shown white
const DEFFONT: &str = "/lib/font/bit/vga/unicode.font";
const PALEYELLOW: u32 = 0xFFFFAA; // a selection, as acme's and rio's
const PALEBLUEGREEN: u32 = 0x9EEEEE; // a menu's item under the mouse

enum Ev {
    Tick, // every 50 ms: a scroll button held repeats
    Msg(Vec<u8>),
    Hangup,
    Mouse { x: i32, y: i32, b: i32, ms: u32 },
    Kbd(Vec<Vec<u8>>), // kbd(3) messages: k, K or c and runes
    ChildExit(String),
    WinExit(u32), // the process wsys started in a window
    // the overlay's thread did a request: a present (its write's tag, unless off) or off
    Overlay { win: u32, b: u32, tag: Option<u16>, count: u32, err: Option<String> },
    // the GPU's thread (-g): a frame on its way, or it is gone
    Comp(comp::Done),
}

/// a request for the overlay's thread: its own fid of wsys' GPU context, as the
/// display's update waits for the vblank (wsys goes on meanwhile)
struct OvJob {
    win: u32,
    b: u32,
    tag: Option<u16>, // a present's write, answered when it is done; None: off
    count: u32,
    line: String,
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
    Window,
    Screen,
}

const WINFILES: [(F, &str, u32); 16] = [
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
    (F::Window, "window", 0o444),
    (F::Screen, "screen", 0o444),
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
    snap: Option<Vec<u8>>, // screen, window: the image (6) as it was at the first read
}

/// A client's buffer. Only the main loop touches it: Rust's std on Plan 9 runs
/// threads as procs, and a segment attached after they started is the attaching
/// proc's alone. Each is a segment of wsys's: with the 64 stack segments std
/// makes, some 190 fit (NSEG).
struct Buf {
    mem: Mem,
    w: i32,
    h: i32,
    stride: usize,
    comp: bool, // imported by the GPU's thread too (-g): it composes it
}

/// where a buffer's pixels are: a segment of the client's, or a GPU buffer it
/// exported (gpu(4)), imported by wsys
enum Mem {
    Seg(Rc<Seg>, usize), // and where in it the buffer begins
    Gpu(gpu::GpuMem),
}

impl Mem {
    /// rows y0 to y1 of the pixels, stride bytes a row, and the row the slice
    /// begins with: a GPU buffer's are read once into wsys's memory first (the
    /// device's memory can be slow to read twice, write-combined)
    fn rows(&mut self, y0: i32, y1: i32, stride: usize) -> (&[u8], i32) {
        match self {
            Mem::Seg(s, off) => (&s.bytes()[*off..], 0),
            Mem::Gpu(g) => (g.read(y0 as usize * stride, (y1 - y0) as usize * stride), y0),
        }
    }
}

struct Win {
    r: Rect, // content, in screen coordinates
    slot: usize,
    bufs: HashMap<u32, Buf>,
    img: u32,   // what a client of buffers presented, x8r8g8b8, the window's size
    stale: bool, // presented into the layer since: img is behind it (freshen)
    layer: u32, // its window on wsys's screen in draw(3), border and all
    name: String, // the layer's public name (winname)
    refs: u32,    // fids holding the window
    pinned: bool, // made by wctl's new: kept until its process attached or ended
    pid: Option<u32>, // the note group for interrupt and hangup
    label: String,
    wdir: String,
    text: String,     // what was written to cons and text, and the lines typed
    partial: Vec<u8>, // the start of a rune whose rest is in the next write
    events: VecDeque<String>,
    // the files of rio(4)
    raw: bool,
    line: String,        // cons, cooked: the line being typed
    cons: VecDeque<u8>,  // cons: what reads get
    eof: bool,           // cons: ^D on an empty line, a read of 0
    kbd: VecDeque<Vec<u8>>, // kbd: messages, each with its null byte
    kbdopen: u32,
    mouseopen: u32,
    mouse: VecDeque<(i32, i32, i32, u32)>,
    resized: bool, // the next mouse read is an r message
    cursor: Vec<u8>, // cursor(3)'s 72 bytes, or none: the default arrow
    state: u64,      // bumped when wctl's answer changes
    textwin: bool,   // made by new: shows its text while no program has its mouse
    scroll: usize,   // a text window: lines back from the end
    scrolling: bool, // output scrolls to the end even when looking back (button 2's scroll, as rio's)
    scrollr: Rect,   // its scroll bar
    shown: (usize, usize, usize), // lines laid out, the first shown, how many fit
    sel: (usize, usize), // the selection: byte offsets in text, then line
    rows: Vec<Row>,      // where the runes shown are
    hold: bool,          // rio's hold mode: lines typed wait (Esc toggles it)
    want: Option<usize>, // drawtext scrolls so that this offset is shown (look)
    hidden: bool,        // off the screen (button 3's Hide), listed in its menu
    lastb: Option<u32>,  // the buffer presented last (the overlay's: copied when it goes off)
    inside: bool,        // the mouse was over it, last seen
    nooverlay: bool,     // the driver refused it the overlay: copies until it moves or resizes
    ovsent: Option<bool>, // the last "overlay 0|1" event: whether it may go on the plane
    // -g: the buffer presented last, composed by the GPU, and the one a frame on
    // its way shows (held: the client gets it back when another is shown)
    shownb: Option<u32>,
    held: Option<u32>,
    // the GPU buffers presented since the one held, oldest first (shownb last): a
    // frame with one of them on the display gives back the ones before it
    queued: Vec<u32>,
    // on the overlay plane: the buffer shown there (variant b: the display reads
    // the client's own), back to the client when another one is shown
    ovheld: Option<u32>,
}

/// a line of a text window as drawn: each rune's x, width and offset, and the
/// offset after the last
struct Row {
    y: i32,
    runes: Vec<(i32, i32, usize)>,
    end: usize,
}

/// a menu while its button is held
struct Menu {
    win: Option<u32>, // button 2's: the text window
    button: i32,
    items: Vec<String>,
    hidden: Vec<u32>, // button 3's: the hidden windows after its commands
    r: Rect,
    layer: u32,
    cur: Option<usize>,
}

const MENU2: &[&str] = &["cut", "paste", "snarf", "plumb", "look", "send"];
const HOLDCOL: u32 = 0x000099; // rio's DMedblue: text and border while holding
const PALEHOLDCOL: u32 = 0x4993DD; // rio's DPalegreyblue: the border of one not focused
const MENU3: &[&str] = &["New", "Resize", "Move", "Delete", "Hide"];

/// a command of button 3's menu, waiting for button 3 on the screen
#[derive(Clone, Copy, PartialEq, Debug)]
enum Op3 {
    New,
    Resize,
    Move,
    Delete,
    Hide,
}

/// what of r is not in o: up to four rectangles (above, below, left, right of it)
fn subtract(r: &Rect, o: &Rect) -> Vec<Rect> {
    let i = r.clip(o);
    if i.empty() {
        return vec![*r];
    }
    [
        Rect::new(r.x0, r.y0, r.x1, i.y0),
        Rect::new(r.x0, i.y1, r.x1, r.y1),
        Rect::new(r.x0, i.y0, i.x0, i.y1),
        Rect::new(i.x1, i.y0, r.x1, i.y1),
    ]
    .into_iter()
    .filter(|x| !x.empty())
    .collect()
}

/// cursor(3) while a command waits: a cross (offset, clr, set: 16x16 bits)
fn crosscursor() -> Vec<u8> {
    let mut c = vec![];
    c.extend_from_slice(&(-7i32).to_le_bytes());
    c.extend_from_slice(&(-7i32).to_le_bytes());
    let (mut clr, mut set) = ([0u8; 32], [0u8; 32]);
    for row in 0..16usize {
        // a cross two pixels thick through 7 and 8, white around it
        let (s, w): (u16, u16) = if row == 7 || row == 8 {
            (0x7FFE, 0xFFFF)
        } else if row >= 1 && row <= 14 {
            (0x0180, 0x03C0)
        } else {
            (0, 0x03C0)
        };
        set[2 * row] = (s >> 8) as u8;
        set[2 * row + 1] = s as u8;
        clr[2 * row] = (w >> 8) as u8;
        clr[2 * row + 1] = w as u8;
    }
    c.extend_from_slice(&clr);
    c.extend_from_slice(&set);
    c
}

#[derive(Clone, Copy)]
enum DragKind {
    Move,
    Sweep { left: bool, right: bool, top: bool, bottom: bool },
    Box(Option<u32>), // a rectangle swept from the press: a new window's, or a window's new frame
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
    font: Option<font::Font>, // of the text windows
    black: u32,
    pale: u32,    // a selection's background
    menuhi: u32,  // a menu's item under the mouse
    selecting: Option<(u32, usize)>, // button 1 sweeping a selection: window, where it began
    menu: Option<Menu>,
    parentsnarf: bool, // in rio: its /dev/snarf is the snarf buffer
    op3: Option<Op3>,  // button 3's command waiting for its click
    tabstop: i32,      // $tabstop, as rio: a tab is this many zeros wide
    scrollbg: u32,
    scrolling: Option<(u32, i32, std::time::Instant)>, // window, button held in its scroll bar, next repeat
    click: Option<(u32, i32, i32, u32)>, // the last button 1 click in a text window: window, x, y, msec
    holdcol: u32,
    paleholdcol: u32,
    plumbfd: Option<Fd>, // /mnt/plumb/send, opened when first plumbed
    ovtx: Option<mpsc::Sender<OvJob>>, // the overlay's thread, started for the first
    ov: Option<u32>,                   // the window on the overlay plane
    ovtags: Vec<u16>,                  // presents waiting for the overlay's thread
    ovwhy: Option<&'static str>,       // why the last present did not go there (WSYSDEBUG)
    nooverlay: bool,                   // -O
    gpu: Option<gpu::Gpu>, // a context for clients' GPU buffers, made for the first
    segs: HashMap<usize, Weak<Seg>>, // the clients' segments attached, by address
    comp: Option<comp::Comp>, // -g: the GPU composes the screen
    compscene: Vec<comp::Shown>, // what it was told last
    compdirty: bool,             // a buffer presented since: a frame
    compgone: Vec<(u32, u32)>,   // buffers out of the scene: back when the next frame is composed
    sweepr: Option<Rect>,        // the sweep's outline (over the GPU windows)
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
        self.fids.insert(fid, FidState { kind, open: false, dirpos: 0, win, owns: false, raw: false, seen: 0, snap: None });
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
                    Some("new") => match opts(&words[1..], false).and_then(|o| self.newwin(&o, true)) {
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
                self.ovtags.retain(|&t| t != oldtag); // no answer for it any more
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
                    Kind::File(_, F::Event | F::Winid | F::Winname | F::Window | F::Screen) if writing => return e("permission denied"),
                    Kind::File(_, F::Consctl) if reading => return e("permission denied"),
                    Kind::Clone => match self.newwin(&Opts::default(), false) {
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
                    Kind::Screen | Kind::File(_, F::Screen | F::Window) => {
                        // an image(6) of the screen or of the window (border and
                        // all), taken at the read at offset 0
                        if offset == 0 || f.snap.is_none() {
                            let (id, r) = match k {
                                Kind::File(n, F::Window) => (self.wins[&n].layer, self.wins[&n].r.inset(-BORDER)),
                                _ => (self.d.screen, self.d.r),
                            };
                            // -g: the screen as the GPU composed it (the GPU windows are
                            // not in devdraw's); a window's is what is seen of it
                            let gpuwin = matches!(k, Kind::File(n, F::Window) if self.wins[&n].shownb.is_some());
                            let snap = if self.comp.is_some() && (gpuwin || !matches!(k, Kind::File(_, F::Window))) {
                                self.compsnapshot(r)
                            } else {
                                self.snapshot(id, r)
                            };
                            match snap {
                                Ok(b) => self.fids.get_mut(&fid).unwrap().snap = Some(b),
                                Err(err) => return e(&err.to_string()),
                            }
                        }
                        let snap = self.fids[&fid].snap.as_ref().unwrap();
                        let o = (offset as usize).min(snap.len());
                        let b = &snap[o..(o + count as usize).min(snap.len())];
                        Some(R::new(117, tag).u32(b.len() as u32).bytes(b).done())
                    }
                    Kind::File(n, F::Ctl) => text(self.ctltext(n).as_bytes()),
                    Kind::File(n, F::Label) => text(self.wins[&n].label.as_bytes()),
                    Kind::File(n, F::Wdir) => text(self.wins[&n].wdir.as_bytes()),
                    Kind::File(n, F::Text) => text(self.wins[&n].text.as_bytes()),
                    Kind::File(n, F::Cursor) => text(&self.wins[&n].cursor),
                    Kind::File(n, F::Winname) => text(self.wins[&n].name.as_bytes()),
                    Kind::File(n, F::Winid) => text(format!("{n:11} ").as_bytes()),
                    Kind::File(_, F::Snarf) => {
                        if offset == 0 {
                            self.getsnarf();
                        }
                        text(&self.snarf)
                    }
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
                        let lines: Vec<&str> = s.lines().map(str::trim).filter(|l| !l.is_empty()).collect();
                        if let [line] = lines[..] {
                            if self.overlay_present(n, line, tag, data.len() as u32) {
                                return None; // answered when the overlay's thread is done
                            }
                        }
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
                        self.output(n, &data);
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
                                "holdon" | "holdoff" => self.sethold(n, w == "holdon"),
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
                        self.putsnarf();
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
                if matches!(f.kind, Kind::File(_, F::Mouse)) {
                    // the program let go of the window: its text again
                    self.paint(n, None);
                    self.drawtext(n);
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
            w.cons.extend(l.as_bytes());
            self.wake(n);
        }
    }

    /// image(6), uncompressed: rectangle r of image id (the screen's channels)
    fn snapshot(&mut self, id: u32, r: Rect) -> std::io::Result<Vec<u8>> {
        let chan = self.d.chan;
        let mut b = format!("{:>11} {:11} {:11} {:11} {:11} ", draw::chanstr(chan), r.x0, r.y0, r.x1, r.y1).into_bytes();
        b.extend(self.d.unload(id, &r, draw::chandepth(chan))?);
        self.ovpaste(&mut b, r);
        Ok(b)
    }

    /// the window on the overlay plane into snapshot b of rectangle r: the display
    /// shows its buffer over the screen, devdraw's screen has not got it.  A buffer
    /// the processor cannot see (in VRAM for the plane, variant b) is copied where
    /// it can first (gpu(4) readback)
    fn ovpaste(&mut self, b: &mut [u8], r: Rect) {
        let Some(n) = self.ov else { return };
        let Some(w) = self.wins.get(&n) else { return };
        let Some(buf) = w.ovheld.and_then(|h| w.bufs.get(&h)) else { return };
        let Mem::Gpu(g) = &buf.mem else { return };
        let i = r.clip(&w.r);
        if self.d.chan != XRGB32 || i.empty() {
            return;
        }
        let (o, stride) = ((w.r.x0, w.r.y0), buf.stride);
        let mut copy = None;
        if !g.mapped() {
            match self.gpu.as_ref().map(|gpu| gpu.readback(g)) {
                Some(Ok(c)) => copy = Some(c),
                Some(Err(e)) => {
                    eprintln!("wsys: window {n}: the overlay's pixels: {e}");
                    return;
                }
                None => return,
            }
        }
        let n = self.wins.get_mut(&n).unwrap();
        let h = n.ovheld.unwrap();
        let Mem::Gpu(g) = &mut n.bufs.get_mut(&h).unwrap().mem else { return };
        let src = copy.as_mut().unwrap_or(g);
        let (y0, y1) = (i.y0 - o.1, i.y1 - o.1);
        let px = src.read(y0 as usize * stride, (y1 - y0) as usize * stride);
        let hdr = b.len() - (r.dx() * r.dy()) as usize * 4;
        let row = (i.dx() * 4) as usize;
        for y in i.y0..i.y1 {
            let s = (y - i.y0) as usize * stride + (i.x0 - o.0) as usize * 4;
            let d = hdr + (((y - r.y0) * r.dx() + (i.x0 - r.x0)) * 4) as usize;
            if s + row <= px.len() && d + row <= b.len() {
                b[d..d + row].copy_from_slice(&px[s..s + row]);
            }
        }
    }

    /// image(6) of rectangle r of the screen as the GPU composes it
    fn compsnapshot(&mut self, r: Rect) -> std::io::Result<Vec<u8>> {
        self.compsync();
        let px = self.comp.as_ref().unwrap().snapshot(r).map_err(std::io::Error::other)?;
        let r = r.clip(&self.d.r);
        let mut b = format!("{:>11} {:11} {:11} {:11} {:11} ", draw::chanstr(XRGB32), r.x0, r.y0, r.x1, r.y1).into_bytes();
        b.extend(px);
        self.ovpaste(&mut b, r);
        Ok(b)
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
                let vis = if w.hidden { "hidden" } else { "visible" };
                let s = format!("{:11} {:11} {:11} {:11} {:>11} {:>11} ", f.x0, f.y0, f.x1, f.y1, cur, vis);
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
        // WSYSDEBUG=release: each buffer given back
        if line.starts_with("b ") && std::env::var("WSYSDEBUG").is_ok_and(|v| v.contains("release")) {
            eprintln!("wsys: window {n}: {line}");
        }
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

    /// a new window, where the options put its frame (else by itself); text: a
    /// text window (made by new, not by clone)
    fn newwin(&mut self, o: &Opts, text: bool) -> Result<u32, String> {
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
            label: String::new(), wdir: o.cd.clone().unwrap_or_default(), text: String::new(), partial: vec![],
            events: VecDeque::new(), raw: false, line: String::new(), cons: VecDeque::new(), eof: false,
            kbd: VecDeque::new(), kbdopen: 0, mouseopen: 0, mouse: VecDeque::new(), resized: false,
            cursor: vec![], state: 1, textwin: text, scroll: 0, scrolling: false, sel: (0, 0), rows: vec![], hidden: false,
            hold: false, want: None, stale: false,
            scrollr: Rect::default(), shown: (1, 0, 1), lastb: None, inside: false, nooverlay: false, ovsent: None, shownb: None, held: None, queued: vec![], ovheld: None,
        });
        self.z.push(n);
        self.setfocus(Some(n));
        self.paint(n, None);
        self.drawtext(n);
        Ok(n)
    }

    /// window n goes; hangup: its note group gets the note (a delete by wctl)
    fn delwin(&mut self, n: u32, hangup: bool) {
        let Some(w) = self.wins.remove(&n) else { return };
        if let Some(c) = &self.comp {
            for (&b, buf) in &w.bufs {
                if buf.comp {
                    c.forget((n, b));
                }
            }
        }
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
        if self.selecting.map(|s| s.0) == Some(n) {
            self.selecting = None;
        }
        if self.menu.as_ref().map(|m| m.win) == Some(Some(n)) {
            let m = self.menu.take().unwrap();
            let _ = self.d.free(m.layer);
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
        let mut c = self.focus.and_then(|n| self.wins.get(&n)).map(|w| w.cursor.clone()).unwrap_or_default();
        if self.op3.is_some() || matches!(self.drag.map(|g| g.kind), Some(DragKind::Box(_))) {
            c = crosscursor();
        }
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
            "hide" => {
                self.hide(n, true);
                Ok(())
            }
            "unhide" => {
                self.hide(n, false);
                Ok(())
            }
            "scroll" | "noscroll" => {
                self.setscrolling(n, cmd == "scroll");
                Ok(())
            }
            _ => Err(format!("unknown wctl message: {cmd}")),
        }
    }

    /// wctl's new: a window, and cmd (default rc -i) run in it in a name space
    /// of its own with the window's files on /dev and /mnt/wsys
    fn spawn(&mut self, o: Opts) -> Result<(), String> {
        let n = self.newwin(&o, true)?;
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
                let off = match a.get(6) {
                    None => 0,
                    Some(x) => x.parse::<usize>().map_err(|_| "bad buffer offset")?,
                };
                let at = Seg::find(name).map_err(|e| format!("buffer {b}: {e}"))?;
                let w = self.wins.get_mut(&n).unwrap();
                let base = SEGBASE + w.slot * SEGSIZE;
                let (rbase, rtop) = p9::shared_region();
                let (va, len) = at;
                let inwin = va >= base && va + len <= base + SEGSIZE;
                let inregion = va >= rbase && va + len <= rtop;
                if !inwin && !inregion {
                    return Err(format!("buffer {b}: segment {name} at {va:#x} is outside the window's range"));
                }
                if off.checked_add((stride * (bh - 1) + bw * 4) as usize).is_none_or(|e| e > len) {
                    return Err(format!("buffer {b}: segment {name} too small"));
                }
                // a segment attached once, however many buffers are in it (a
                // Wayland client's pool): by its address, unique while attached
                let seg = match self.segs.get(&va).and_then(Weak::upgrade) {
                    Some(s) => s,
                    None => {
                        let s = Rc::new(Seg::attach(name, at).map_err(|e| format!("buffer {b}: {e}"))?);
                        self.segs.retain(|_, s| s.strong_count() > 0);
                        self.segs.insert(va, Rc::downgrade(&s));
                        s
                    }
                };
                let w = self.wins.get_mut(&n).unwrap();
                w.bufs.insert(b as u32, Buf { mem: Mem::Seg(seg, off), w: bw as i32, h: bh as i32, stride: stride as usize, comp: false });
                Ok(())
            }
            "gpubuffer" => {
                let a: Vec<&str> = line.split_whitespace().collect();
                let num = |i: usize| a.get(i).and_then(|x| x.parse::<i64>().ok());
                let (Some(b), Some(token), Some(bw), Some(bh), Some(stride)) = (num(1), a.get(2), num(3), num(4), num(5)) else {
                    return Err("usage: gpubuffer n token w h stride".into());
                };
                if !(0..MAXBUF as i64).contains(&b) || bw < 1 || bh < 1 || bw > 16384 || bh > 16384 || stride < bw * 4 {
                    return Err("bad buffer".into());
                }
                if self.gpu.is_none() {
                    self.gpu = Some(gpu::Gpu::open().map_err(|e| format!("gpubuffer {b}: {e}"))?);
                }
                let size = (stride * (bh - 1) + bw * 4) as usize;
                let mem = self.gpu.as_mut().unwrap().import(token, size).map_err(|e| format!("gpubuffer {b}: {e}"))?;
                self.unbuffer(n, b as u32);
                // -g: the GPU's thread imports it too and composes it; else (or if it
                // cannot) wsys copies it with the processor
                let comp = match &self.comp {
                    Some(c) => c
                        .import((n, b as u32), token, bw as i32, bh as i32, stride as usize)
                        .inspect_err(|e| eprintln!("wsys: window {n}: gpubuffer {b}: the GPU composes it not: {e}"))
                        .is_ok(),
                    None => false,
                };
                let w = self.wins.get_mut(&n).unwrap();
                w.bufs.insert(b as u32, Buf { mem: Mem::Gpu(mem), w: bw as i32, h: bh as i32, stride: stride as usize, comp });
                Ok(())
            }
            "unbuffer" => {
                let b = *nums.first().ok_or("usage: unbuffer n")? as u32;
                if !self.wins[&n].bufs.contains_key(&b) {
                    return Err("no such buffer".into());
                }
                self.unbuffer(n, b);
                Ok(())
            }
            "present" => {
                let b = *nums.first().ok_or("usage: present n [x0 y0 x1 y1]")? as u32;
                let r = if nums.len() == 5 { Some(Rect::new(nums[1], nums[2], nums[3], nums[4])) } else { None };
                if self.compshow(n, b) {
                    return Ok(());
                }
                self.uncompose(n);
                self.copybuf(n, b, r)?;
                self.wins.get_mut(&n).unwrap().lastb = Some(b);
                // copied: shown, and the client may write it again
                self.event(n, format!("s {b}"));
                self.event(n, format!("b {b}"));
                if self.ov != Some(n) {
                    self.ovletgo(n); // off the plane (ovcheck turned it off)
                }
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

    /// window n's buffer b composed by the GPU from now on (the next frame), if it
    /// can be; the client gets back the ones shown or presented before when that
    /// frame is on the display
    fn compshow(&mut self, n: u32, b: u32) -> bool {
        let Some(w) = self.wins.get_mut(&n) else { return false };
        if self.comp.is_none() || !w.bufs.get(&b).is_some_and(|x| x.comp) {
            return false;
        }
        if w.held == Some(b) {
            w.held = None; // presented again while shown: shown again
        }
        w.queued.retain(|&q| q != b);
        w.queued.push(b);
        w.shownb = Some(b);
        w.lastb = Some(b);
        self.compdirty = true;
        true
    }

    /// buffer b of window n gone (unbuffer, a new one of the same number)
    fn unbuffer(&mut self, n: u32, b: u32) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        let Some(buf) = w.bufs.remove(&b) else { return };
        if w.ovheld == Some(b) {
            w.ovheld = None;
        }
        if buf.comp {
            if w.shownb == Some(b) {
                w.shownb = None;
                self.compdirty = true;
            }
            if w.held == Some(b) {
                w.held = None;
            }
            w.queued.retain(|&q| q != b);
            self.compgone.retain(|&k| k != (n, b));
            if let Some(c) = &self.comp {
                c.forget((n, b));
            }
        }
    }

    /// window n no longer composed by the GPU (its client presents a buffer wsys
    /// copies): the one held goes back to it
    fn uncompose(&mut self, n: u32) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        if w.shownb.take().is_some() {
            self.compdirty = true;
        }
        // a frame on its way may still read them: back with the next one
        let gone: Vec<u32> = w.held.take().into_iter().chain(w.queued.drain(..)).collect();
        if !gone.is_empty() {
            self.compdirty = true;
            self.compgone.extend(gone.into_iter().map(|b| (n, b)));
        }
    }

    /// what the GPU composes now (-g): the windows with a GPU buffer presented,
    /// bottom to top, each where it is not covered by a window above it or by
    /// wsys' menu and sweep; told to the GPU's thread when it changed or a
    /// buffer was presented
    fn compsync(&mut self) {
        if self.comp.is_none() {
            return;
        }
        let mut scene = vec![];
        for (i, &n) in self.z.iter().enumerate() {
            let w = &self.wins[&n];
            let Some(b) = w.shownb else { continue };
            let Some(buf) = w.bufs.get(&b) else { continue };
            if self.ov == Some(n) {
                continue; // on the overlay plane, over the frames
            }
            if w.hidden {
                // nothing of it seen, but its frames go back at the display's rate
                // (else its client would wait for ever)
                scene.push(comp::Shown { key: (n, b), at: (w.r.x0, w.r.y0), rects: vec![] });
                continue;
            }
            let mine = Rect::new(w.r.x0, w.r.y0, w.r.x0 + buf.w.min(w.r.dx()), w.r.y0 + buf.h.min(w.r.dy())).clip(&self.d.r);
            let mut rects = if mine.empty() { vec![] } else { vec![mine] };
            let mut over: Vec<Rect> = self.z[i + 1..].iter().map(|o| &self.wins[o]).filter(|o| !o.hidden).map(|o| o.r.inset(-BORDER)).collect();
            if let Some(m) = &self.menu {
                over.push(m.r);
            }
            if let Some(r) = self.sweepr {
                let f = r.inset(-BORDER);
                over.extend([
                    Rect::new(f.x0, f.y0, f.x1, r.y0),
                    Rect::new(f.x0, r.y1, f.x1, f.y1),
                    Rect::new(f.x0, r.y0, r.x0, r.y1),
                    Rect::new(r.x1, r.y0, f.x1, r.y1),
                ]);
            }
            for o in over {
                rects = rects.iter().flat_map(|r| subtract(r, &o)).collect();
            }
            scene.push(comp::Shown { key: (n, b), at: (w.r.x0, w.r.y0), rects });
        }
        if scene == self.compscene && !self.compdirty {
            return;
        }
        self.compdirty = false;
        self.compscene = scene.clone();
        if !self.comp.as_ref().unwrap().scene(scene) {
            self.compfailed("its thread is gone".into());
        }
    }

    /// a frame with these buffers is on its way: each window's buffer shown
    /// before goes back to its client
    fn composed(&mut self, keys: Vec<comp::Key>) {
        if std::env::var("WSYSDEBUG").is_ok_and(|v| v.contains("release")) {
            eprintln!("wsys: composed {keys:?}");
        }
        for (n, b) in std::mem::take(&mut self.compgone) {
            self.event(n, format!("b {b}"));
        }
        for (n, b) in keys {
            let Some(w) = self.wins.get_mut(&n) else { continue };
            let Some(i) = w.queued.iter().position(|&q| q == b) else { continue };
            // b is on the display: the one shown before and those presented before
            // b (no frame showed them) go back
            let mut back: Vec<u32> = w.queued.drain(..=i).filter(|&q| q != b).collect();
            if let Some(h) = w.held.replace(b) {
                if h != b {
                    back.push(h);
                }
            }
            // on the display: the time for the client's next picture (Wayland's frame
            // callbacks, wlsys), while wsys keeps b until another one is shown
            self.event(n, format!("s {b}"));
            for q in back {
                self.event(n, format!("b {q}"));
            }
        }
    }

    /// the GPU's thread is gone: the processor copies the buffers again
    fn compfailed(&mut self, e: String) {
        eprintln!("wsys: the GPU composes no longer: {e}");
        self.comp = None;
        self.compscene.clear();
        let ids: Vec<u32> = self.wins.keys().copied().collect();
        for n in ids {
            let w = self.wins.get_mut(&n).unwrap();
            for b in w.bufs.values_mut() {
                b.comp = false;
            }
            let shown = w.shownb.take();
            let mut back: Vec<u32> = w.held.take().into_iter().chain(w.queued.drain(..)).collect();
            if let Some(b) = shown {
                let _ = self.copybuf(n, b, None);
                if !back.contains(&b) {
                    back.push(b);
                }
            }
            for b in back {
                self.event(n, format!("b {b}"));
            }
        }
        for (n, b) in std::mem::take(&mut self.compgone) {
            self.event(n, format!("b {b}"));
        }
    }

    /// move window n to r; a new size gives it a new window in draw(3) that keeps
    /// what it showed (white beyond it), and its client an r event; a draw(3)
    /// program reads r from its mouse file either way
    fn setrect(&mut self, n: u32, r: Rect) -> Result<(), String> {
        if let Some(w) = self.wins.get_mut(&n) {
            w.nooverlay = false; // somewhere else: the overlay again, perhaps
        }
        if r.dx() < 1 || r.dy() < 1 || r.dx() > 16384 || r.dy() > 16384 {
            return Err("bad rectangle".into());
        }
        let old = self.wins[&n].r;
        if r == old {
            return Ok(());
        }
        self.freshen(n);
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
            self.drawtext(n);
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
        self.sweepr = r;
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
        let col = match (w.hold, self.focus == Some(n)) {
            (true, true) => self.holdcol,
            (true, false) => self.paleholdcol,
            (false, true) => self.cfocus,
            (false, false) => self.cother,
        };
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

    /// buffer b of window n (all, or its rectangle r) into what the window shows
    fn copybuf(&mut self, n: u32, b: u32, r: Option<Rect>) -> Result<(), String> {
        let w = &self.wins[&n];
        let buf = w.bufs.get(&b).ok_or("no such buffer")?;
        if let Mem::Gpu(g) = &buf.mem {
            if !g.mapped() {
                // in VRAM for the overlay plane, which the window may not have now: the
                // client makes buffers wsys can read when it hears "overlay 0"
                return Ok(());
            }
        }
        // what of the buffer the window shows: its top left corner
        let full = Rect::new(0, 0, buf.w.min(w.r.dx()), buf.h.min(w.r.dy()));
        let r = match r {
            Some(r) => r.clip(&full),
            None => full,
        };
        if r.empty() {
            return Ok(());
        }
        // straight into its window in draw(3) (screen coordinates), img follows
        // when it is needed; but through img if the screen's pixels are not the
        // buffers'
        let direct = self.d.chan == XRGB32;
        if !direct {
            self.freshen(n);
        }
        let w = self.wins.get_mut(&n).unwrap();
        let (layer, img, o) = (w.layer, w.img, (w.r.x0, w.r.y0));
        let buf = w.bufs.get_mut(&b).unwrap();
        let stride = buf.stride;
        let (px, row) = buf.mem.rows(r.y0, r.y1, stride);
        let d = &mut self.d;
        let res = if direct {
            d.load_at(layer, &r.add(o.0, o.1), px, stride, r.x0, r.y0 - row).and_then(|_| d.flush())
        } else {
            d.load_at(img, &r, px, stride, r.x0, r.y0 - row)
        };
        res.map_err(|e| e.to_string())?;
        if direct {
            self.wins.get_mut(&n).unwrap().stale = true;
        } else {
            self.paint(n, Some(r));
        }
        Ok(())
    }

    /*
     * The overlay plane (gpu(4) overlay, aux/amdgpu): the window on top, wholly on
     * the screen, nothing of wsys' over it (menu, sweep), its GPU buffer as big as
     * the window: the display shows the buffer itself (the driver copies it into
     * VRAM on the GPU), wsys copies nothing.  The request waits for the display:
     * a thread of its own does it, the present's write is answered then.
     */
    fn overlayable(&self, n: u32, b: u32) -> bool {
        self.ovwhynot(n, b).is_none()
    }

    /// why window n's buffer b may not go on the overlay plane (None: it may)
    fn ovwhynot(&self, n: u32, b: u32) -> Option<&'static str> {
        if let Some(why) = self.ovwinwhynot(n) {
            return Some(why);
        }
        let w = &self.wins[&n];
        let Some(buf) = w.bufs.get(&b) else { return Some("no buffer") };
        let why = [
            (!matches!(buf.mem, Mem::Gpu(_)), "not a GPU buffer"),
            (buf.w != w.r.dx() || buf.h != w.r.dy(), "the buffer is not the window's size"),
            (buf.stride % 256 != 0, "its stride is not a multiple of 256"),
        ];
        why.iter().find(|(c, _)| *c).map(|(_, s)| *s)
    }

    /// why window n may not go on the overlay plane, whatever its buffers
    fn ovwinwhynot(&self, n: u32) -> Option<&'static str> {
        let Some(w) = self.wins.get(&n) else { return Some("no window") };
        let why = [
            (self.nooverlay, "-O"),
            (self.gpu.as_ref().and_then(|g| g.overlay_ctl()).is_none(), "the driver has no overlay"),
            (self.z.last() != Some(&n), "not on top"),
            (w.hidden, "hidden"),
            (w.nooverlay, "refused before"),
            (self.menu.is_some() || self.drag.is_some() || self.op3.is_some(), "wsys draws over it"),
            (!self.d.r.contains(w.r.x0, w.r.y0) || w.r.x1 > self.d.r.x1 || w.r.y1 > self.d.r.y1, "not wholly on the screen"),
        ];
        why.iter().find(|(c, _)| *c).map(|(_, s)| *s)
    }

    /// the overlay's thread, started for the first request
    fn ovthread(&mut self) -> Option<&mpsc::Sender<OvJob>> {
        if self.ovtx.is_none() {
            let path = self.gpu.as_ref()?.overlay_ctl()?;
            let fd = Fd::open(&path, ORDWR).inspect_err(|e| eprintln!("wsys: {path}: {e}")).ok()?;
            let (jtx, jrx) = mpsc::channel::<OvJob>();
            let tx = self.tx.clone();
            thread::spawn(move || {
                for j in jrx {
                    let err = fd.write1(j.line.as_bytes()).err().map(|e| e.to_string());
                    if tx.send(Ev::Overlay { win: j.win, b: j.b, tag: j.tag, count: j.count, err }).is_err() {
                        return;
                    }
                }
            });
            self.ovtx = Some(jtx);
        }
        self.ovtx.as_ref()
    }

    /// a present of window n to the overlay's thread, if it can go there
    fn overlay_present(&mut self, n: u32, line: &str, tag: u16, count: u32) -> bool {
        let mut f = line.split_whitespace();
        if f.next() != Some("present") {
            return false;
        }
        let (Some(b), None) = (f.next().and_then(|x| x.parse::<u32>().ok()), f.next()) else { return false };
        if let Some(why) = self.ovwhynot(n, b) {
            // WSYSDEBUG=overlay: why not, once for each change
            if std::env::var("WSYSDEBUG").is_ok_and(|v| v.contains("overlay")) && self.ovwhy != Some(why) {
                eprintln!("wsys: window {n}: no overlay: {why}");
            }
            self.ovwhy = Some(why);
            return false;
        }
        self.ovwhy = None;
        let w = &self.wins[&n];
        let buf = &w.bufs[&b];
        let Mem::Gpu(g) = &buf.mem else { return false };
        let req = format!("overlay {} {} {} {} {} {}", g.handle(), w.r.x0, w.r.y0, buf.w, buf.h, buf.stride);
        let Some(tx) = self.ovthread() else { return false };
        if tx.send(OvJob { win: n, b, tag: Some(tag), count, line: req }).is_err() {
            return false;
        }
        self.ovtags.push(tag);
        self.ov = Some(n);
        self.wins.get_mut(&n).unwrap().lastb = Some(b);
        // the GPU composing the screen: the plane shows the window over its frames,
        // which leave it out (the buffers it composed go back with the next one)
        self.uncompose(n);
        true
    }

    /// the overlay's thread did a request
    fn overlay_done(&mut self, win: u32, b: u32, tag: Option<u16>, count: u32, err: Option<String>) {
        let Some(tag) = tag else { return }; // off
        let answer = self.ovtags.contains(&tag);
        self.ovtags.retain(|&t| t != tag);
        let r = if !self.wins.contains_key(&win) {
            Err("window deleted".to_string())
        } else if let Some(e) = err {
            // refused (no output has it all, another's on the plane): copies from now on
            eprintln!("wsys: window {win}: overlay: {e}");
            self.wins.get_mut(&win).unwrap().nooverlay = true;
            if self.ov == Some(win) {
                self.ov = None;
            }
            // the GPU composes it, else the processor copies it
            let r = if self.compshow(win, b) {
                Ok(())
            } else {
                let r = self.copybuf(win, b, None);
                if r.is_ok() {
                    self.event(win, format!("s {b}"));
                    self.event(win, format!("b {b}"));
                }
                r
            };
            self.ovletgo(win);
            r
        } else if self.ov != Some(win) {
            // shown, but off the plane meanwhile (ovcheck): composed by the GPU now,
            // or the client's again
            let w = &self.wins[&win];
            if w.shownb != Some(b) && w.held != Some(b) && !w.queued.contains(&b) {
                self.event(win, format!("b {b}"));
            }
            Ok(())
        } else {
            // on the plane: the client's again when the next one is shown there
            self.event(win, format!("s {b}"));
            let w = self.wins.get_mut(&win).unwrap();
            match w.ovheld.replace(b) {
                Some(h) if h != b => self.event(win, format!("b {h}")),
                _ => {}
            }
            Ok(())
        };
        if answer {
            self.send(match r {
                Ok(()) => R::new(119, tag).u32(count).done(),
                Err(e) => ninep::error(tag, &e),
            });
        }
    }

    /// the buffer window n had on the overlay plane back to its client (the
    /// plane shows it no longer)
    fn ovletgo(&mut self, n: u32) {
        if let Some(h) = self.wins.get_mut(&n).and_then(|w| w.ovheld.take()) {
            self.event(n, format!("b {h}"));
        }
    }

    /// the window on the overlay plane may not stay there: off, and what it showed
    /// last into its window (the overlay was over it)
    fn ovcheck(&mut self) {
        // to each window with GPU buffers: whether it may go on the plane now (Mesa's
        // WSI makes buffers in VRAM for it, else ones wsys reads: variant b)
        let gpuwins: Vec<u32> = self.wins.iter()
            .filter(|(_, w)| w.bufs.values().any(|b| matches!(b.mem, Mem::Gpu(_))))
            .map(|(&n, _)| n).collect();
        for n in gpuwins {
            let ok = self.ovwinwhynot(n).is_none();
            if self.wins[&n].ovsent != Some(ok) {
                self.wins.get_mut(&n).unwrap().ovsent = Some(ok);
                self.event(n, format!("overlay {}", ok as u8));
            }
        }
        let Some(n) = self.ov else { return };
        let lastb = self.wins.get(&n).and_then(|w| w.lastb);
        if lastb.is_some_and(|b| self.overlayable(n, b)) {
            return;
        }
        self.ov = None;
        if let Some(tx) = &self.ovtx {
            let _ = tx.send(OvJob { win: n, b: 0, tag: None, count: 0, line: "overlay off".into() });
        }
        if let Some(b) = lastb {
            if self.wins.contains_key(&n) {
                if self.compshow(n, b) {
                    // the GPU composes what the plane showed: its buffer stays wsys'
                    let w = self.wins.get_mut(&n).unwrap();
                    if w.ovheld == Some(b) {
                        w.ovheld = None;
                    }
                } else {
                    let _ = self.copybuf(n, b, None);
                }
            }
        }
        self.ovletgo(n);
    }

    /// what window n shows, all of it or rectangle r (window coordinates), from
    /// its image into its window in draw(3); devdraw shows what is not hidden
    /// img again what window n shows, after presents straight into its layer
    fn freshen(&mut self, n: u32) {
        let Some(w) = self.wins.get(&n) else { return };
        if !w.stale {
            return;
        }
        let (img, l, r) = (w.img, w.layer, w.r);
        if let Err(e) = self.d.draw(img, &Rect::new(0, 0, r.dx(), r.dy()), l, r.x0, r.y0) {
            eprintln!("wsys: draw: {e}");
        }
        self.wins.get_mut(&n).unwrap().stale = false;
    }

    fn paint(&mut self, n: u32, r: Option<Rect>) {
        self.freshen(n);
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

    /// a text window shows the end of its text and the line being typed (none
    /// while a program has its mouse: then the program draws), the selection on
    /// a pale background; where each rune went is kept for the mouse
    fn drawtext(&mut self, n: u32) {
        let Some(w) = self.wins.get(&n) else { return };
        if !w.textwin || w.mouseopen > 0 || !w.bufs.is_empty() || self.font.is_none() {
            return;
        }
        let (r, l, scroll, raw, sel) = (w.r, w.layer, w.scroll, w.raw, w.sel);
        let mut start = w.text.len().saturating_sub(TEXTSHOWN);
        while !w.text.is_char_boundary(start) {
            start += 1;
        }
        let mut s = w.text[start..].to_string();
        if !raw {
            s.push_str(&w.line);
        }
        let (white, black, pale, scrollbg) = (self.white, self.black, self.pale, self.scrollbg);
        // while holding, the text is blue, as rio's
        let ink = if w.hold { self.holdcol } else { black };
        let want = w.want;
        let tabstop = self.tabstop;
        let (font, d) = (self.font.as_mut().unwrap(), &mut self.d);
        let scrollr = Rect::new(r.x0, r.y0, r.x0 + SCROLLWID, r.y1);
        let inner = Rect::new(r.x0 + SCROLLWID + SCROLLGAP, r.y0, r.x1, r.y1);
        let (maxw, h) = (inner.dx().max(1), font.height.max(1));
        let tab = (tabstop * font.width(d, '0')).max(1);
        // lay out: lines of (rune, x, width, offset)
        let mut lines: Vec<(Vec<(char, i32, i32, usize)>, usize)> = vec![(vec![], start)];
        let mut x = 0;
        for (i, c) in s.char_indices() {
            let off = start + i;
            let next = off + c.len_utf8();
            match c {
                '\n' => {
                    lines.last_mut().unwrap().1 = off;
                    lines.push((vec![], next));
                    x = 0;
                }
                '\r' => {}
                _ => {
                    let cw = if c == '\t' { ((x / tab + 1) * tab - x).max(1) } else { font.width(d, c) };
                    if x + cw > maxw && x > 0 {
                        lines.last_mut().unwrap().1 = off;
                        lines.push((vec![], off));
                        x = 0;
                    }
                    let cw = if c == '\t' { ((x / tab + 1) * tab - x).max(1) } else { cw };
                    let line = lines.last_mut().unwrap();
                    line.0.push((c, x, cw, off));
                    line.1 = next;
                    x += cw;
                }
            }
        }
        let nvis = (inner.dy() / h).max(1) as usize;
        // back no further than a window full from the start
        let mut scroll = scroll.min(lines.len().saturating_sub(nvis));
        if let Some(q) = want {
            // show the line of offset q (look): a third down from the top when it was not
            let li = lines.iter().position(|(_, e)| q <= *e).unwrap_or(lines.len() - 1);
            let (end0, begin0) = (lines.len() - scroll, (lines.len() - scroll).saturating_sub(nvis));
            if li < begin0 || li >= end0 {
                let top = li.saturating_sub(nvis / 3);
                scroll = lines.len().saturating_sub(top + nvis).min(lines.len().saturating_sub(nvis));
            }
        }
        let end = lines.len() - scroll;
        let begin = end.saturating_sub(nvis);
        let mut rows = vec![];
        let res = (|| -> std::io::Result<()> {
            d.draw(l, &r, white, 0, 0)?;
            // the scroll bar: grey, white where the lines shown are (rio's scrpos)
            d.draw(l, &scrollr, scrollbg, 0, 0)?;
            let (tot, hh) = (lines.len().max(1) as i64, scrollr.dy() as i64);
            let mut t = scrollr;
            t.y0 += (hh * begin as i64 / tot) as i32;
            t.y1 -= (hh * (tot - end as i64) / tot) as i32;
            if t.y1 < t.y0 + 2 {
                t.y1 = (t.y0 + 2).min(scrollr.y1);
                t.y0 = t.y1 - 2;
            }
            d.draw(l, &Rect::new(t.x0, t.y0, t.x1 - 1, t.y1), white, 0, 0)?;
            for (row, (line, lend)) in lines[begin..end].iter().enumerate() {
                let y = inner.y0 + row as i32 * h;
                for &(c, cx, cw, off) in line {
                    if off >= sel.0 && off < sel.1 {
                        d.draw(l, &Rect::new(inner.x0 + cx, y, inner.x0 + cx + cw, y + h).clip(&inner), pale, 0, 0)?;
                    }
                    if c == '\t' {
                        continue;
                    }
                    if let Some(g) = font.glyph(d, c) {
                        let dst = Rect::new(0, 0, g.src.dx(), g.src.dy()).add(inner.x0 + cx + g.dx, y + g.dy + g.src.y0);
                        d.drawmask(l, &dst.clip(&inner), ink, (0, 0), g.img, (g.src.x0, g.src.y0))?;
                    }
                }
                // a selection across the end of the line: the rest of it pale
                if *lend >= sel.0 && *lend < sel.1 {
                    let lx = line.last().map(|r| r.1 + r.2).unwrap_or(0);
                    d.draw(l, &Rect::new(inner.x0 + lx, y, inner.x1, y + h), pale, 0, 0)?;
                }
                rows.push(Row { y, runes: line.iter().map(|r| (inner.x0 + r.1, r.2, r.3)).collect(), end: *lend });
            }
            if scroll == 0 && !raw {
                // the tick where typing goes
                let y = inner.y0 + (end - begin - 1) as i32 * h;
                let t = Rect::new(inner.x0 + x, y, inner.x0 + x + 1, y + h).clip(&inner);
                d.draw(l, &t, black, 0, 0)?;
            }
            d.flush()
        })();
        if let Err(e) = res {
            eprintln!("wsys: text: {e}");
        }
        if let Some(w) = self.wins.get_mut(&n) {
            w.rows = rows;
            w.scrollr = scrollr;
            w.shown = (lines.len(), begin, nvis);
            w.scroll = scroll;
            w.want = None;
        }
    }

    /// button but in window n's scroll bar at y, as rio's: 1 back and 3 on by the
    /// lines from the top of the bar to the mouse, 2 to that place of the text
    fn scrollstep(&mut self, n: u32, but: i32, y: i32) {
        let h = self.font.as_ref().map(|f| f.height).unwrap_or(1).max(1);
        let Some(w) = self.wins.get_mut(&n) else { return };
        let s = w.scrollr;
        let (total, begin, nvis) = w.shown;
        let y = y.clamp(s.y0, s.y1);
        let lines = ((y - s.y0) / h).max(1) as usize;
        // scroll counts the lines back from the end (drawtext keeps it in the text)
        let _ = begin;
        w.scroll = match but {
            1 => (w.scroll + lines).min(total.saturating_sub(nvis)),
            4 => w.scroll.saturating_sub(lines),
            _ => {
                let top = (total as i64 * (y - s.y0).min(s.dy() - 2).max(0) as i64 / s.dy().max(1) as i64) as usize;
                total.saturating_sub(top + nvis)
            }
        };
        self.drawtext(n);
    }

    /// a double click at q in window n's text, as rio's: inside brackets or quotes
    /// to their match, a line from its start or end, else a word
    fn stretch(&self, n: u32, q: usize) -> (usize, usize) {
        let w = &self.wins[&n];
        let all: Vec<(usize, char)> = format!("{}{}", w.text, w.line).char_indices().collect();
        let end = w.text.len() + w.line.len();
        let i = all.partition_point(|&(o, _)| o < q); // rune index of q
        let at = |k: usize| all.get(k).map(|r| r.1).unwrap_or('\n');
        let off = |k: usize| all.get(k).map(|r| r.0).unwrap_or(end);
        let pairs: [(&str, &str); 3] = [("{[(<«", "}])>»"), ("\n", "\n"), ("'\"`", "'\"`")];
        for (l, r) in pairs {
            // the rune before q opens: forward to its match
            let c = if i == 0 { '\n' } else { at(i - 1) };
            if let Some(p) = l.chars().position(|x| x == c) {
                let cr = r.chars().nth(p).unwrap();
                let (mut k, mut nest) = (i, 1);
                while k < all.len() {
                    let x = at(k);
                    k += 1;
                    if x == cr {
                        nest -= 1;
                        if nest == 0 {
                            return (q, off(if c == '\n' { k } else { k - 1 }));
                        }
                    } else if x == c {
                        nest += 1;
                    }
                }
                return if c == '\n' && nest == 1 { (q, end) } else { (q, q) };
            }
            // the rune at q closes: back to its match
            let c = if i >= all.len() { '\n' } else { at(i) };
            if let Some(p) = r.chars().position(|x| x == c) {
                let cl = l.chars().nth(p).unwrap();
                let (mut k, mut nest) = (i, 1);
                while k > 0 {
                    k -= 1;
                    let x = at(k);
                    if x == cl {
                        nest -= 1;
                        if nest == 0 {
                            return (off(k + 1), off(i + (c == '\n' && i < all.len()) as usize));
                        }
                    } else if x == c {
                        nest += 1;
                    }
                }
                return if c == '\n' && nest == 1 { (0, off(i + (i < all.len()) as usize)) } else { (q, q) };
            }
        }
        // a word: letters, digits and _
        let word = |c: char| c.is_alphanumeric() || c == '_';
        let (mut a, mut b) = (i, i);
        while b < all.len() && word(at(b)) {
            b += 1;
        }
        while a > 0 && word(at(a - 1)) {
            a -= 1;
        }
        (off(a), off(b))
    }

    fn mouse(&mut self, x: i32, y: i32, b: i32, ms: u32) {
        (self.mx, self.my, self.ms) = (x, y, ms);
        if let Some(m) = &self.menu {
            // a menu while its button is held: let go over an item, it is done
            let button = m.button;
            self.menumove(x, y);
            if b & button == 0 {
                let m = self.menu.take().unwrap();
                let _ = self.d.free(m.layer);
                let _ = self.d.flush();
                if let Some(i) = m.cur {
                    match m.win {
                        Some(n) if self.wins.contains_key(&n) => self.menu2(n, &m.items[i]),
                        Some(_) => {}
                        None => self.menu3(i, &m.hidden),
                    }
                }
            }
            self.buttons = b;
            return;
        }
        if let Some((n, but, _)) = self.scrolling {
            // a button held in a scroll bar: 2 follows the mouse, 1 and 3 repeat on ticks
            if b & but == 0 {
                self.scrolling = None;
            } else if but == 2 {
                self.scrollstep(n, 2, y);
            }
            self.buttons = b;
            return;
        }
        if let Some((n, q0)) = self.selecting {
            // button 1 sweeps a selection in a text window
            if self.wins.contains_key(&n) {
                let q = self.hit(n, x, y);
                self.wins.get_mut(&n).unwrap().sel = (q0.min(q), q0.max(q));
                self.drawtext(n);
            }
            if b & 1 == 0 {
                self.selecting = None;
            }
            self.buttons = b;
            return;
        }
        if let Some(g) = self.drag {
            self.dragto(g, x, y, b);
            self.buttons = b;
            return;
        }
        let pressed = b != 0 && self.buttons == 0;
        let hit = self.z.iter().rev().copied().find(|n| !self.wins[n].hidden && self.wins[n].r.inset(-BORDER).contains(x, y));
        if pressed && self.op3.is_some() {
            // button 3's command takes this press (another button cancels it)
            let op = self.op3.take().unwrap();
            self.buttons = b;
            if b == 4 {
                self.do3(op, hit, x, y);
            }
            self.setcursor();
            return;
        }
        if pressed && b == 4 && self.font.is_some() {
            // button 3's menu: on the background, or a text window no program has the mouse of
            let mine = match hit {
                None => true,
                Some(n) => {
                    let w = &self.wins[&n];
                    // (button 3 in the scroll bar scrolls)
                    w.r.contains(x, y) && w.textwin && w.mouseopen == 0 && !w.scrollr.contains(x, y)
                }
            };
            if mine {
                let hidden: Vec<u32> = self.z.iter().copied().filter(|n| self.wins[n].hidden).collect();
                let mut items: Vec<String> = MENU3.iter().map(|s| s.to_string()).collect();
                for n in &hidden {
                    let w = &self.wins[n];
                    items.push(if w.label.is_empty() { format!("window {n}") } else { w.label.clone() });
                }
                self.openmenu(None, 4, items, hidden, x, y);
                self.buttons = b;
                return;
            }
        }
        if pressed {
            if let Some(n) = hit {
                if self.focus != Some(n) {
                    self.setfocus(Some(n));
                }
                self.raise(n);
                // on the border, as rio: button 1 or 2 sweeps the window to a new size
                // (rio's whichcorner: 20 pixels from an end of an edge take the
                // corner), button 3 moves it
                let r = self.wins[&n].r;
                if !r.contains(x, y) && b & 7 != 0 {
                    let kind = if b & 3 == 0 {
                        DragKind::Move
                    } else {
                        let f = r.inset(-BORDER);
                        let portion = |v: i32, lo: i32, hi: i32| {
                            let (v, hi) = (v - lo, hi - lo);
                            if v < hi / 2 { if v < CORNER { 0 } else { 1 } } else if v > hi - CORNER { 2 } else { 1 }
                        };
                        let (i, j) = (portion(x, f.x0, f.x1), portion(y, f.y0, f.y1));
                        DragKind::Sweep { left: i == 0, right: i == 2, top: j == 0, bottom: j == 2 }
                    };
                    self.drag = Some(Drag { win: n, kind, x0: x, y0: y, orig: r, cur: r });
                    self.buttons = b;
                    return;
                }
                // in a text window: button 1 selects, button 2 is its menu
                let w = &self.wins[&n];
                if r.contains(x, y) && w.textwin && w.mouseopen == 0 && self.font.is_some() {
                    if w.scrollr.contains(x, y) && (b == 1 || b == 2 || b == 4) {
                        // the scroll bar: a step now, more while the button is held
                        self.scrollstep(n, b, y);
                        let next = std::time::Instant::now() + std::time::Duration::from_millis(200);
                        self.scrolling = Some((n, b, next));
                        self.buttons = b;
                        return;
                    }
                    if b == 1 {
                        let q = self.hit(n, x, y);
                        // a second click at the same place within 500 ms: rio's double click
                        let double = matches!(self.click, Some((cw, cx, cy, cms))
                            if cw == n && (cx - x).abs() <= 3 && (cy - y).abs() <= 3 && ms.wrapping_sub(cms) < 500);
                        if double {
                            self.click = None;
                            let sel = self.stretch(n, q);
                            self.wins.get_mut(&n).unwrap().sel = sel;
                            self.drawtext(n);
                            self.buttons = b;
                            return;
                        }
                        self.click = Some((n, x, y, ms));
                        self.wins.get_mut(&n).unwrap().sel = (q, q);
                        self.selecting = Some((n, q));
                        self.drawtext(n);
                        self.buttons = b;
                        return;
                    }
                    if b == 2 {
                        // as rio's: scroll, or noscroll when on
                        let sc = if self.wins[&n].scrolling { "noscroll" } else { "scroll" };
                        let items = MENU2.iter().map(|s| s.to_string()).chain([sc.to_string()]).collect();
                        self.openmenu(Some(n), 2, items, vec![], x, y);
                        self.buttons = b;
                        return;
                    }
                }
            }
        }
        let held = b != 0 || self.buttons != 0;
        let wheel = b & 24 != 0 && self.buttons & 24 == 0;
        self.buttons = b;
        if let Some(n) = self.focus {
            let r = self.wins[&n].r;
            let w = &self.wins[&n];
            if wheel && w.textwin && w.mouseopen == 0 && r.contains(x, y) {
                // the wheel scrolls a text window: 8 back, 16 on
                let w = self.wins.get_mut(&n).unwrap();
                w.scroll = if b & 8 != 0 { w.scroll + 3 } else { w.scroll.saturating_sub(3) };
                self.drawtext(n);
            }
            let inside = r.contains(x, y);
            if !held && !inside && self.wins[&n].inside {
                // gone out: once more, so that the client knows (Wayland's leave)
                self.event(n, format!("m {} {} {} {}", x - r.x0, y - r.y0, b, ms));
                self.wins.get_mut(&n).unwrap().inside = false;
            }
            if held || inside {
                self.event(n, format!("m {} {} {} {}", x - r.x0, y - r.y0, b, ms));
                let w = self.wins.get_mut(&n).unwrap();
                w.inside |= inside;
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
        if !matches!(g.kind, DragKind::Box(None)) && !self.wins.contains_key(&g.win) {
            self.drag = None;
            self.outline(None);
            self.setcursor();
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
            DragKind::Box(target) => {
                // the frame from the press to the mouse; its content shown as the outline
                let f = Rect::new(g.x0.min(x), g.y0.min(y), g.x0.max(x), g.y0.max(y));
                let big = f.dx() >= MINSIZE + 2 * BORDER && f.dy() >= MINSIZE + 2 * BORDER;
                if b != 0 {
                    g.cur = f;
                    self.drag = Some(g);
                    self.outline(if big { Some(f.inset(BORDER)) } else { None });
                    return;
                }
                self.drag = None;
                self.outline(None);
                self.setcursor();
                if !big {
                    return; // too small: nothing
                }
                match target {
                    None => {
                        let o = Opts { r: Some(f), ..Default::default() };
                        if let Err(e) = self.spawn(o) {
                            eprintln!("wsys: new: {e}");
                        }
                    }
                    Some(n) => {
                        let _ = self.setrect(n, f.inset(BORDER));
                    }
                }
            }
        }
    }

    /// button 3's command item i, or a hidden window to bring back
    fn menu3(&mut self, i: usize, hidden: &[u32]) {
        if i >= MENU3.len() {
            if let Some(&n) = hidden.get(i - MENU3.len()) {
                if self.wins.contains_key(&n) {
                    self.hide(n, false);
                }
            }
            return;
        }
        self.op3 = Some([Op3::New, Op3::Resize, Op3::Move, Op3::Delete, Op3::Hide][i]);
        self.setcursor();
    }

    /// button 3 pressed for a waiting command: New and Resize sweep from here,
    /// Move drags the window, Delete and Hide take the window under the mouse
    fn do3(&mut self, op: Op3, hit: Option<u32>, x: i32, y: i32) {
        let r = Rect::new(x, y, x, y);
        match (op, hit) {
            (Op3::New, _) => self.drag = Some(Drag { win: 0, kind: DragKind::Box(None), x0: x, y0: y, orig: r, cur: r }),
            (Op3::Resize, Some(n)) => {
                self.raise(n);
                self.drag = Some(Drag { win: n, kind: DragKind::Box(Some(n)), x0: x, y0: y, orig: r, cur: r });
            }
            (Op3::Move, Some(n)) => {
                self.raise(n);
                let orig = self.wins[&n].r;
                self.drag = Some(Drag { win: n, kind: DragKind::Move, x0: x, y0: y, orig, cur: orig });
            }
            (Op3::Delete, Some(n)) => self.delwin(n, true),
            (Op3::Hide, Some(n)) => self.hide(n, true),
            _ => {}
        }
    }

    /// window n off the screen (rio's Hide: past its far corner) or back on it
    fn hide(&mut self, n: u32, off: bool) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        if w.hidden == off {
            return;
        }
        w.hidden = off;
        let (l, f) = (w.layer, w.r.inset(-BORDER));
        let at = if off { (self.d.r.x1, self.d.r.y1) } else { (f.x0, f.y0) };
        if let Err(e) = self.d.origin(l, (f.x0, f.y0), at).and_then(|_| self.d.flush()) {
            eprintln!("wsys: draw: {e}");
        }
        if off {
            if self.focus == Some(n) {
                let next = self.z.iter().rev().copied().find(|i| !self.wins[i].hidden);
                self.setfocus(next);
            }
        } else {
            self.raise(n);
            self.setfocus(Some(n));
        }
        self.changed(n);
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
        if !self.wins[&n].raw {
            self.drawtext(n); // the echo
        }
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
        // Esc toggles hold; Del while holding ends it, then interrupts (rio)
        if c == '\u{1b}' || (w.hold && c == '\u{7f}') {
            let on = !w.hold;
            self.sethold(n, on);
            if c == '\u{1b}' {
                return;
            }
        }
        let w = self.wins.get_mut(&n).unwrap();
        match c {
            '\u{7f}' => {
                // Del: interrupt, the line goes
                w.line.clear();
                if let Some(pid) = w.pid {
                    postnote(pid, "interrupt");
                }
            }
            '\u{8}' => {
                w.line.pop(); // backspace: the last rune
            }
            '\u{15}' => w.line.clear(), // ^U: the line
            '\u{4}' => {
                // ^D: the line as it is; on an empty one, end of file
                if w.line.is_empty() {
                    w.eof = true;
                }
                let l = std::mem::take(&mut w.line);
                w.cons.extend(l.as_bytes());
                w.text.push_str(&l);
            }
            '\n' if w.hold => w.line.push('\n'), // held: it waits
            '\n' => {
                // the line goes to the program, and stays in the text as typed
                let mut l = std::mem::take(&mut w.line);
                l.push('\n');
                w.cons.extend(l.as_bytes());
                w.text.push_str(&l);
                w.scroll = 0;
            }
            _ => w.line.push(c),
        }
        Self::trim(w);
    }

    /// a window's text no longer than MAXTEXT, its selection moved with it
    fn trim(w: &mut Win) {
        let (len, total) = (w.text.len(), w.text.len() + w.line.len());
        if len > MAXTEXT {
            let mut k = len - MAXTEXT;
            while !w.text.is_char_boundary(k) {
                k += 1;
            }
            w.text.drain(..k);
            w.sel = (w.sel.0.saturating_sub(k), w.sel.1.saturating_sub(k));
        }
        let total = total.min(w.text.len() + w.line.len());
        w.sel = (w.sel.0.min(total), w.sel.1.min(total));
    }

    /// what a program wrote to window n's cons or text: whole runes (the start
    /// of one waits for the rest), backspace takes one back
    fn output(&mut self, n: u32, data: &[u8]) {
        let w = self.wins.get_mut(&n).unwrap();
        let mut b = std::mem::take(&mut w.partial);
        b.extend_from_slice(data);
        let mut ok = match std::str::from_utf8(&b) {
            Ok(_) => b.len(),
            Err(e) if e.error_len().is_none() => e.valid_up_to(), // a rune cut short
            Err(_) => b.len(),
        };
        if b.len() - ok > 3 {
            ok = b.len();
        }
        w.partial = b[ok..].to_vec();
        let old = w.text.len();
        for c in String::from_utf8_lossy(&b[..ok]).chars() {
            if c == '\u{8}' {
                w.text.pop();
            } else {
                w.text.push(c);
            }
        }
        // a selection in the line typed stays on what it was
        let tl = w.text.len();
        let shift = |q: usize| if q >= old { q + tl - old.min(tl) } else { q.min(tl) };
        if tl >= old {
            w.sel = (shift(w.sel.0), shift(w.sel.1));
        }
        Self::trim(w);
        // at the end, or scroll: the end shown; looking back (noscroll, as rio's): the
        // same text stays where it was, the lines that came below it
        if w.scrolling || w.scroll == 0 {
            w.scroll = 0;
            self.drawtext(n);
            return;
        }
        let before = w.shown.0;
        self.drawtext(n);
        if let Some(w) = self.wins.get_mut(&n) {
            let added = w.shown.0.saturating_sub(before);
            if added > 0 {
                w.scroll += added;
                self.drawtext(n);
            }
        }
    }

    /// button 2's scroll and noscroll, wctl's: whether output goes to the end
    fn setscrolling(&mut self, n: u32, on: bool) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        w.scrolling = on;
        if on {
            w.scroll = 0;
            self.drawtext(n);
        }
    }

    /// the snarf buffer from rio's, when wsys runs in rio
    fn getsnarf(&mut self) {
        if self.parentsnarf {
            if let Ok(b) = std::fs::read("/dev/snarf") {
                self.snarf = b;
            }
        }
    }

    fn putsnarf(&mut self) {
        if self.parentsnarf {
            let _ = std::fs::write("/dev/snarf", &self.snarf);
        }
    }

    /// text and line of window n as one: the bytes from q0 to q1
    fn seltext(w: &Win) -> String {
        let all = format!("{}{}", w.text, w.line);
        let (q0, q1) = (w.sel.0.min(all.len()), w.sel.1.min(all.len()));
        all.get(q0..q1).unwrap_or("").to_string()
    }

    /// what is in window n's line, as typed: each newline sends the line
    fn typeline(&mut self, n: u32, s: &str) {
        for c in s.chars() {
            self.typed(n, c);
        }
        self.wake(n);
    }

    /// a command of button 2's menu in text window n
    fn menu2(&mut self, n: u32, cmd: &str) {
        let w = &self.wins[&n];
        let tl = w.text.len();
        let (q0, q1) = w.sel;
        match cmd {
            "snarf" | "cut" => {
                if q0 < q1 {
                    self.snarf = Self::seltext(w).into_bytes();
                    self.putsnarf();
                }
                if cmd == "cut" && q0 < q1 {
                    let w = self.wins.get_mut(&n).unwrap();
                    // the part in the line, then the part in the text
                    let (l0, l1) = (q0.max(tl) - tl, q1.max(tl) - tl);
                    if l0 < l1 && w.line.is_char_boundary(l0) && w.line.is_char_boundary(l1.min(w.line.len())) {
                        w.line.replace_range(l0..l1.min(w.line.len()), "");
                    }
                    let (t0, t1) = (q0.min(tl), q1.min(tl));
                    if t0 < t1 && w.text.is_char_boundary(t0) && w.text.is_char_boundary(t1) {
                        w.text.replace_range(t0..t1, "");
                    }
                    w.sel = (q0, q0);
                }
            }
            "paste" => {
                self.getsnarf();
                let s = String::from_utf8_lossy(&self.snarf).into_owned();
                let w = self.wins.get_mut(&n).unwrap();
                if q0 >= tl && q1 - tl <= w.line.len() && w.line.is_char_boundary(q0 - tl) && w.line.is_char_boundary(q1 - tl) {
                    // over a selection in the line: it is replaced, the rest is typed after it
                    let tail = w.line.split_off(q1 - tl);
                    w.line.truncate(q0 - tl);
                    w.sel = (q0, q0 + s.len());
                    self.typeline(n, &format!("{s}{tail}"));
                } else {
                    self.typeline(n, &s);
                }
            }
            "plumb" => self.plumb(n),
            "look" => {
                // the next place with the selected text, round the end to the start
                if q0 < q1 {
                    let all = format!("{}{}", w.text, w.line);
                    let pat = all.get(q0..q1).unwrap_or("").to_string();
                    let next = all.get(q1..).and_then(|r| r.find(&pat)).map(|i| q1 + i).or_else(|| all.find(&pat));
                    if let Some(i) = next {
                        let w = self.wins.get_mut(&n).unwrap();
                        w.sel = (i, i + pat.len());
                        w.want = Some(i);
                    }
                }
            }
            "send" => {
                let mut s = if q0 < q1 { Self::seltext(w) } else { String::from_utf8_lossy(&self.snarf).into_owned() };
                if !s.ends_with('\n') {
                    s.push('\n');
                }
                self.snarf = s.clone().into_bytes();
                self.putsnarf();
                self.typeline(n, &s);
            }
            "scroll" | "noscroll" => self.setscrolling(n, cmd == "scroll"),
            _ => {}
        }
        self.drawtext(n);
    }

    /// rio's hold mode on or off; off, what was typed meanwhile goes as lines
    fn sethold(&mut self, n: u32, on: bool) {
        let Some(w) = self.wins.get_mut(&n) else { return };
        if w.hold == on {
            return;
        }
        w.hold = on;
        if !on {
            if let Some(i) = w.line.rfind('\n') {
                let l: String = w.line.drain(..=i).collect();
                w.cons.extend(l.as_bytes());
                w.text.push_str(&l);
                w.scroll = 0;
            }
            self.wake(n);
        }
        self.paintborder(n);
        self.drawtext(n);
    }

    /// the selection to the plumber (plumb(6)), else the word around it with
    /// click= as rio does
    fn plumb(&mut self, n: u32) {
        if self.plumbfd.is_none() {
            // as libplumb's plumbopen: the plumber of wsys' name space (the profile
            // starts it before wsys)
            match Fd::open("/mnt/plumb/send", OWRITE) {
                Ok(f) => self.plumbfd = Some(f),
                Err(e) => {
                    eprintln!("wsys: plumb: {e}");
                    return;
                }
            }
        }
        let w = &self.wins[&n];
        let all = format!("{}{}", w.text, w.line);
        let (q0, q1) = (w.sel.0.min(all.len()), w.sel.1.min(all.len()));
        let (data, attr) = if q0 < q1 {
            (all[q0..q1].to_string(), String::new())
        } else {
            let sep = |c: char| c == ' ' || c == '\t' || c == '\n';
            let p0 = all[..q0].rfind(sep).map(|i| i + 1).unwrap_or(0);
            let p1 = all[q0..].find(sep).map(|i| q0 + i).unwrap_or(all.len());
            (all[p0..p1].to_string(), format!("click={}", all[p0..q0].chars().count()))
        };
        if data.is_empty() || data.len() > 8192 {
            return;
        }
        let wdir = w.wdir.trim().to_string();
        let msg = format!("rio\n\n{}\ntext\n{}\n{}\n{}", wdir, attr, data.len(), data);
        if let Err(e) = self.plumbfd.as_ref().unwrap().write1(msg.as_bytes()) {
            eprintln!("wsys: plumb: {e}");
        }
    }

    /// the offset in window n's text of the rune at x, y (as drawn)
    fn hit(&self, n: u32, x: i32, y: i32) -> usize {
        let w = &self.wins[&n];
        let h = self.font.as_ref().map(|f| f.height).unwrap_or(1).max(1);
        let Some(row) = w.rows.iter().find(|r| y < r.y + h).or(w.rows.last()) else {
            return w.text.len() + w.line.len();
        };
        if y < w.rows.first().map(|r| r.y).unwrap_or(0) {
            return row.runes.first().map(|r| r.2).unwrap_or(row.end);
        }
        for &(rx, rw, off) in &row.runes {
            if x < rx + rw / 2 {
                return off;
            }
        }
        row.end
    }

    /// button 2's menu, its item cur lit
    fn drawmenu(&mut self) {
        let Some(m) = &self.menu else { return };
        let (l, r, items, cur) = (m.layer, m.r, m.items.clone(), m.cur);
        let (white, black, hi, border) = (self.white, self.black, self.menuhi, self.cfocus);
        let (font, d) = (self.font.as_mut().unwrap(), &mut self.d);
        let h = font.height;
        let res = (|| -> std::io::Result<()> {
            d.draw(l, &r, border, 0, 0)?;
            let inner = r.inset(2);
            d.draw(l, &inner, white, 0, 0)?;
            for (i, it) in items.iter().enumerate() {
                let y = inner.y0 + i as i32 * h;
                let row = Rect::new(inner.x0, y, inner.x1, y + h);
                if cur == Some(i) {
                    d.draw(l, &row, hi, 0, 0)?;
                }
                let tw: i32 = it.chars().map(|c| font.width(d, c)).sum();
                let mut x = inner.x0 + (inner.dx() - tw) / 2;
                for c in it.chars() {
                    if let Some(g) = font.glyph(d, c) {
                        let dst = Rect::new(0, 0, g.src.dx(), g.src.dy()).add(x + g.dx, y + g.dy + g.src.y0);
                        d.drawmask(l, &dst, black, (0, 0), g.img, (g.src.x0, g.src.y0))?;
                        x += g.width;
                    }
                }
            }
            d.flush()
        })();
        if let Err(e) = res {
            eprintln!("wsys: menu: {e}");
        }
    }

    /// button b's menu at x, y for window n: the first item under the mouse
    fn openmenu(&mut self, n: Option<u32>, b: i32, items: Vec<String>, hidden: Vec<u32>, x: i32, y: i32) {
        let Some(font) = self.font.as_mut() else { return };
        let h = font.height;
        let d = &mut self.d;
        let wd = items.iter().map(|it| it.chars().map(|c| font.width(d, c)).sum::<i32>()).max().unwrap_or(0) + 16 + 4;
        let ht = items.len() as i32 * h + 4;
        let s = self.d.r;
        let mut r = Rect::new(x - wd / 2, y - h / 2 - 2, x - wd / 2 + wd, y - h / 2 - 2 + ht);
        let (dx, dy) = ((s.x0 - r.x0).max(0) + (s.x1 - r.x1).min(0), (s.y0 - r.y0).max(0) + (s.y1 - r.y1).min(0));
        r = r.add(dx, dy);
        let Ok(layer) = self.d.allocwindow(self.scr, r, 0xFFFF_FFFF) else { return };
        self.menu = Some(Menu { win: n, button: b, items, hidden, r, layer, cur: None });
        self.menumove(x, y);
    }

    fn menumove(&mut self, x: i32, y: i32) {
        let h = self.font.as_ref().map(|f| f.height).unwrap_or(1).max(1);
        let Some(m) = &mut self.menu else { return };
        let inner = m.r.inset(2);
        let cur = if inner.contains(x, y) { Some(((y - inner.y0) / h) as usize).filter(|&i| i < m.items.len()) } else { None };
        if cur != m.cur || m.cur.is_none() {
            m.cur = cur;
            self.drawmenu();
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
    eprintln!("usage: wsys [-gpOx] [-f font] [-s srvname] [-i cmd]");
    std::process::exit(1);
}

fn main() {
    let mut args = std::env::args().skip(1);
    let user = std::env::var("user").unwrap_or_else(|_| "none".into());
    // with its pid as rio: a wsys that died leaves its entry, another starts all the same
    let mut srvname = format!("wsys.{user}.{}", std::process::id());
    let mut alias = true; // /srv/wsys.$user too, unless -s names it
    // gpucomp: None the GPU where it can, quietly; Some(true) -g; Some(false) -p
    let (mut init, mut exitwith, mut nooverlay, mut gpucomp) = (None, false, false, None);
    let mut fontname = std::env::var("font").ok().filter(|f| !f.is_empty()).unwrap_or(DEFFONT.to_string());
    while let Some(a) = args.next() {
        match a.as_str() {
            "-s" => {
                srvname = args.next().unwrap_or_else(|| usage());
                alias = false;
            }
            "-i" => init = Some(args.next().unwrap_or_else(|| usage())),
            "-x" => exitwith = true,
            "-O" => nooverlay = true,
            "-g" => gpucomp = Some(true),
            "-p" => gpucomp = Some(false),
            "-f" => fontname = args.next().unwrap_or_else(|| usage()),
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
    // the user's wsys under a name that does not change: the newest one's
    // (a wsys gone left its entry; programs that outlive it, such as wlsys,
    // find the next one there); made first, so that whoever waits for the
    // entry with the pid finds this one ready too
    if alias {
        let apath = format!("/srv/wsys.{user}");
        let _ = p9::remove(&apath);
        match Fd::create(&apath, OWRITE, 0o600) {
            Ok(a) => {
                if let Err(e) = a.write1(p0.0.to_string().as_bytes()) {
                    eprintln!("wsys: post {apath}: {e}");
                }
                a.leak();
            }
            Err(e) => eprintln!("wsys: post {apath}: {e}"),
        }
    }
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
    {
        let tx = tx.clone();
        thread::spawn(move || {
            while tx.send(Ev::Tick).is_ok() {
                thread::sleep(std::time::Duration::from_millis(50));
            }
        });
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
        font: font::Font::open(&fontname).inspect_err(|e| eprintln!("wsys: font {fontname}: {e}")).ok(),
        black: 0,
        pale: 0,
        menuhi: 0,
        selecting: None,
        menu: None,
        op3: None,
        scrollbg: 0,
        scrolling: None,
        click: None,
        holdcol: 0,
        paleholdcol: 0,
        ovtx: None,
        ov: None,
        ovtags: vec![],
        ovwhy: None,
        nooverlay,
        plumbfd: None,
        gpu: None,
        segs: HashMap::new(),
        comp: None,
        compscene: vec![],
        compdirty: false,
        compgone: vec![],
        sweepr: None,
        tabstop: std::env::var("tabstop").ok().and_then(|t| t.trim().parse().ok()).filter(|&t: &i32| t > 0).unwrap_or(4),
        // rio's snarf, when wsys runs in a window of rio
        parentsnarf: std::fs::metadata("/dev/snarf").is_ok() && std::fs::metadata("/dev/wctl").is_ok(),
    };
    let colors = (|| -> std::io::Result<(u32, u32, u32, u32, u32)> {
        Ok((ws.d.color(BG)?, ws.d.color(FOCUSED)?, ws.d.color(UNFOCUSED)?, ws.d.color(WHITE)?, ws.d.color(0)?))
    })();
    match colors {
        Ok((a, b, c, w, k)) => (ws.bg, ws.cfocus, ws.cother, ws.white, ws.black) = (a, b, c, w, k),
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
    match ws.d.color(PALEYELLOW).and_then(|p| {
        Ok((p, ws.d.color(PALEBLUEGREEN)?, ws.d.color(SCROLLBG)?, ws.d.color(HOLDCOL)?, ws.d.color(PALEHOLDCOL)?))
    }) {
        Ok((p, h, sb, hc, phc)) => (ws.pale, ws.menuhi, ws.scrollbg, ws.holdcol, ws.paleholdcol) = (p, h, sb, hc, phc),
        Err(e) => {
            eprintln!("wsys: colours: {e}");
            std::process::exit(1);
        }
    }
    match made {
        Ok(scr) => ws.scr = scr,
        Err(e) => {
            eprintln!("wsys: screen: {e}");
            std::process::exit(1);
        }
    }
    // the GPU composes the screen (unless -p), if wsys has it all (not in a window:
    // the frames would go into the window wsys draws into) and Vulkan can show it;
    // -g says why not and what it does
    if gpucomp != Some(false) {
        let verbose = gpucomp == Some(true) || std::env::var("WSYSDEBUG").is_ok_and(|v| v.contains("comp"));
        let winname = std::fs::read_to_string("/dev/winname").unwrap_or_default();
        let why = if !winname.starts_with("noborder.screen.") {
            Some(format!("not the whole screen ({winname})"))
        } else if ws.d.chan != XRGB32 {
            Some(format!("the screen is {}, not x8r8g8b8", draw::chanstr(ws.d.chan)))
        } else {
            let tx = tx.clone();
            match comp::Comp::start(
                Box::new(move |d| {
                    let _ = tx.send(Ev::Comp(d));
                }),
                verbose,
            ) {
                Ok(c) => {
                    ws.comp = Some(c);
                    None
                }
                Err(e) => Some(e),
            }
        };
        if let Some(why) = why.filter(|_| verbose) {
            eprintln!("wsys: -g: the processor composes: {why}");
        }
    }
    if exitwith {
        // for the scripts that run wsys -x: where it is (a desktop prints nothing:
        // the kernel's console would draw over the screen)
        println!("wsys: {srvpath} {} {} {} {}", all.x0, all.y0, all.x1, all.y1);
        let _ = std::io::stdout().flush();
    }

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
            Ev::Tick => {
                if let Some((n, but, next)) = ws.scrolling {
                    let now = std::time::Instant::now();
                    if but != 2 && now >= next {
                        let my = ws.my;
                        ws.scrollstep(n, but, my);
                        ws.scrolling = Some((n, but, now + std::time::Duration::from_millis(100)));
                    }
                }
            }
            Ev::WinExit(n) => {
                // its process is gone: the window too, unless files of it are open
                if let Some(w) = ws.wins.get_mut(&n) {
                    w.pinned = false;
                    if w.refs == 0 {
                        ws.delwin(n, false);
                    }
                }
            }
            Ev::Overlay { win, b, tag, count, err } => ws.overlay_done(win, b, tag, count, err),
            Ev::Comp(comp::Done::Composed(keys)) => ws.composed(keys),
            Ev::Comp(comp::Done::Failed(e)) => {
                if ws.comp.is_some() {
                    ws.compfailed(e);
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
        // after whatever changed: the window on the overlay plane may have to leave it;
        // with -g, what the GPU composes
        ws.ovcheck();
        ws.compsync();
    }
    let _ = p9::remove(&srvpath);
    // the windows let go of their clients' segments; their processes hang up
    let ids: Vec<u32> = ws.wins.keys().copied().collect();
    for n in ids {
        ws.delwin(n, true);
    }
    // std's threads are procs of their own, blocked in reads of the mouse and the
    // keyboard: std's exit kills them (end_program, /proc/n/ctl kill under its lock of the
    // thread list). Not a kill note to wsys' own note group: it could hit a thread inside
    // that lock (one ending, as the ticker when its channel closes), and exit would then
    // wait for the lock forever (wsys -x hung so, spinning in sleep(0)).
    std::process::exit(0);
}
