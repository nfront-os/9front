// SPDX-License-Identifier: MIT
//
// uiview: a window of ui(4) from the program's state, as Xilem makes
// Masonry's widgets from it. A function of the state gives a tree of views
// (label, button, column, ...); after each event it is called again and the
// new tree compared with the old one (xilem_core's View): what changed goes
// to ui as ctl's lines, all of one event in one write (a transaction). The
// widgets and their state (focus, the text being edited) stay in ui; the
// program holds only its own state.
//
//	fn app(n: &mut i32) -> impl UiView<i32> {
//		column((label(format!("{n}")), button("add one", |n: &mut i32| *n += 1)))
//	}
//	uiview::run("count", 0, app)
//
// Each view's node gets an id of its own (n1, n2, ...), events name it and
// go to the view that made it (its path of ViewIds).

use std::cell::{Cell, RefCell};
use std::collections::{BTreeMap, HashMap};
use std::marker::PhantomData;
use std::rc::Rc;

pub use xilem_core;
use xilem_core::{
    AnyElement, AnyView, AppendVec, DynMessage, ElementSplice, Environment, MessageCtx,
    MessageResult, Mut, SuperElement, View, ViewElement, ViewId, ViewMarker, ViewPathTracker,
    ViewSequence,
};

#[cfg(target_os = "plan9")]
mod conn;
#[cfg(target_os = "plan9")]
pub use conn::{Proxy, Ui, run, run_on, run_on_with, run_with};

/// what the views write and whom events go to, shared by the nodes (a
/// replaced node writes where its successor goes)
#[derive(Default)]
struct Shared {
    tx: RefCell<String>,
    next: Cell<u64>,
    paths: RefCell<HashMap<String, Vec<ViewId>>>,
}

impl Shared {
    fn line(&self, s: &str) {
        let mut t = self.tx.borrow_mut();
        t.push_str(s);
        t.push('\n');
    }
}

/// The context of building views: their path, the node they go into, the
/// transaction being made.
pub struct Cx {
    path: Vec<ViewId>,
    env: Environment,
    parents: Vec<String>,
    sh: Rc<Shared>,
}

impl ViewPathTracker for Cx {
    fn environment(&mut self) -> &mut Environment {
        &mut self.env
    }
    fn push_id(&mut self, id: ViewId) {
        self.path.push(id);
    }
    fn pop_id(&mut self) {
        self.path.pop();
    }
    fn view_path(&mut self) -> &[ViewId] {
        &self.path
    }
}

impl Default for Cx {
    fn default() -> Self {
        Cx::new()
    }
}

impl Cx {
    pub fn new() -> Cx {
        Cx { path: Vec::new(), env: Environment::new(), parents: Vec::new(), sh: Rc::default() }
    }

    /// the lines made since the last call: one transaction of ctl
    pub fn take(&mut self) -> String {
        std::mem::take(&mut *self.sh.tx.borrow_mut())
    }

    fn parent(&self) -> &str {
        self.parents.last().map(|s| s.as_str()).unwrap_or(ROOT)
    }

    /// a new node, in the node being built
    fn node(&mut self, kind: &str) -> Node {
        let n = self.sh.next.get() + 1;
        self.sh.next.set(n);
        let id = format!("n{n}");
        self.sh.line(&format!("new {id} {kind} {}", self.parent()));
        self.sh.paths.borrow_mut().insert(id.clone(), self.path.clone());
        Node { id }
    }

    /// whose event this is: the path of the view that made the node
    pub fn path_of(&self, id: &str) -> Option<Vec<ViewId>> {
        self.sh.paths.borrow().get(id).cloned()
    }

    /// an event for the view of node id; Err when its node is gone
    pub fn message<S: 'static, A, V: View<S, A, Cx, Element = Node>>(
        &mut self,
        view: &V,
        state: &mut V::ViewState,
        root: &mut Node,
        app: &mut S,
        ev: Event,
    ) -> MessageResult<A> {
        let Some(path) = self.path_of(&ev.id) else {
            return MessageResult::Stale;
        };
        let env = std::mem::replace(&mut self.env, Environment::new());
        let mut m = MessageCtx::new(env, path, DynMessage::new(ev));
        let r = view.message(state, &mut m, root_mut(root, &self.sh), app);
        self.env = m.finish().0;
        r
    }

    /// the views again, after the state changed
    pub fn rebuild<S: 'static, A, V: View<S, A, Cx, Element = Node>>(
        &mut self,
        view: &V,
        prev: &V,
        state: &mut V::ViewState,
        root: &mut Node,
        app: &mut S,
    ) {
        let sh = self.sh.clone();
        view.rebuild(prev, state, self, root_mut(root, &sh), app);
    }
}

