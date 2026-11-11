// SPDX-License-Identifier: MIT
//
// ui's files (ui(4)):
//
//	ctl		written: a transaction of the scene (scene.rs); read: where
//			the window's contents are on the screen: x y width height
//	event		read: a line an event (click id, text id ..., check id 0|1,
//			slide id v, enter id ..., close); each open from then on;
//			a read waits for one (Tflush ends the wait)
//	tree		read: the scene in ctl's language
//	renderer	read: what draws the window: gpu adapter, or cpu
//	nodes/id/kind	the node's kind
//	nodes/id/props	its properties, "key value" a line; written: set
//	nodes/id/children
//	nodes/id/rect	where it is in the window: x0 y0 x1 y1 (after the last frame)
//	nodes/id/image	an image node's picture, written (PNG or image(6)), taken
//			when the file is closed; read: its width and height
//
// A synchronous server (one thread: ui's), as wsys' is.

use std::collections::{BTreeMap, HashMap, VecDeque};

use crate::ninep::{self, Dir, Qid, R, T, DMDIR, QTDIR};
use crate::scene::{Change, Scene};

const MAXEVENTS: usize = 256;

#[derive(Clone, Debug, PartialEq)]
enum Path {
    Root,
    Ctl,
    Event,
    Tree,
    Renderer,
    Frames,
    Nodes,
    Node(String),
    NodeFile(String, &'static str),
}

struct Fid {
    path: Path,
    open: bool,
    data: Option<Vec<u8>>, // what an open file reads: taken at open
    events: VecDeque<String>,
    picture: Option<Vec<u8>>, // nodes/id/image: what was written, decoded when closed
}

pub struct Fs {
    fids: HashMap<u32, Fid>,
    pending: Vec<(u16, u32, u32)>, // reads of event waiting: tag, fid, count
    nums: BTreeMap<String, u64>,   // nodes' qid paths
    next: u64,
    pub scene: Scene,
    pub changes: Vec<Change>,
    pub size: (u32, u32),
    pub origin: (i32, i32),
    pub renderer: String, // "gpu adapter" or "cpu"
    pub frames: u64,      // the frames drawn (frames: the pace of what moves, for tests)
    pub rects: HashMap<String, [i32; 4]>, // the widgets' places, as the last frame had them
    uid: String,
    msize: u32,
}

const NODEFILES: [&str; 5] = ["kind", "props", "children", "rect", "image"];

impl Fs {
    pub fn new(uid: &str) -> Fs {
        Fs {
            fids: HashMap::new(),
            pending: Vec::new(),
            nums: BTreeMap::new(),
            next: 100,
            scene: Scene::new(),
            changes: Vec::new(),
            size: (0, 0),
            origin: (0, 0),
            renderer: String::new(),
            frames: 0,
            rects: HashMap::new(),
            uid: uid.to_string(),
            msize: 8192 + ninep::IOHDRSZ,
        }
    }

    fn num(&mut self, id: &str) -> u64 {
        if let Some(n) = self.nums.get(id) {
            return *n;
        }
        self.next += 8;
        self.nums.insert(id.to_string(), self.next);
        self.next
    }

    fn qid(&mut self, p: &Path) -> Qid {
        let (ty, path) = match p {
            Path::Root => (QTDIR, 1),
            Path::Ctl => (0, 2),
            Path::Event => (0, 3),
            Path::Tree => (0, 4),
            Path::Renderer => (0, 6),
            Path::Frames => (0, 7),
            Path::Nodes => (QTDIR, 5),
            Path::Node(id) => (QTDIR, self.num(id)),
            Path::NodeFile(id, f) => (0, self.num(id) + 1 + NODEFILES.iter().position(|x| x == f).unwrap() as u64),
        };
        Qid { ty, vers: 0, path }
    }

