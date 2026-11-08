// SPDX-License-Identifier: MIT
//
// The scene of a ui window: a tree of nodes named by the client, each a
// kind of widget and its properties, changed by ctl's language (one write
// a transaction):
//
//	new id kind [parent [index]]	a node (the parent: root)
//	set id key value		a property (value: the rest of the line)
//	del id				the node and what is under it
//	move id parent [index]
//	title text
//	clear				all but root
//
// kinds: column row label button textbox checkbox progress slider scroll box
// list (a column that scrolls, widgets only for the children seen: long lists)
// image (its picture written to nodes/id/image: PNG or image(6)) select (a
// menu: options a|b|c, selected index) switch divider spinner grid (columns
// n: its children row by row, a table)
// keys: text, placeholder (textbox), checked (checkbox: 0 1), value
// (progress: 0..1, none; slider), min max step (slider), width height (box),
// flex (a child's share of its row or column), gap (between a row's or
// column's children, else the theme's), align (column, row: start center end,
// across: start the default); any kind's look: background border (colours,
// #rrggbb or #rrggbbaa) borderwidth radius padding (pixels) color (text).
// The tree file gives the scene back in the same language.

use std::collections::{BTreeMap, BTreeSet};
use std::sync::Arc;

use crate::image::Picture;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Kind {
    Column,
    Row,
    Label,
    Button,
    TextBox,
    CheckBox,
    Progress,
    Slider,
    Scroll,
    Box,
    List,
    Image,
    Select,
    Switch,
    Divider,
    Spinner,
    Grid,
}

impl Kind {
    pub fn parse(s: &str) -> Option<Kind> {
        Some(match s {
            "column" => Kind::Column,
            "row" => Kind::Row,
            "label" => Kind::Label,
            "button" => Kind::Button,
            "textbox" => Kind::TextBox,
            "checkbox" => Kind::CheckBox,
            "progress" => Kind::Progress,
            "slider" => Kind::Slider,
            "scroll" => Kind::Scroll,
            "box" => Kind::Box,
            "list" => Kind::List,
            "image" => Kind::Image,
            "select" => Kind::Select,
            "switch" => Kind::Switch,
            "divider" => Kind::Divider,
            "spinner" => Kind::Spinner,
            "grid" => Kind::Grid,
            _ => return None,
        })
    }

    pub fn name(self) -> &'static str {
        match self {
            Kind::Column => "column",
            Kind::Row => "row",
            Kind::Label => "label",
            Kind::Button => "button",
            Kind::TextBox => "textbox",
            Kind::CheckBox => "checkbox",
            Kind::Progress => "progress",
            Kind::Slider => "slider",
            Kind::Scroll => "scroll",
            Kind::Box => "box",
            Kind::List => "list",
            Kind::Image => "image",
            Kind::Select => "select",
            Kind::Switch => "switch",
            Kind::Divider => "divider",
            Kind::Spinner => "spinner",
            Kind::Grid => "grid",
        }
    }

    /// how many children it takes
    pub fn holds(self) -> usize {
        match self {
            Kind::Column | Kind::Row | Kind::List | Kind::Grid => usize::MAX,
            Kind::Scroll | Kind::Box => 1,
            _ => 0,
        }
    }
}

#[derive(Clone, Debug)]
pub struct Node {
    pub kind: Kind,
    pub props: BTreeMap<String, String>,
    pub children: Vec<String>,
    pub parent: Option<String>,
}

impl Node {
    pub fn prop(&self, k: &str) -> Option<&str> {
        self.props.get(k).map(|s| s.as_str())
    }

    pub fn num(&self, k: &str) -> Option<f64> {
        self.prop(k).and_then(|v| v.trim().parse().ok())
    }
}

/// what a transaction changed: the tree's shape (structure: the widgets of
/// the rows, columns, boxes and scrolls whose children changed are made to
/// match, the others stay), the nodes made (new, also when an id was there
/// before), or only properties of existing ones
#[derive(Default, Debug)]
pub struct Change {
    pub structure: bool,
    pub created: BTreeSet<String>,
    pub props: Vec<String>, // ids whose properties changed
    pub title: Option<String>,
}