pub const ROOT: &str = "root";

fn root_mut<'a>(root: &'a mut Node, sh: &'a Rc<Shared>) -> NodeMut<'a> {
    NodeMut { node: root, parent: ROOT, idx: 0, sh }
}

/// A node of ui's tree, made by a view.
#[derive(Debug)]
pub struct Node {
    pub id: String,
}

/// A node as its parent holds it: where it is (a node put in its place goes
/// there).
pub struct NodeMut<'a> {
    pub node: &'a mut Node,
    parent: &'a str,
    idx: usize,
    sh: &'a Rc<Shared>,
}

impl NodeMut<'_> {
    pub fn reborrow(&mut self) -> NodeMut<'_> {
        NodeMut { node: self.node, parent: self.parent, idx: self.idx, sh: self.sh }
    }
}

impl ViewElement for Node {
    type Mut<'a> = NodeMut<'a>;
}

impl SuperElement<Node, Cx> for Node {
    fn upcast(_cx: &mut Cx, child: Node) -> Node {
        child
    }
    fn with_downcast_val<R>(mut this: Mut<'_, Self>, f: impl FnOnce(Mut<'_, Node>) -> R) -> (Mut<'_, Self>, R) {
        let r = f(this.reborrow());
        (this, r)
    }
}

// a view of another type in its place (AnyView): the old one is torn down
// (its node deleted), the new one made at the end of the parent; it goes
// where the old one was
impl AnyElement<Node, Cx> for Node {
    fn replace_inner(this: Mut<'_, Self>, child: Node) -> Mut<'_, Self> {
        replace(this, child)
    }
}

/// n where the torn down node of this was
fn replace(this: NodeMut<'_>, n: Node) -> NodeMut<'_> {
    this.sh.line(&format!("move {} {} {}", n.id, this.parent, this.idx));
    *this.node = n;
    this
}

/// Views of ui's widgets, as an argument's or a function's type.
pub trait UiView<S: 'static, A = ()>: View<S, A, Cx, Element = Node> {}
impl<V, S: 'static, A> UiView<S, A> for V where V: View<S, A, Cx, Element = Node> {}

/// A view of one of several types, chosen at run time.
pub type AnyUiView<S, A = ()> = Box<dyn AnyView<S, A, Cx, Node>>;

/// Children of a row, column or box: a view, a tuple, a Vec, an Option.
pub trait UiSeq<S, A = ()>: ViewSequence<S, A, Cx, Node> {}
impl<Seq, S, A> UiSeq<S, A> for Seq where Seq: ViewSequence<S, A, Cx, Node> {}

/// What the user did to a node: a line of ui's event file.
#[derive(Debug, Clone, PartialEq)]
pub struct Event {
    pub kind: String,
    pub id: String,
    pub arg: String,
}

impl Event {
    /// "kind id arg"; None for close and what has no id
    pub fn parse(line: &str) -> Option<Event> {
        let line = line.trim_end_matches('\n');
        let (kind, rest) = line.split_once(' ')?;
        let (id, arg) = rest.split_once(' ').unwrap_or((rest, ""));
        Some(Event { kind: kind.into(), id: id.into(), arg: arg.into() })
    }
}

/// rc's quotes where a value needs them (scene.rs of ui unquotes them)
pub(crate) fn quote(v: &str) -> String {
    if !v.is_empty()
        && !v.starts_with(char::is_whitespace)
        && !v.ends_with(char::is_whitespace)
        && !v.starts_with('\'')
        && !v.contains('\n')
    {
        v.to_string()
    } else {
        format!("'{}'", v.replace('\'', "''").replace('\n', " "))
    }
}

/// the properties ui has of a node, as this side knows them: sent when they
/// differ (what the user changed comes back by events, and is not sent again)
#[derive(Default, Debug)]
pub struct Known(BTreeMap<&'static str, String>);

impl Known {
    fn send(&mut self, sh: &Shared, id: &str, props: &[(&'static str, String)]) {
        for (k, v) in props {
            if self.0.get(k) != Some(v) {
                sh.line(&format!("set {id} {k} {}", quote(v)));
                self.0.insert(k, v.clone());
            }
        }
    }
}

type Handler<S, A> = Box<dyn Fn(&mut S, &str) -> A>;

/// A widget without children: label, button, text box, check box,
/// progress bar, slider.
pub struct Widget<S, A = ()> {
    kind: &'static str,
    props: Vec<(&'static str, String)>,
    on: Vec<(&'static str, Handler<S, A>)>,
}

impl<S, A> Widget<S, A> {
    fn new(kind: &'static str) -> Self {
        Widget { kind, props: Vec::new(), on: Vec::new() }
    }

    /// a property of ui(4)'s (set id key value)
    pub fn prop(mut self, key: &'static str, value: impl ToString) -> Self {
        let v = value.to_string();
        match self.props.iter_mut().find(|(k, _)| *k == key) {
            Some(p) => p.1 = v,
            None => self.props.push((key, v)),
        }
        self
    }

    /// its share of the row or column it is in
    pub fn flex(self, f: f64) -> Self {
        self.prop("flex", f)
    }

    fn on(mut self, kind: &'static str, f: impl Fn(&mut S, &str) -> A + 'static) -> Self {
        self.on.push((kind, Box::new(f)));
        self
    }
}

impl<S: 'static, A: 'static> Widget<S, A> {
    /// a text box: what is typed, when enter is pressed
    pub fn on_enter(self, f: impl Fn(&mut S, String) -> A + 'static) -> Self {
        self.on("enter", move |s, a| f(s, a.to_string()))
    }

    pub fn placeholder(self, p: impl ToString) -> Self {
        self.prop("placeholder", p)
    }

    /// a slider's steps
    pub fn step(self, s: f64) -> Self {
        self.prop("step", s)
    }
}

pub fn label<S, A>(text: impl ToString) -> Widget<S, A> {
    Widget::new("label").prop("text", text)
}

pub fn button<S: 'static, A: 'static>(text: impl ToString, f: impl Fn(&mut S) -> A + 'static) -> Widget<S, A> {
    Widget::new("button").prop("text", text).on("click", move |s, _| f(s))
}

/// a text box showing text; f gets each change the user makes
pub fn textbox<S: 'static, A: 'static>(text: impl ToString, f: impl Fn(&mut S, String) -> A + 'static) -> Widget<S, A> {
    Widget::new("textbox").prop("text", text).on("text", move |s, a| f(s, a.to_string()))
}

pub fn checkbox<S: 'static, A: 'static>(
    text: impl ToString,
    checked: bool,
    f: impl Fn(&mut S, bool) -> A + 'static,
) -> Widget<S, A> {
    Widget::new("checkbox")
        .prop("text", text)
        .prop("checked", if checked { 1 } else { 0 })
        .on("check", move |s, a| f(s, a.trim() == "1"))
}

/// done so far, 0 to 1; None: not known
pub fn progress<S, A>(done: Option<f64>) -> Widget<S, A> {
    Widget::new("progress").prop("value", done.map(|d| d.to_string()).unwrap_or("none".into()))
}

pub fn slider<S: 'static, A: 'static>(
    min: f64,
    max: f64,
    value: f64,
    f: impl Fn(&mut S, f64) -> A + 'static,
) -> Widget<S, A> {
    Widget::new("slider")
        .prop("min", min)
        .prop("max", max)
        .prop("value", value)
        .on("slide", move |s, a| f(s, a.trim().parse().unwrap_or(0.0)))
}

/// a switch, on or off; f gets each change the user makes
pub fn switch<S: 'static, A: 'static>(on: bool, f: impl Fn(&mut S, bool) -> A + 'static) -> Widget<S, A> {
    Widget::new("switch").prop("checked", if on { 1 } else { 0 }).on("check", move |s, a| f(s, a.trim() == "1"))
}

/// a menu of options, the one at selected chosen; f gets the index the user picks
pub fn select<S: 'static, A: 'static>(
    options: &[&str],
    selected: usize,
    f: impl Fn(&mut S, usize) -> A + 'static,
) -> Widget<S, A> {
    Widget::new("select")
        .prop("options", options.join("|"))
        .prop("selected", selected)
        .on("select", move |s, a| f(s, a.split_whitespace().next().and_then(|i| i.parse().ok()).unwrap_or(0)))
}

/// a line between widgets (vertical: in a row)
pub fn divider<S, A>(vertical: bool) -> Widget<S, A> {
    let d = Widget::new("divider");
    if vertical { d.prop("axis", "vertical") } else { d }
}

/// something is being done
pub fn spinner<S, A>() -> Widget<S, A> {
    Widget::new("spinner")
}

/// what the user's change made of a property: ui has it already
fn learn(known: &mut Known, ev: &Event) {
    let (key, v) = match ev.kind.as_str() {
        "text" | "enter" => ("text", ev.arg.clone()),
        "check" => ("checked", ev.arg.clone()),
        "slide" => ("value", ev.arg.clone()),
        "select" => ("selected", ev.arg.split_whitespace().next().unwrap_or("0").to_string()),
        _ => return,
    };
    known.0.insert(key, v);
}

impl<S, A> ViewMarker for Widget<S, A> {}
impl<S: 'static, A: 'static> View<S, A, Cx> for Widget<S, A> {
    type Element = Node;
    type ViewState = Known;

    fn build(&self, cx: &mut Cx, _: &mut S) -> (Node, Known) {
        let n = cx.node(self.kind);
        let mut k = Known::default();
        k.send(&cx.sh, &n.id, &self.props);
        (n, k)
    }

    fn rebuild(&self, prev: &Self, k: &mut Known, cx: &mut Cx, el: NodeMut<'_>, s: &mut S) {
        if prev.kind != self.kind {
            // another widget: a node of its own, where the old one was
            prev.teardown(k, cx, NodeMut { node: el.node, parent: el.parent, idx: el.idx, sh: el.sh });
            let (n, nk) = self.build(cx, s);
            replace(el, n);
            *k = nk;
            return;
        }
        k.send(&cx.sh, &el.node.id, &self.props);
    }

    fn teardown(&self, _: &mut Known, cx: &mut Cx, el: NodeMut<'_>) {
        cx.sh.line(&format!("del {}", el.node.id));
        cx.sh.paths.borrow_mut().remove(&el.node.id);
    }

    fn message(&self, k: &mut Known, m: &mut MessageCtx, _: NodeMut<'_>, s: &mut S) -> MessageResult<A> {
        if m.take_first().is_some() {
            return MessageResult::Stale;
        }
        let Some(ev) = m.take_message::<Event>() else {
            return MessageResult::Stale;
        };
        learn(k, &ev);
        match self.on.iter().find(|(kind, _)| *kind == ev.kind) {
            Some((_, f)) => MessageResult::Action(f(s, &ev.arg)),
            None => MessageResult::Nop,
        }
    }
}

/// A widget holding others: a row, a column, a scrolled box, a box.
pub struct Container<Seq, S, A = ()> {
    kind: &'static str,
    props: Vec<(&'static str, String)>,
    seq: Seq,
    phantom: PhantomData<fn() -> (S, A)>,
}

impl<Seq, S, A> Container<Seq, S, A> {
    pub fn prop(mut self, key: &'static str, value: impl ToString) -> Self {
        let v = value.to_string();
        match self.props.iter_mut().find(|(k, _)| *k == key) {
            Some(p) => p.1 = v,
            None => self.props.push((key, v)),
        }
        self
    }

    pub fn flex(self, f: f64) -> Self {
        self.prop("flex", f)
    }

    /// pixels between the children
    pub fn gap(self, g: f64) -> Self {
        self.prop("gap", g)
    }

    /// across the row or column: start, center, end
    pub fn align(self, a: &str) -> Self {
        self.prop("align", a)
    }

    /// a box's size
    pub fn width(self, w: f64) -> Self {
        self.prop("width", w)
    }

    pub fn height(self, h: f64) -> Self {
        self.prop("height", h)
    }
}

fn container<Seq, S, A>(kind: &'static str, seq: Seq) -> Container<Seq, S, A> {
    Container { kind, props: Vec::new(), seq, phantom: PhantomData }
}

pub fn column<S, A, Seq: UiSeq<S, A>>(seq: Seq) -> Container<Seq, S, A> {
    container("column", seq)
}

pub fn row<S, A, Seq: UiSeq<S, A>>(seq: Seq) -> Container<Seq, S, A> {
    container("row", seq)
}

/// children in a grid of columns, row by row (a table)
pub fn grid<S, A, Seq: UiSeq<S, A>>(columns: usize, seq: Seq) -> Container<Seq, S, A> {
    let mut c = container("grid", seq);
    c.props.push(("columns", columns.to_string()));
    c
}

/// children in a column that scrolls, widgets only for those seen (long lists)
pub fn list<S, A, Seq: UiSeq<S, A>>(seq: Seq) -> Container<Seq, S, A> {
    container("list", seq)
}

/// one child, scrolled
pub fn scroll<S: 'static, A, V: UiView<S, A>>(child: V) -> Container<V, S, A> {
    container("scroll", child)
}

/// one child or none, in a box of its size
pub fn sized<S, A, Seq: UiSeq<S, A>>(child: Seq) -> Container<Seq, S, A> {
    container("box", child)
}

pub struct ContainerState<SeqState> {
    known: Known,
    seq: SeqState,
    children: Vec<Node>,
    scratch: AppendVec<Node>,
}

/// the children of a node, as a sequence of views changes them
struct Splice<'a> {
    parent: &'a str,
    children: &'a mut Vec<Node>,
    scratch: &'a mut AppendVec<Node>,
    idx: usize,
    sh: &'a Rc<Shared>,
}

impl Splice<'_> {
    // a new node: made at the parent's end, moved to where it goes
    fn put(&mut self, n: Node) {
        if self.idx < self.children.len() {
            self.sh.line(&format!("move {} {} {}", n.id, self.parent, self.idx));
        }
        self.children.insert(self.idx, n);
        self.idx += 1;
    }
}

impl ElementSplice<Node> for Splice<'_> {
    fn with_scratch<R>(&mut self, f: impl FnOnce(&mut AppendVec<Node>) -> R) -> R {
        let r = f(self.scratch);
        let new: Vec<Node> = self.scratch.drain().collect();
        for n in new {
            self.put(n);
        }
        r
    }

    fn insert(&mut self, n: Node) {
        self.put(n);
    }

    fn mutate<R>(&mut self, f: impl FnOnce(NodeMut<'_>) -> R) -> R {
        let r = f(NodeMut { node: &mut self.children[self.idx], parent: self.parent, idx: self.idx, sh: self.sh });
        self.idx += 1;
        r
    }

    fn skip(&mut self, n: usize) {
        self.idx += n;
    }

    fn index(&self) -> usize {
        self.idx
    }

    // the view's teardown deleted the node
    fn delete<R>(&mut self, f: impl FnOnce(NodeMut<'_>) -> R) -> R {
        let r = f(NodeMut { node: &mut self.children[self.idx], parent: self.parent, idx: self.idx, sh: self.sh });
        self.children.remove(self.idx);
        r
    }
}

impl<Seq, S, A> ViewMarker for Container<Seq, S, A> {}
impl<Seq, S: 'static, A: 'static> View<S, A, Cx> for Container<Seq, S, A>
where
    Seq: UiSeq<S, A>,
{
    type Element = Node;
    type ViewState = ContainerState<Seq::SeqState>;

    fn build(&self, cx: &mut Cx, s: &mut S) -> (Node, Self::ViewState) {
        let n = cx.node(self.kind);
        let mut known = Known::default();
        known.send(&cx.sh, &n.id, &self.props);
        let mut el = AppendVec::default();
        cx.parents.push(n.id.clone());
        let seq = self.seq.seq_build(cx, &mut el, s);
        cx.parents.pop();
        let children = el.into_inner();
        (n, ContainerState { known, seq, children, scratch: AppendVec::default() })
    }

    fn rebuild(&self, prev: &Self, st: &mut Self::ViewState, cx: &mut Cx, el: NodeMut<'_>, s: &mut S) {
        if prev.kind != self.kind {
            prev.teardown(st, cx, NodeMut { node: el.node, parent: el.parent, idx: el.idx, sh: el.sh });
            let (n, nst) = self.build(cx, s);
            replace(el, n);
            *st = nst;
            return;
        }
        let id = el.node.id.clone();
        st.known.send(&cx.sh, &id, &self.props);
        let sh = cx.sh.clone();
        let mut sp = Splice { parent: &id, children: &mut st.children, scratch: &mut st.scratch, idx: 0, sh: &sh };
        cx.parents.push(id.clone());
        self.seq.seq_rebuild(&prev.seq, &mut st.seq, cx, &mut sp, s);
        cx.parents.pop();
    }

    // the children first: each deletes its node, then this one
    fn teardown(&self, st: &mut Self::ViewState, cx: &mut Cx, el: NodeMut<'_>) {
        let id = el.node.id.clone();
        let sh = cx.sh.clone();
        let mut sp = Splice { parent: &id, children: &mut st.children, scratch: &mut st.scratch, idx: 0, sh: &sh };
        self.seq.seq_teardown(&mut st.seq, cx, &mut sp);
        cx.sh.line(&format!("del {id}"));
        cx.sh.paths.borrow_mut().remove(&id);
    }

    fn message(&self, st: &mut Self::ViewState, m: &mut MessageCtx, el: NodeMut<'_>, s: &mut S) -> MessageResult<A> {
        let id = el.node.id.clone();
        let mut sp = Splice { parent: &id, children: &mut st.children, scratch: &mut st.scratch, idx: 0, sh: el.sh };
        self.seq.seq_message(&mut st.seq, m, &mut sp, s)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct St {
        n: i32,
        items: Vec<String>,
        fancy: bool,
        text: String,
    }

    fn app(s: &mut St) -> impl UiView<St> + use<> {
        let items: Vec<_> = s.items.iter().map(|t| label(t.clone())).collect();
        let last: AnyUiView<St> = if s.n < 0 {
            Box::new(row(label("minus")))
        } else if s.fancy {
            Box::new(button("fancy", |_: &mut St| {}))
        } else {
            Box::new(label("plain"))
        };
        column((
            label(format!("count {}", s.n)),
            button("add", |s: &mut St| {
                s.n += 1;
                s.items.push(format!("item {}", s.n));
            }),
            textbox(&s.text, |s: &mut St, t| s.text = t),
            column(items).gap(4.0),
            last,
        ))
    }

    #[test]
    fn views_make_transactions() {
        let mut s = St { n: 0, items: vec![], fancy: false, text: String::new() };
        let mut cx = Cx::new();
        let mut v = app(&mut s);
        let (mut root, mut vs) = v.build(&mut cx, &mut s);
        let t = cx.take();
        assert!(t.starts_with("new n1 column root\nnew n2 label n1\nset n2 text count 0\nnew n3 button n1\n"), "{t}");
        assert!(t.contains("new n5 column n1\nset n5 gap 4\nnew n6 label n1\nset n6 text plain\n"), "{t}");

        // the button pressed: the label changes, an item comes
        let r = cx.message(&v, &mut vs, &mut root, &mut s, Event::parse("click n3").unwrap());
        assert!(matches!(r, MessageResult::Action(())));
        assert_eq!(s.n, 1);
        let next = app(&mut s);
        cx.rebuild(&next, &v, &mut vs, &mut root, &mut s);
        v = next;
        assert_eq!(cx.take(), "set n2 text count 1\nnew n7 label n5\nset n7 text item 1\n");

        // typed into the text box: ui has the text, nothing goes back
        // (a view that keeps its own text would set it again)
        cx.message(&v, &mut vs, &mut root, &mut s, Event::parse("text n4 hello").unwrap());
        let next = app(&mut s);
        cx.rebuild(&next, &v, &mut vs, &mut root, &mut s);
        v = next;
        assert_eq!(cx.take(), "");

        // another widget in the last place: the old node goes, the new one is moved there
        s.fancy = true;
        let next = app(&mut s);
        cx.rebuild(&next, &v, &mut vs, &mut root, &mut s);
        v = next;
        assert_eq!(cx.take(), "del n6\nnew n8 button n1\nset n8 text fancy\nmove n8 n1 4\n");

        // a view of another type (AnyView)
        s.items.push("x".into());
        s.fancy = false;
        s.n = -1;
        let next = app(&mut s);
        cx.rebuild(&next, &v, &mut vs, &mut root, &mut s);
        v = next;
        assert_eq!(
            cx.take(),
            "set n2 text count -1\nnew n9 label n5\nset n9 text x\n\
             del n8\nnew n10 row n1\nnew n11 label n10\nset n11 text minus\nmove n10 n1 4\n"
        );
        s.items = vec!["item 1".into()];
        s.n = 1;
        let next = app(&mut s);
        cx.rebuild(&next, &v, &mut vs, &mut root, &mut s);
        v = next;
        assert_eq!(
            cx.take(),
            "set n2 text count 1\ndel n9\n\
             del n11\ndel n10\nnew n12 label n1\nset n12 text plain\nmove n12 n1 4\n"
        );

        // an item removed
        s.items.clear();
        let next = app(&mut s);
        cx.rebuild(&next, &v, &mut vs, &mut root, &mut s);
        assert_eq!(cx.take(), "del n7\n");
        assert!(cx.path_of("n7").is_none());
        // an event of a node gone: stale
        let r = cx.message(&next, &mut vs, &mut root, &mut s, Event::parse("click n7").unwrap());
        assert!(matches!(r, MessageResult::Stale));
    }
}
