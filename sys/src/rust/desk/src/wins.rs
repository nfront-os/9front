// SPDX-License-Identifier: MIT
//
// The windows of /dev/wsys, as winwatch reads them (wsys(4): each window's
// directory with its wctl and label), and what the bars do with them: a
// message to a window's wctl, a command run by rc.

use std::io::Read;

#[derive(Clone, Debug, PartialEq)]
pub struct Win {
    pub n: u32,
    pub label: String, // "window n" when it has none
    pub current: bool,
    pub hidden: bool,
}

/// one read of a file (wctl: a second read would wait for a change)
pub fn read1(path: &str) -> Option<String> {
    let mut f = std::fs::File::open(path).ok()?;
    let mut b = [0u8; 512];
    let n = f.read(&mut b).ok()?;
    Some(String::from_utf8_lossy(&b[..n]).into_owned())
}

/// this program's window (/dev/winid), 0 if none
pub fn own() -> u32 {
    read1("/dev/winid").and_then(|s| s.trim().parse().ok()).unwrap_or(0)
}

/// the windows of /dev/wsys by number, but for skip (one's own)
pub fn list(skip: u32) -> Vec<Win> {
    let mut v = vec![];
    let Ok(d) = std::fs::read_dir("/dev/wsys") else { return v };
    for e in d.flatten() {
        let Ok(n) = e.file_name().to_string_lossy().parse::<u32>() else { continue };
        if n == skip {
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

/// messages to window n's wctl, in turn; an error is said, the rest still sent
pub fn wctl(n: u32, msgs: &[&str], who: &str) {
    for m in msgs {
        if let Err(e) = std::fs::write(format!("/dev/wsys/{n}/wctl"), m) {
            eprintln!("{who}: window {n}: {m}: {e}");
        }
    }
}

/// cmd run by rc, in this program's name space, not waited for here
pub fn run(cmd: &str, who: &str) {
    match std::process::Command::new("rc").args(["-c", cmd]).spawn() {
        Ok(mut c) => {
            std::thread::spawn(move || {
                let _ = c.wait();
            });
        }
        Err(e) => eprintln!("{who}: rc: {e}"),
    }
}