    fn name(p: &Path) -> String {
        match p {
            Path::Root => "/".into(),
            Path::Ctl => "ctl".into(),
            Path::Event => "event".into(),
            Path::Tree => "tree".into(),
            Path::Renderer => "renderer".into(),
            Path::Frames => "frames".into(),
            Path::Nodes => "nodes".into(),
            Path::Node(id) => id.clone(),
            Path::NodeFile(_, f) => f.to_string(),
        }
    }

    fn dir(&mut self, p: &Path) -> Dir {
        let qid = self.qid(p);
        let mode = match p {
            Path::Root | Path::Nodes | Path::Node(_) => DMDIR | 0o555,
            Path::Ctl | Path::NodeFile(_, "props") => 0o666,
            _ => 0o444,
        };
        Dir { qid, mode, length: 0, name: Self::name(p), uid: self.uid.clone(), mtime: 0 }
    }

    fn exists(&self, p: &Path) -> bool {
        match p {
            Path::Node(id) | Path::NodeFile(id, _) => self.scene.nodes.contains_key(id),
            _ => true,
        }
    }

    fn walk1(&self, p: &Path, name: &str) -> Option<Path> {
        if name == ".." {
            return Some(match p {
                Path::NodeFile(id, _) => Path::Node(id.clone()),
                Path::Node(_) => Path::Nodes,
                _ => Path::Root,
            });
        }
        let q = match p {
            Path::Root => match name {
                "ctl" => Path::Ctl,
                "event" => Path::Event,
                "tree" => Path::Tree,
                "renderer" => Path::Renderer,
                "frames" => Path::Frames,
                "nodes" => Path::Nodes,
                _ => return None,
            },
            Path::Nodes => Path::Node(name.to_string()),
            Path::Node(id) => {
                let f = NODEFILES.iter().find(|f| **f == name)?;
                Path::NodeFile(id.clone(), f)
            }
            _ => return None,
        };
        self.exists(&q).then_some(q)
    }

    fn children(&self, p: &Path) -> Vec<Path> {
        match p {
            Path::Root => vec![Path::Ctl, Path::Event, Path::Tree, Path::Renderer, Path::Frames, Path::Nodes],
            Path::Nodes => self.scene.nodes.keys().map(|k| Path::Node(k.clone())).collect(),
            Path::Node(id) => NODEFILES.iter().map(|f| Path::NodeFile(id.clone(), f)).collect(),
            _ => vec![],
        }
    }

    fn content(&self, p: &Path) -> Vec<u8> {
        match p {
            Path::Ctl => format!("{} {} {} {}\n", self.origin.0, self.origin.1, self.size.0, self.size.1).into_bytes(),
            Path::Tree => self.scene.dump().into_bytes(),
            Path::Renderer => format!("{}\n", self.renderer).into_bytes(),
            Path::Frames => format!("{}\n", self.frames).into_bytes(),
            Path::NodeFile(id, f) => {
                let Some(n) = self.scene.nodes.get(id) else { return vec![] };
                match *f {
                    "kind" => format!("{}\n", n.kind.name()),
                    "props" => n.props.iter().map(|(k, v)| format!("{k} {v}\n")).collect(),
                    "rect" => match self.rects.get(id) {
                        Some(r) => format!("{} {} {} {}\n", r[0], r[1], r[2], r[3]),
                        None => String::new(),
                    },
                    "image" => match self.scene.images.get(id) {
                        Some(p) => format!("{} {}\n", p.width, p.height),
                        None => String::new(),
                    },
                    _ => n.children.iter().map(|c| format!("{c}\n")).collect(),
                }
                .into_bytes()
            }
            _ => vec![],
        }
    }

    /// an event for the clients reading event; their waiting reads answered
    pub fn event(&mut self, line: String) -> Vec<Vec<u8>> {
        for f in self.fids.values_mut() {
            if f.open && f.path == Path::Event {
                if f.events.len() >= MAXEVENTS {
                    f.events.pop_front();
                }
                f.events.push_back(line.clone());
            }
        }
        self.answer()
    }

