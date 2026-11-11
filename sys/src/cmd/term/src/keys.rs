// SPDX-License-Identifier: MIT
//
// What a key typed in term sends to the pty: characters as UTF-8, Plan 9's
// runes for the other keys (keyboard.h) as xterm's sequences (the cursor keys
// as SS3 in the application cursor mode), Enter as carriage return and
// backspace as DEL, as a POSIX program expects them. Del is the caller's:
// rio's interrupt where a shell reads lines, else xterm's delete.

use p9win::keys as k;

pub enum Key {
    Bytes(Vec<u8>),
    Del,
    /// Shift and PgUp/PgDn or the wheel: term's own scrolling
    Scroll(i32),
}

fn csi(s: &str) -> Key {
    Key::Bytes(format!("\x1b[{s}").into_bytes())
}

/// c typed; appcursor: the program asked for the cursor keys' application mode;
/// shift held
pub fn key(c: char, appcursor: bool, shift: bool) -> Key {
    let r = c as u32;
    let cursor = |f: char| {
        if appcursor { Key::Bytes(format!("\x1bO{f}").into_bytes()) } else { csi(&f.to_string()) }
    };
    match r {
        k::UP => cursor('A'),
        k::DOWN => cursor('B'),
        k::RIGHT => cursor('C'),
        k::LEFT => cursor('D'),
        k::HOME => cursor('H'),
        k::END => cursor('F'),
        k::PGUP if shift => Key::Scroll(-1),
        k::PGDOWN if shift => Key::Scroll(1),
        k::PGUP => csi("5~"),
        k::PGDOWN => csi("6~"),
        k::INS => csi("2~"),
        0x7f => Key::Del,
        0x08 => Key::Bytes(vec![0x7f]),
        0x0a => Key::Bytes(vec![b'\r']),
        _ if (k::KF + 1..=k::KF + 12).contains(&r) => match r - k::KF {
            1 => Key::Bytes(b"\x1bOP".to_vec()),
            2 => Key::Bytes(b"\x1bOQ".to_vec()),
            3 => Key::Bytes(b"\x1bOR".to_vec()),
            4 => Key::Bytes(b"\x1bOS".to_vec()),
            n => csi(&format!("{}~", [15, 17, 18, 19, 20, 21, 23, 24][n as usize - 5])),
        },
        // the other keys of Plan 9's own (Alt, Shift, Print, ...): nothing
        _ if (k::KF..k::KF + 0x1000).contains(&r) => Key::Bytes(vec![]),
        _ => {
            let mut b = [0u8; 4];
            Key::Bytes(c.encode_utf8(&mut b).as_bytes().to_vec())
        }
    }
}
