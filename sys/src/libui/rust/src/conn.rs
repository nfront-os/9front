// SPDX-License-Identifier: MIT
//
// The files of a ui (ui(4)): mounted from its /srv entry, in a namespace of
// the program's own (it does not change its shell's); ui started in a new
// window of rio or wsys (window(1)) first, as libui's uinew does (ui(2)).

use std::ffi::{CString, c_char, c_int};
use std::fs::File;
use std::io::{self, Read, Write};
use std::time::{Duration, Instant};

use xilem_core::{MessageResult, View};

use crate::{Cx, Event, Node};

unsafe extern "C" {
    #[link_name = "__p9_mount"]
    fn p9mount(fd: c_int, afd: c_int, old: *const c_char, flag: c_int, aname: *const c_char) -> c_int;
    #[link_name = "__p9_unmount"]
    fn p9unmount(name: *const c_char, old: *const c_char) -> c_int;
    #[link_name = "__p9_rfork"]
    fn p9rfork(flags: c_int) -> c_int;
    #[link_name = "__p9_open"]
    fn p9open(file: *const c_char, mode: c_int) -> c_int;
    #[link_name = "__p9_close"]
    fn p9close(fd: c_int) -> c_int;
    #[link_name = "__p9_rerrstr"]
    fn p9rerrstr(buf: *mut c_char, n: u32);
}

const ORDWR: c_int = 2;
const MREPL: c_int = 0;
const RFNAMEG: c_int = 1 << 0;

fn err(what: &str) -> io::Error {
    let mut b = [0u8; 256];
    unsafe { p9rerrstr(b.as_mut_ptr() as *mut c_char, b.len() as u32) };
    let n = b.iter().position(|&c| c == 0).unwrap_or(b.len());
    io::Error::other(format!("{what}: {}", String::from_utf8_lossy(&b[..n])))
}

fn cstr(s: &str) -> io::Result<CString> {
    CString::new(s).map_err(|_| io::Error::other("nul in name"))
}

/// The files of a ui: its ctl (transactions) and event (what the user did).
pub struct Ui {
    pub srv: String,
    pub mtpt: String,
    ctl: File,
    event: File,
    buf: Vec<u8>,
}

impl Ui {
    /// the ui posted as /srv/srv, its files on mtpt (the program's own
    /// namespace from here on)
    pub fn open(srv: &str, mtpt: &str) -> io::Result<Ui> {
        let s = cstr(&format!("/srv/{srv}"))?;
        let fd = unsafe { p9open(s.as_ptr(), ORDWR) };
        if fd < 0 {
            return Err(err(&format!("/srv/{srv}")));
        }
        let m = cstr(mtpt)?;
        let none = cstr("")?;
        let r = unsafe {
            if p9rfork(RFNAMEG) < 0 {
                -1
            } else {
                p9mount(fd, -1, m.as_ptr(), MREPL, none.as_ptr())
            }
        };
        unsafe { p9close(fd) };
        if r < 0 {
            return Err(err(&format!("mount {mtpt}")));
        }
        let ctl = File::options().write(true).open(format!("{mtpt}/ctl"))?;
        let event = File::open(format!("{mtpt}/event"))?;
        Ok(Ui { srv: srv.into(), mtpt: mtpt.into(), ctl, event, buf: Vec::new() })
    }

    /// ui in a new window, titled
    pub fn new(title: &str) -> io::Result<Ui> {
        let user = std::env::var("user").unwrap_or_else(|_| "none".into());
        let srv = format!("ui.{user}.{}.0", std::process::id());
        // window(1) returns once the window is there; ui posts soon after
        std::process::Command::new("/bin/window").arg(format!("/bin/ui -s {srv}")).status()?;
        let t = Instant::now();
        while std::fs::metadata(format!("/srv/{srv}")).is_err() {
            if t.elapsed() > Duration::from_secs(10) {
                return Err(io::Error::other(format!("no /srv/{srv} from a new ui")));
            }
            std::thread::sleep(Duration::from_millis(100));
        }
        let mut u = Ui::open(&srv, "/mnt/ui")?;
        u.ctl(&format!("title {}\n", crate::quote(title)))?;
        Ok(u)
    }

    /// a transaction: its lines all or none
    pub fn ctl(&mut self, t: &str) -> io::Result<()> {
        if t.is_empty() {
            return Ok(());
        }
        let n = self.ctl.write(t.as_bytes())?;
        if n != t.len() {
            return Err(io::Error::other("short write of ctl"));
        }
        Ok(())
    }

    /// the next line of event, waiting for it; None when ui is gone
    pub fn event(&mut self) -> Option<String> {
        loop {
            if let Some(i) = self.buf.iter().position(|&c| c == b'\n') {
                let line: Vec<u8> = self.buf.drain(..=i).collect();
                return Some(String::from_utf8_lossy(&line[..i]).into_owned());
            }
            let mut b = [0u8; 8192];
            match self.event.read(&mut b) {
                Ok(0) | Err(_) => return None,
                Ok(n) => self.buf.extend_from_slice(&b[..n]),
            }
        }
    }
}

impl Drop for Ui {
    fn drop(&mut self) {
        if let Ok(m) = cstr(&self.mtpt) {
            unsafe { p9unmount(std::ptr::null(), m.as_ptr()) };
        }
    }
}

/// The program's window: ui in a new one, the views of app(state) in it,
/// until the window is closed.
pub fn run<S: 'static, V>(title: &str, state: S, app: impl FnMut(&mut S) -> V) -> io::Result<()>
where
    V: View<S, (), Cx, Element = Node>,
{
    run_on(Ui::new(title)?, state, app)
}

/// as run, in a ui's window already there
pub fn run_on<S: 'static, V>(mut ui: Ui, mut state: S, mut app: impl FnMut(&mut S) -> V) -> io::Result<()>
where
    V: View<S, (), Cx, Element = Node>,
{
    let mut cx = Cx::new();
    let mut view = app(&mut state);
    let (mut root, mut vs) = view.build(&mut cx, &mut state);
    let t = cx.take();
    ui.ctl(&format!("clear\n{t}"))?;
    while let Some(line) = ui.event() {
        if line == "close" {
            break;
        }
        let Some(ev) = Event::parse(&line) else { continue };
        match cx.message(&view, &mut vs, &mut root, &mut state, ev) {
            MessageResult::Stale | MessageResult::Nop => continue,
            MessageResult::Action(()) | MessageResult::RequestRebuild => {}
        }
        let next = app(&mut state);
        cx.rebuild(&next, &view, &mut vs, &mut root, &mut state);
        view = next;
        ui.ctl(&cx.take())?;
    }
    Ok(())
}