#[derive(Clone)]
pub struct Scene {
    pub nodes: BTreeMap<String, Node>,
    pub title: String,
    pub images: BTreeMap<String, Arc<Picture>>, // image nodes' pictures (nodes/id/image)
}

pub const ROOT: &str = "root";

fn valid_id(id: &str) -> bool {
    !id.is_empty()
        && id.len() <= 64
        && id.chars().all(|c| c.is_ascii_alphanumeric() || c == '_' || c == '-' || c == '.')
}

impl Scene {
    pub fn new() -> Scene {
        let mut nodes = BTreeMap::new();
        nodes.insert(
            ROOT.to_string(),
            Node { kind: Kind::Column, props: BTreeMap::new(), children: Vec::new(), parent: None },
        );
        Scene { nodes, title: "ui".to_string(), images: BTreeMap::new() }
    }

    /// a write of ctl: all its lines or none (an error names the line)
    pub fn apply(&mut self, text: &str) -> Result<Change, String> {
        let mut next = self.clone();
        let mut ch = Change::default();
        for (i, line) in text.lines().enumerate() {
            next.line(line, &mut ch).map_err(|e| format!("line {}: {e}", i + 1))?;
        }
        *self = next;
        Ok(ch)
    }

    fn line(&mut self, line: &str, ch: &mut Change) -> Result<(), String> {
        let line = line.trim_start();
        if line.is_empty() || line.starts_with('#') {
            return Ok(());
        }
        let (verb, rest) = split1(line);
        match verb {
            "new" => {
                let f: Vec<&str> = rest.split_whitespace().collect();
                if f.len() < 2 || f.len() > 4 {
                    return Err("usage: new id kind [parent [index]]".into());
                }
                let (id, kind) = (f[0], Kind::parse(f[1]).ok_or_else(|| format!("unknown kind {}", f[1]))?);
                if !valid_id(id) {
                    return Err(format!("bad id {id:?}"));
                }
                if self.nodes.contains_key(id) {
                    return Err(format!("{id} exists"));
                }
                let parent = f.get(2).copied().unwrap_or(ROOT);
                let index = match f.get(3) {
                    Some(n) => Some(n.parse::<usize>().map_err(|_| format!("bad index {n}"))?),
                    None => None,
                };
                self.nodes.insert(
                    id.to_string(),
                    Node { kind, props: BTreeMap::new(), children: Vec::new(), parent: None },
                );
                self.attach(id, parent, index)?;
                ch.structure = true;
                ch.created.insert(id.to_string());
            }
            "set" => {
                let (id, rest) = split1(rest);
                let (key, value) = split1(rest);
                if key.is_empty() {
                    return Err("usage: set id key value".into());
                }
                let n = self.nodes.get_mut(id).ok_or_else(|| format!("no node {id}"))?;
                n.props.insert(key.to_string(), unquote(value));
                if key == "flex" {
                    ch.structure = true; // the row's or column's share of it
                }
                ch.props.push(id.to_string());
            }
            "del" => {
                let id = rest.trim();
                if id == ROOT {
                    return Err("root stays".into());
                }
                if !self.nodes.contains_key(id) {
                    return Err(format!("no node {id}"));
                }
                self.detach(id);
                self.remove(id);
                ch.structure = true;
            }
            "move" => {
                let f: Vec<&str> = rest.split_whitespace().collect();
                if f.len() < 2 || f.len() > 3 {
                    return Err("usage: move id parent [index]".into());
                }
                if f[0] == ROOT || !self.nodes.contains_key(f[0]) {
                    return Err(format!("cannot move {}", f[0]));
                }
                // not into itself
                let mut p = Some(f[1].to_string());
                while let Some(q) = p {
                    if q == f[0] {
                        return Err(format!("{} would be inside itself", f[0]));
                    }
                    p = self.nodes.get(&q).and_then(|n| n.parent.clone());
                }
                let index = match f.get(2) {
                    Some(n) => Some(n.parse::<usize>().map_err(|_| format!("bad index {n}"))?),
                    None => None,
                };
                self.detach(f[0]);
                self.attach(f[0], f[1], index)?;
                ch.structure = true;
            }
            "title" => {
                self.title = unquote(rest);
                ch.title = Some(self.title.clone());
            }
            "clear" => {
                for c in self.nodes[ROOT].children.clone() {
                    self.remove(&c);
                }
                self.nodes.get_mut(ROOT).unwrap().children.clear();
                ch.structure = true;
            }
            _ => return Err(format!("unknown command {verb}")),
        }
        Ok(())
    }

