// SPDX-License-Identifier: MIT
//
// term's pty: one of ptyfs(4) (started and mounted after /dev when there is
// none), its master held here, the program on its slave: rc binds the slave
// on /dev/cons and its consctl on /dev/consctl in a namespace and note group
// of its own, tells ptyfs that note group (interrupt, hangup) and execs the
// program, so Plan 9's programs run in it as in a window of rio and POSIX
// programs (APE) find a terminal with termios.

use p9win::sys::{Fd, ORDWR};
use std::ffi::{CString, c_char, c_int};
use std::io;
use std::process::{Child, Command, Stdio};
use std::sync::Arc;

unsafe extern "C" {
    #[link_name = "__p9_mount"]
    fn p9mount(fd: c_int, afd: c_int, old: *const c_char, flag: c_int, aname: *const c_char) -> c_int;
    #[link_name = "__p9_rfork"]
    fn p9rfork(flags: c_int) -> c_int;
}

const MAFTER: c_int = 2;
const RFNAMEG: c_int = 1;

// APE's termios.h: the modes ptyctl's IOR line gives in hex
const ICANON: u32 = 0x010;
const ISIG: u32 = 0x040;
const VINTR: usize = 3;

/// a namespace of term's own: ptyfs mounted in it is not the window's
pub fn ownns() -> io::Result<()> {
    if unsafe { p9rfork(RFNAMEG) } < 0 { Err(p9win::sys::err()) } else { Ok(()) }
}

fn exists(name: &str) -> bool {
    std::fs::metadata(name).is_ok()
}

/// /srv/ptyfs after /dev; false if that fails (a ptyfs that has gone)
fn mountsrv() -> bool {
    let Ok(f) = Fd::open("/srv/ptyfs", ORDWR) else { return false };
    let old = CString::new("/dev").unwrap();
    let aname = CString::new("").unwrap();
    // mount closes the descriptor when it works
    let ok = unsafe { p9mount(f.0, -1, old.as_ptr(), MAFTER, aname.as_ptr()) } >= 0;
    if ok {
        std::mem::forget(f);
    }
    ok
}

/// /dev/ptyclone, from the ptyfs posted or a new one (as APE's _getpty)
fn server() -> io::Result<()> {
    if exists("/dev/ptyclone") || mountsrv() {
        return Ok(());
    }
    // a ptyfs gone (its srv entry hung up): a new one posts the name again
    let _ = std::fs::remove_file("/srv/ptyfs");
    // without our window in its namespace: ptyfs outlives term, and a window of
    // wsys or rio (/mnt/wsys, bound before /dev) held there would stay
    Command::new("/bin/rc")
        .arg("-c")
        .arg("rfork n; unmount /mnt/wsys /dev >[2]/dev/null; unmount /mnt/wsys >[2]/dev/null; exec /bin/ape/ptyfs")
        .stdin(Stdio::null())
        .spawn()
        .map_err(|e| io::Error::other(format!("/bin/ape/ptyfs: {e}")))?
        .wait()?;
    for _ in 0..50 {
        if mountsrv() {
            return Ok(());
        }
        std::thread::sleep(std::time::Duration::from_millis(100));
    }
    Err(io::Error::other("no ptyfs: /srv/ptyfs not mounted"))
}

pub struct Pty {
    pub master: Arc<Fd>, // written here, read by reader's thread
    pub n: u32,
    ctl: Fd,
}

/// the pty's number from the master's qid (ptyfs: "PTY"<<40 | n<<4 | type), as
/// #d's ctl file shows it: "fd mode type dev (path vers type) ..."
fn ptynum(fd: &Fd) -> io::Result<u32> {
    let s = std::fs::read_to_string(format!("#d/{}ctl", fd.0))?;
    let path = s
        .split('(')
        .nth(1)
        .and_then(|t| t.split_whitespace().next())
        .and_then(|h| u64::from_str_radix(h, 16).ok())
        .ok_or_else(|| io::Error::other(format!("no qid in #d: {s}")))?;
    if path >> 40 != 0x505459 {
        return Err(io::Error::other("ptyclone gave no pty"));
    }
    Ok(((path >> 4) & 0xFFF_FFFF) as u32)
}

impl Pty {
    pub fn open() -> io::Result<Pty> {
        server()?;
        let master = Fd::open("/dev/ptyclone", ORDWR)?;
        let n = ptynum(&master)?;
        let ctl = Fd::open(&format!("/dev/ptyctl{n}"), ORDWR)?;
        Ok(Pty { master: Arc::new(master), n, ctl })
    }

    /// the program on the slave: argv, rc -i without one
    pub fn spawn(&self, argv: &[String]) -> io::Result<Child> {
        let n = self.n;
        let slave = || std::fs::OpenOptions::new().read(true).write(true).open(format!("/dev/ptty{n}"));
        let cmd = if argv.is_empty() {
            "exec rc -i".to_string()
        } else {
            format!("exec {}", argv.iter().map(|a| quote(a)).collect::<Vec<_>>().join(" "))
        };
        let script = format!(
            "rfork ns; bind /dev/ptty{n} /dev/cons; bind /dev/pconsctl{n} /dev/consctl; \
             echo IOW note $pid >/dev/ptyctl{n}; TERM=xterm-256color; {cmd}"
        );
        Command::new("/bin/rc")
            .arg("-c")
            .arg(&script)
            .stdin(slave()?)
            .stdout(slave()?)
            .stderr(slave()?)
            .spawn()
    }

    /// the terminal's size, told to the programs that ask (TIOCGWINSZ)
    pub fn winsize(&self, rows: u16, cols: u16, xpix: i32, ypix: i32) {
        let _ = self.ctl.write1(format!("IOW winsize {rows} {cols} {xpix} {ypix}").as_bytes());
    }

    /// the modes now: (lflag, interrupt character)
    fn modes(&self) -> Option<(u32, u8)> {
        let mut b = [0u8; 256];
        let n = self.ctl.pread(&mut b, 0).ok()?;
        let s = String::from_utf8_lossy(&b[..n]);
        let f: Vec<&str> = s.split_whitespace().collect();
        if f.len() < 5 + VINTR + 1 || f[0] != "IOR" {
            return None;
        }
        let lflag = u32::from_str_radix(f[4], 16).ok()?;
        let intr = u8::from_str_radix(f[5 + VINTR], 16).ok()?;
        Some((lflag, intr))
    }

    /// the line discipline edits lines (a shell reads them): Del interrupts there,
    /// as in rio; the character that does it (ISIG), if there is one
    pub fn interrupt(&self) -> Option<u8> {
        let (lflag, intr) = self.modes()?;
        (lflag & ICANON != 0 && lflag & ISIG != 0 && intr != 0).then_some(intr)
    }
}

/// an argument as rc reads it
fn quote(s: &str) -> String {
    if !s.is_empty() && s.chars().all(|c| c.is_ascii_alphanumeric() || "/._-+=:,".contains(c)) {
        s.to_string()
    } else {
        format!("'{}'", s.replace('\'', "''"))
    }
}

/// the master's output, read by a thread of its own: send gets each piece,
/// None at the end (no program on the slave any more)
pub fn reader<F>(master: Arc<Fd>, send: F)
where
    F: Fn(Option<Vec<u8>>) + Send + 'static,
{
    std::thread::Builder::new()
        .name("term pty".into())
        .spawn(move || {
            let mut b = vec![0u8; 16384];
            loop {
                match master.read(&mut b) {
                    Ok(n) if n > 0 => send(Some(b[..n].to_vec())),
                    _ => break,
                }
            }
            send(None);
        })
        .expect("thread");
}
