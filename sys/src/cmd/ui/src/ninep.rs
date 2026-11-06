// SPDX-License-Identifier: MIT
#![allow(dead_code)] // the whole of 9P, used or not
//
// 9P2000 (intro(5)): the messages a file server receives, and its answers
// (as wsys has them: sys/src/cmd/wsys/src/ninep.rs).

pub const QTDIR: u8 = 0x80;
pub const DMDIR: u32 = 0x8000_0000;
pub const IOHDRSZ: u32 = 24;

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Qid {
    pub ty: u8,
    pub vers: u32,
    pub path: u64,
}

pub struct Dir {
    pub qid: Qid,
    pub mode: u32,
    pub length: u64,
    pub name: String,
    pub uid: String,
    pub mtime: u32,
}

#[derive(Debug)]
pub enum T {
    Version { msize: u32, version: String },
    Auth,
    Attach { fid: u32, _uname: String, aname: String },
    Flush { oldtag: u16 },
    Walk { fid: u32, newfid: u32, names: Vec<String> },
    Open { fid: u32, mode: u8 },
    Create,
    Read { fid: u32, offset: u64, count: u32 },
    Write { fid: u32, offset: u64, data: Vec<u8> },
    Clunk { fid: u32 },
    Remove { fid: u32 },
    Stat { fid: u32 },
    Wstat,
}

struct Rd<'a> {
    b: &'a [u8],
    i: usize,
}

impl Rd<'_> {
    fn take(&mut self, n: usize) -> Option<&[u8]> {
        let s = self.b.get(self.i..self.i + n)?;
        self.i += n;
        Some(s)
    }
    fn u8(&mut self) -> Option<u8> {
        Some(self.take(1)?[0])
    }
    fn u16(&mut self) -> Option<u16> {
        Some(u16::from_le_bytes(self.take(2)?.try_into().ok()?))
    }
    fn u32(&mut self) -> Option<u32> {
        Some(u32::from_le_bytes(self.take(4)?.try_into().ok()?))
    }
    fn u64(&mut self) -> Option<u64> {
        Some(u64::from_le_bytes(self.take(8)?.try_into().ok()?))
    }
    fn str(&mut self) -> Option<String> {
        let n = self.u16()? as usize;
        Some(String::from_utf8_lossy(self.take(n)?).into_owned())
    }
}

/// a message as read from the connection: (tag, the request)
pub fn parse(m: &[u8]) -> Option<(u16, T)> {
    let mut r = Rd { b: m, i: 0 };
    let size = r.u32()? as usize;
    if size != m.len() {
        return None;
    }
    let ty = r.u8()?;
    let tag = r.u16()?;
    let t = match ty {
        100 => T::Version { msize: r.u32()?, version: r.str()? },
        102 => T::Auth,
        104 => {
            let fid = r.u32()?;
            let _afid = r.u32()?;
            T::Attach { fid, _uname: r.str()?, aname: r.str()? }
        }
        108 => T::Flush { oldtag: r.u16()? },
        110 => {
            let fid = r.u32()?;
            let newfid = r.u32()?;
            let n = r.u16()?;
            let names = (0..n).map(|_| r.str()).collect::<Option<Vec<_>>>()?;
            T::Walk { fid, newfid, names }
        }
        112 => T::Open { fid: r.u32()?, mode: r.u8()? },
        114 => T::Create,
        116 => T::Read { fid: r.u32()?, offset: r.u64()?, count: r.u32()? },
        118 => {
            let fid = r.u32()?;
            let offset = r.u64()?;
            let n = r.u32()? as usize;
            T::Write { fid, offset, data: r.take(n)?.to_vec() }
        }
        120 => T::Clunk { fid: r.u32()? },
        122 => T::Remove { fid: r.u32()? },
        124 => T::Stat { fid: r.u32()? },
        126 => T::Wstat,
        _ => return None,
    };
    Some((tag, t))
}

/// an answer under construction: R(type, tag).u32(..)...done()
pub struct R(Vec<u8>);

impl R {
    pub fn new(ty: u8, tag: u16) -> R {
        let mut v = vec![0; 4];
        v.push(ty);
        v.extend_from_slice(&tag.to_le_bytes());
        R(v)
    }
    pub fn u16(mut self, x: u16) -> R {
        self.0.extend_from_slice(&x.to_le_bytes());
        self
    }
    pub fn u32(mut self, x: u32) -> R {
        self.0.extend_from_slice(&x.to_le_bytes());
        self
    }
    pub fn str(mut self, s: &str) -> R {
        self.0.extend_from_slice(&(s.len() as u16).to_le_bytes());
        self.0.extend_from_slice(s.as_bytes());
        self
    }
    pub fn qid(mut self, q: Qid) -> R {
        put_qid(&mut self.0, q);
        self
    }
    pub fn bytes(mut self, b: &[u8]) -> R {
        self.0.extend_from_slice(b);
        self
    }
    pub fn done(mut self) -> Vec<u8> {
        let n = self.0.len() as u32;
        self.0[..4].copy_from_slice(&n.to_le_bytes());
        self.0
    }
}

fn put_qid(v: &mut Vec<u8>, q: Qid) {
    v.push(q.ty);
    v.extend_from_slice(&q.vers.to_le_bytes());
    v.extend_from_slice(&q.path.to_le_bytes());
}

pub fn error(tag: u16, e: &str) -> Vec<u8> {
    R::new(107, tag).str(e).done()
}

/// a directory entry as stat(5) has it (with its leading size)
pub fn stat(d: &Dir) -> Vec<u8> {
    let mut v = vec![0u8; 2];
    v.extend_from_slice(&0u16.to_le_bytes()); // type
    v.extend_from_slice(&0u32.to_le_bytes()); // dev
    put_qid(&mut v, d.qid);
    v.extend_from_slice(&d.mode.to_le_bytes());
    v.extend_from_slice(&d.mtime.to_le_bytes()); // atime
    v.extend_from_slice(&d.mtime.to_le_bytes());
    v.extend_from_slice(&d.length.to_le_bytes());
    for s in [d.name.as_str(), d.uid.as_str(), d.uid.as_str(), d.uid.as_str()] {
        v.extend_from_slice(&(s.len() as u16).to_le_bytes());
        v.extend_from_slice(s.as_bytes());
    }
    let n = (v.len() - 2) as u16;
    v[..2].copy_from_slice(&n.to_le_bytes());
    v
}