    fn answer(&mut self) -> Vec<Vec<u8>> {
        let mut out = Vec::new();
        let mut i = 0;
        while i < self.pending.len() {
            let (tag, fid, count) = self.pending[i];
            let Some(f) = self.fids.get_mut(&fid) else {
                self.pending.remove(i);
                continue;
            };
            if f.events.is_empty() {
                i += 1;
                continue;
            }
            // whole lines, as many as fit
            let mut b = Vec::new();
            while let Some(e) = f.events.front() {
                let l = format!("{e}\n");
                if !b.is_empty() && b.len() + l.len() > count as usize {
                    break;
                }
                b.extend_from_slice(&l.as_bytes()[..l.len().min(count as usize)]);
                f.events.pop_front();
            }
            self.pending.remove(i);
            out.push(R::new(117, tag).u32(b.len() as u32).bytes(&b).done());
        }
        out
    }

    /// a message of a client; the answers (none yet for a read that waits)
    pub fn handle(&mut self, m: &[u8]) -> Vec<Vec<u8>> {
        let Some((tag, t)) = ninep::parse(m) else {
            return vec![];
        };
        let r = |e: &str| vec![ninep::error(tag, e)];
        match t {
            T::Version { msize, version } => {
                self.msize = msize.min(65536 + ninep::IOHDRSZ);
                self.fids.clear();
                self.pending.clear();
                let v = if version.starts_with("9P2000") { "9P2000" } else { "unknown" };
                vec![R::new(101, tag).u32(self.msize).str(v).done()]
            }
            T::Auth => r("ui: no authentication"),
            T::Attach { fid, .. } => {
                let q = self.qid(&Path::Root);
                self.fids.insert(fid, Fid { path: Path::Root, open: false, data: None, events: VecDeque::new(), picture: None });
                vec![R::new(105, tag).qid(q).done()]
            }
            T::Flush { oldtag } => {
                // a waiting read let go: answered no more
                self.pending.retain(|p| p.0 != oldtag);
                vec![R::new(109, tag).done()]
            }
            T::Walk { fid, newfid, names } => {
                let Some(f) = self.fids.get(&fid) else { return r("unknown fid") };
                if f.open {
                    return r("walk of an open file");
                }
                let mut p = f.path.clone();
                let mut qids = Vec::new();
                for n in &names {
                    match self.walk1(&p, n) {
                        Some(q) => {
                            p = q;
                            qids.push(self.qid(&p));
                        }
                        None => break,
                    }
                }
                if qids.len() < names.len() && qids.is_empty() && !names.is_empty() {
                    return r("file does not exist");
                }
                if qids.len() == names.len() {
                    self.fids.insert(newfid, Fid { path: p, open: false, data: None, events: VecDeque::new(), picture: None });
                }
                let mut rr = R::new(111, tag).u16(qids.len() as u16);
                for q in qids {
                    rr = rr.qid(q);
                }
                vec![rr.done()]
            }
            T::Open { fid, mode } => {
                let Some(f) = self.fids.get(&fid) else { return r("unknown fid") };
                let p = f.path.clone();
                if !self.exists(&p) {
                    return r("file does not exist");
                }
                let write = mode & 3 != 0;
                let writable = matches!(p, Path::Ctl | Path::NodeFile(_, "props") | Path::NodeFile(_, "image"));
                if write && !writable {
                    return r("permission denied");
                }
                let data = (!matches!(p, Path::Event)).then(|| self.content(&p));
                let q = self.qid(&p);
                let f = self.fids.get_mut(&fid).unwrap();
                f.open = true;
                f.data = data;
                if write && matches!(p, Path::NodeFile(_, "image")) {
                    f.picture = Some(Vec::new());
                }
                vec![R::new(113, tag).qid(q).u32(8192).done()]
            }
            T::Create => r("ui: no creating files"),
            T::Read { fid, offset, count } => {
                let count = count.min(self.msize - ninep::IOHDRSZ);
                let Some(f) = self.fids.get(&fid) else { return r("unknown fid") };
                let p = f.path.clone();
                match &p {
                    Path::Root | Path::Nodes | Path::Node(_) => {
                        // directory entries, whole, from offset (as stat(5) packs them)
                        let mut all = Vec::new();
                        for c in self.children(&p) {
                            let d = self.dir(&c);
                            all.extend_from_slice(&ninep::stat(&d));
                        }
                        let mut b = Vec::new();
                        let mut o = 0usize;
                        let mut i = 0usize;
                        while i + 2 <= all.len() {
                            let n = u16::from_le_bytes([all[i], all[i + 1]]) as usize + 2;
                            if o as u64 >= offset {
                                if b.len() + n > count as usize {
                                    break;
                                }
                                b.extend_from_slice(&all[i..i + n]);
                            }
                            o += n;
                            i += n;
                        }
                        vec![R::new(117, tag).u32(b.len() as u32).bytes(&b).done()]
                    }
                    Path::Event => {
                        self.pending.push((tag, fid, count));
                        self.answer()
                    }
                    _ => {
                        let d = f.data.as_deref().unwrap_or(&[]);
                        let o = (offset as usize).min(d.len());
                        let e = (o + count as usize).min(d.len());
                        vec![R::new(117, tag).u32((e - o) as u32).bytes(&d[o..e]).done()]
                    }
                }
            }
            T::Write { fid, offset, data } => {
                let Some(f) = self.fids.get_mut(&fid) else { return r("unknown fid") };
                if let Some(pic) = f.picture.as_mut() {
                    // at its offset; the picture taken when the file is closed
                    let o = offset as usize;
                    if o > 64 << 20 {
                        return r("picture too big");
                    }
                    if pic.len() < o + data.len() {
                        pic.resize(o + data.len(), 0);
                    }
                    pic[o..o + data.len()].copy_from_slice(&data);
                    return vec![R::new(119, tag).u32(data.len() as u32).done()];
                }
                let text = String::from_utf8_lossy(&data).into_owned();
                let cmds = match &f.path {
                    Path::Ctl => text,
                    Path::NodeFile(id, "props") => {
                        let id = id.clone();
                        text.lines()
                            .filter(|l| !l.trim().is_empty())
                            .map(|l| format!("set {id} {l}\n"))
                            .collect()
                    }
                    _ => return r("permission denied"),
                };
                match self.scene.apply(&cmds) {
                    Ok(ch) => {
                        self.changes.push(ch);
                        vec![R::new(119, tag).u32(data.len() as u32).done()]
                    }
                    Err(e) => r(&e),
                }
            }
            T::Clunk { fid } => {
                let f = self.fids.remove(&fid);
                self.pending.retain(|p| p.1 != fid);
                // an image node's picture, written: decoded now
                if let Some(Fid { path: Path::NodeFile(id, _), picture: Some(b), .. }) = f {
                    match crate::image::decode(&b) {
                        Ok(p) if self.scene.nodes.contains_key(&id) => {
                            self.scene.images.insert(id.clone(), std::sync::Arc::new(p));
                            self.changes.push(Change { props: vec![id], ..Default::default() });
                        }
                        Ok(_) => return r("node gone"),
                        Err(e) => return r(&e),
                    }
                }
                vec![R::new(121, tag).done()]
            }
            T::Remove { fid } => {
                self.fids.remove(&fid);
                r("ui: no removing files")
            }
            T::Stat { fid } => {
                let Some(f) = self.fids.get(&fid) else { return r("unknown fid") };
                let p = f.path.clone();
                if !self.exists(&p) {
                    return r("file does not exist");
                }
                let d = self.dir(&p);
                let st = ninep::stat(&d);
                vec![R::new(125, tag).u16(st.len() as u16).bytes(&st).done()]
            }
            T::Wstat => r("ui: no wstat"),
        }
    }
}