    fn attach(&mut self, id: &str, parent: &str, index: Option<usize>) -> Result<(), String> {
        let p = self.nodes.get(parent).ok_or_else(|| format!("no node {parent}"))?;
        if p.children.len() >= p.kind.holds() {
            return Err(format!("{parent} ({}) holds no more", p.kind.name()));
        }
        let p = self.nodes.get_mut(parent).unwrap();
        let i = index.unwrap_or(p.children.len()).min(p.children.len());
        p.children.insert(i, id.to_string());
        self.nodes.get_mut(id).unwrap().parent = Some(parent.to_string());
        Ok(())
    }

    fn detach(&mut self, id: &str) {
        if let Some(p) = self.nodes.get(id).and_then(|n| n.parent.clone()) {
            if let Some(pn) = self.nodes.get_mut(&p) {
                pn.children.retain(|c| c != id);
            }
        }
        if let Some(n) = self.nodes.get_mut(id) {
            n.parent = None;
        }
    }

    fn remove(&mut self, id: &str) {
        self.images.remove(id);
        if let Some(n) = self.nodes.remove(id) {
            for c in n.children {
                self.remove(&c);
            }
        }
    }

    /// the scene in ctl's language: written to ctl it makes the scene again
    pub fn dump(&self) -> String {
        let mut s = format!("title {}\n", quote(&self.title));
        self.dump_node(ROOT, &mut s);
        s
    }

    fn dump_node(&self, id: &str, s: &mut String) {
        let n = &self.nodes[id];
        if id != ROOT {
            s.push_str(&format!("new {id} {} {}\n", n.kind.name(), n.parent.as_deref().unwrap_or(ROOT)));
        }
        for (k, v) in &n.props {
            s.push_str(&format!("set {id} {k} {}\n", quote(v)));
        }
        for c in &n.children {
            self.dump_node(c, s);
        }
    }
}

fn split1(s: &str) -> (&str, &str) {
    let s = s.trim_start();
    match s.find(char::is_whitespace) {
        Some(i) => (&s[..i], s[i..].strip_prefix(|c: char| c == ' ' || c == '\t').unwrap_or(&s[i..])),
        None => (s, ""),
    }
}

/// rc's quotes: 'it''s' is it's; anything else as it is
pub fn unquote(v: &str) -> String {
    let t = v.trim_end_matches(['\n', '\r']);
    if t.len() >= 2 && t.starts_with('\'') && t.ends_with('\'') {
        t[1..t.len() - 1].replace("''", "'")
    } else {
        t.to_string()
    }
}

/// quoted as rc would, where it has to be
pub fn quote(v: &str) -> String {
    if !v.is_empty() && !v.starts_with(char::is_whitespace) && !v.ends_with(char::is_whitespace) && !v.starts_with('\'') && !v.contains('\n') {
        v.to_string()
    } else {
        format!("'{}'", v.replace('\'', "''").replace('\n', " "))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn transactions() {
        let mut s = Scene::new();
        s.apply("new t label\nset t text hello world\nnew b button\nset b text 'it''s'\n").unwrap();
        assert_eq!(s.nodes["t"].prop("text"), Some("hello world"));
        assert_eq!(s.nodes["b"].prop("text"), Some("it's"));
        assert!(s.apply("new x label\nset nope text a\n").is_err());
        assert!(!s.nodes.contains_key("x"), "a failed write changes nothing");
        let d = s.dump();
        let mut t = Scene::new();
        t.apply(&d).unwrap();
        assert_eq!(t.dump(), d, "tree, written to ctl, makes the same scene");
        assert!(s.apply("new l label\nnew c label l\n").is_err(), "a label holds nothing");
        s.apply("new r row\nmove b r\n").unwrap();
        assert_eq!(s.nodes["r"].children, vec!["b"]);
        assert!(s.apply("move r r\n").is_err());
        s.apply("del r\n").unwrap();
        assert!(!s.nodes.contains_key("b"));
    }
}
