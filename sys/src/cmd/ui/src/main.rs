// SPDX-License-Identifier: MIT
//
// ui: a window whose contents a client describes as a tree of widgets,
// through files (ui(4)), as acme's windows are: the client writes the scene
// (ctl), reads what the user did (event); ui keeps the widgets, their state
// (focus, text being edited, scrolling), lays them out, draws them.  The
// widgets are Masonry's (linebender), drawn with the GPU (vello on wgpu's
// Vulkan: gpu.rs) or, without one, on the processor (vello_cpu into the
// window through softbuffer; winit and softbuffer with their plan9
// backends): VESA, arm64 alike.  A client may be on another machine: the
// files are 9P, posted in /srv.
//
// usage: ui [-p] [-s srvname]	(default ui.$user.$pid; the name on stdout)
//	-p: on the processor, also with a GPU

mod fs;
#[cfg(feature = "gpu")]
mod gpu;
mod ninep;
mod scene;

use std::collections::{BTreeSet, HashMap};
use std::num::NonZeroU32;
use std::sync::{mpsc, Arc};

use masonry::app::{RenderRoot, RenderRootOptions, RenderRootSignal, VisualLayerKind, WindowSizePolicy};
use masonry::core::{CollectionWidget, ErasedAction, NewWidget, TextEvent, Widget, WidgetId, WindowEvent};
use masonry::peniko::Blob;
use masonry::properties::{Gap, Padding};
use masonry::properties::types::CrossAxisAlignment;
use masonry::layout::Length;
use masonry::widgets::{
    Button, ButtonPress, Checkbox, CheckboxToggled, Flex, Label, Portal, ProgressBar, SizedBox, Slider,
    SliderMoved, TextAction, TextArea, TextInput, VirtualScroll, VirtualScrollAction,
};
use masonry_imaging::{Layer as ImagingLayer, PreparedFrame};
use imaging::render::{ImageBufferFormat, ImageBufferTarget, ImageRenderer};
use p9win::sys::{Fd, OREAD};
use ui_events_winit::{WindowEventReducer, WindowEventTranslation};
use winit::application::ApplicationHandler;
use winit::dpi::PhysicalSize;
use winit::event::WindowEvent as WinitWindowEvent;
use winit::event_loop::{ActiveEventLoop, EventLoop, EventLoopProxy};
use winit::window::{Window, WindowId};

use fs::Fs;
use scene::{Kind, Scene, ROOT};

/// what comes to the window's loop from elsewhere
enum UserEvent {
    NineP(Vec<u8>), // a client's message
    Hangup,         // the clients' connection ended
}

/// the fonts of the system: /lib/font/ttf (Plan 9's fonts are files)
fn fonts() -> Vec<Vec<u8>> {
    let mut v = Vec::new();
    if let Ok(d) = std::fs::read_dir("/lib/font/ttf") {
        let mut names: Vec<_> = d.flatten().map(|e| e.path()).collect();
        names.sort();
        for p in names {
            let ext = p.extension().and_then(|e| e.to_str()).unwrap_or("");
            if ext.eq_ignore_ascii_case("ttf") || ext.eq_ignore_ascii_case("otf") {
                if let Ok(b) = std::fs::read(&p) {
                    v.push(b);
                }
            }
        }
    }
    v
}

/// the scene's widgets: which node a widget is, which widget a node, the
/// children a row's, column's, scroll's or box's widget has (sync)
#[derive(Default)]
struct Ids {
    node: HashMap<WidgetId, String>,
    widget: HashMap<String, WidgetId>,
    shown: HashMap<String, Vec<String>>,
    // a list's children that have widgets: by index, the node and its widget
    active: HashMap<String, std::collections::BTreeMap<usize, (String, WidgetId)>>,
}

impl Ids {
    /// node id's widget wid gone, and the widgets under it (the scene's nodes
    /// below it), unless the node has another widget meanwhile (moved)
    fn forget(&mut self, sc: &Scene, id: &str, wid: WidgetId) {
        if self.widget.get(id) != Some(&wid) {
            return;
        }
        let mut todo = vec![id.to_string()];
        while let Some(n) = todo.pop() {
            if let Some(w) = self.widget.remove(&n) {
                self.node.retain(|k, v| *k != w && *v != n);
            }
            self.shown.remove(&n);
            self.active.remove(&n);
            if let Some(node) = sc.nodes.get(&n) {
                todo.extend(node.children.iter().cloned());
            }
        }
    }
}

fn len(v: Option<f64>) -> Option<Length> {
    v.filter(|x| *x >= 0.0).map(Length::px)
}

/// a node of the scene and what is under it as Masonry's widgets
fn build(sc: &Scene, id: &str, ids: &mut Ids) -> NewWidget<dyn Widget> {
    let n = &sc.nodes[id];
    let text = n.prop("text").unwrap_or("").to_string();
    let w: NewWidget<dyn Widget> = match n.kind {
        Kind::Column | Kind::Row => {
            let mut f = if n.kind == Kind::Column { Flex::column() } else { Flex::row() };
            f = f.cross_axis_alignment(match n.prop("align") {
                Some("center") => CrossAxisAlignment::Center,
                Some("end") => CrossAxisAlignment::End,
                _ => CrossAxisAlignment::Start,
            });
            for c in &n.children {
                let child = build(sc, c, ids);
                f = f.with(child, flex(sc, c));
            }
            ids.shown.insert(id.to_string(), n.children.clone());
            // gap: between the children (Masonry's Gap), else the theme's
            match len(n.num("gap")) {
                Some(g) => NewWidget::new(f).with_props(Gap::new(g)).erased(),
                None => NewWidget::new(f).erased(),
            }
        }
        Kind::Label => NewWidget::new(Label::new(text)).erased(),
        Kind::Button => NewWidget::new(Button::with_text(text)).erased(),
        Kind::TextBox => {
            // the actions come from the area inside: its id is the node's too
            let area = NewWidget::new(TextArea::new_editable(&text));
            ids.node.insert(area.id(), id.to_string());
            let mut t = TextInput::from_text_area(area);
            if let Some(p) = n.prop("placeholder") {
                t = t.with_placeholder(p.to_string());
            }
            NewWidget::new(t).erased()
        }
        Kind::CheckBox => NewWidget::new(Checkbox::new(n.prop("checked") == Some("1"), text)).erased(),
        Kind::Progress => NewWidget::new(ProgressBar::new(n.num("value").map(|v| v.clamp(0.0, 1.0)))).erased(),
        Kind::Slider => {
            let (lo, hi) = (n.num("min").unwrap_or(0.0), n.num("max").unwrap_or(1.0));
            let mut s = Slider::new(lo, hi, n.num("value").unwrap_or(lo).clamp(lo.min(hi), hi.max(lo)));
            if let Some(st) = n.num("step").filter(|s| *s > 0.0) {
                s = s.with_step(st);
            }
            NewWidget::new(s).erased()
        }
        Kind::Scroll => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let inner = match n.children.first() {
                Some(c) => build(sc, c, ids),
                None => NewWidget::new(Flex::column()).erased(),
            };
            NewWidget::new(Portal::new(inner)).erased()
        }
        Kind::List => {
            // no children yet: VirtualScroll asks for those it shows (fetch)
            ids.shown.insert(id.to_string(), n.children.clone());
            ids.active.insert(id.to_string(), Default::default());
            NewWidget::new(VirtualScroll::new(0, n.children.len())).erased()
        }
        Kind::Box => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let mut b = match n.children.first() {
                Some(c) => SizedBox::new(build(sc, c, ids)),
                None => SizedBox::empty(),
            };
            if let Some(w) = len(n.num("width")) {
                b = b.width(w);
            }
            if let Some(h) = len(n.num("height")) {
                b = b.height(h);
            }
            NewWidget::new(b).erased()
        }
    };
    ids.node.insert(w.id(), id.to_string());
    ids.widget.insert(id.to_string(), w.id());
    w
}

/// a child's share of its row or column (0: as big as it is)
fn flex(sc: &Scene, id: &str) -> f64 {
    sc.nodes[id].num("flex").filter(|f| *f > 0.0).unwrap_or(0.0)
}

/// which of new keep their widgets: in the order of old, the most of them
/// (a longest common subsequence), none of those made anew (created); for each
/// of old and of new whether it is one of them
fn keep(old: &[String], new: &[String], created: &BTreeSet<String>) -> (Vec<bool>, Vec<bool>) {
    let (n, m) = (old.len(), new.len());
    let mut t = vec![vec![0u32; m + 1]; n + 1];
    for i in (0..n).rev() {
        for j in (0..m).rev() {
            t[i][j] = if old[i] == new[j] && !created.contains(&new[j]) {
                t[i + 1][j + 1] + 1
            } else {
                t[i + 1][j].max(t[i][j + 1])
            };
        }
    }
    let (mut ko, mut kn) = (vec![false; n], vec![false; m]);
    let (mut i, mut j) = (0, 0);
    while i < n && j < m {
        if old[i] == new[j] && !created.contains(&new[j]) {
            ko[i] = true;
            kn[j] = true;
            i += 1;
            j += 1;
        } else if t[i + 1][j] >= t[i][j + 1] {
            i += 1;
        } else {
            j += 1;
        }
    }
    (ko, kn)
}

/*
 * The widgets made to match the scene after its shape changed: from the root
 * down, each row's, column's, scroll's and box's children compared with those
 * its widget has; those that stay keep their widgets, and so their state (the
 * focus, the text being edited, a scroll's place), the others go, the new
 * ones are made (a node moved is made again in its new place).
 */
fn sync(root: &mut RenderRoot, sc: &Scene, ids: &mut Ids, created: &BTreeSet<String>) {
    let mut todo = vec![ROOT.to_string()];
    while let Some(p) = todo.pop() {
        let (Some(n), Some(&wid)) = (sc.nodes.get(&p), ids.widget.get(&p)) else { continue };
        let old = ids.shown.get(&p).cloned().unwrap_or_default();
        let new = n.children.clone();
        if n.kind == Kind::List {
            // its children with widgets: those whose place has another node made again
            let act = ids.active.get(&p).cloned().unwrap_or_default();
            root.edit_widget(wid, |mut w| {
                let mut v = w.downcast::<VirtualScroll>();
                for (idx, (node, cw)) in act {
                    let now = new.get(idx);
                    if now == Some(&node) && !created.contains(&node) {
                        todo.push(node);
                        continue;
                    }
                    VirtualScroll::remove_child(&mut v, idx);
                    ids.forget(sc, &node, cw);
                    let a = ids.active.entry(p.clone()).or_default();
                    a.remove(&idx);
                    if let Some(c) = now {
                        let child = build(sc, c, ids);
                        let cid = child.id();
                        VirtualScroll::add_child(&mut v, idx, child);
                        ids.active.entry(p.clone()).or_default().insert(idx, (c.clone(), cid));
                    }
                }
                VirtualScroll::set_len(&mut v, new.len());
            });
            ids.shown.insert(p, new);
            continue;
        }
        let (ko, kn) = keep(&old, &new, created);
        let kind = n.kind;
        root.edit_widget(wid, |mut w| match kind {
            Kind::Column | Kind::Row => {
                let mut f = w.downcast::<Flex>();
                for i in (0..old.len()).rev() {
                    if !ko[i] {
                        Flex::remove(&mut f, i);
                    }
                }
                for (j, c) in new.iter().enumerate() {
                    if kn[j] {
                        Flex::set_params(&mut f, j, flex(sc, c));
                    } else {
                        let child = build(sc, c, ids);
                        Flex::insert(&mut f, j, child, flex(sc, c));
                    }
                }
            }
            Kind::Scroll => {
                if !kn.first().copied().unwrap_or(old.is_empty()) {
                    let inner = match new.first() {
                        Some(c) => build(sc, c, ids),
                        None => NewWidget::new(Flex::column()).erased(),
                    };
                    Portal::set_child(&mut w.downcast::<Portal<dyn Widget>>(), inner);
                }
            }
            Kind::Box => {
                if !kn.first().copied().unwrap_or(old.is_empty()) {
                    let mut b = w.downcast::<SizedBox>();
                    match new.first() {
                        Some(c) => SizedBox::set_child(&mut b, build(sc, c, ids)),
                        None => SizedBox::remove_child(&mut b),
                    }
                }
            }
            _ => {}
        });
        ids.shown.insert(p, new.clone());
        for (j, c) in new.iter().enumerate() {
            if kn[j] {
                todo.push(c.clone());
            }
        }
    }
    // the nodes gone: their widgets with them
    ids.widget.retain(|id, _| sc.nodes.contains_key(id));
    ids.shown.retain(|id, _| sc.nodes.contains_key(id));
    ids.node.retain(|_, id| sc.nodes.contains_key(id));
}

/// the root's contents: the scene, with some room around it
fn content(sc: &Scene, ids: &mut Ids) -> NewWidget<dyn Widget> {
    *ids = Ids::default();
    let root = build(sc, ROOT, ids);
    NewWidget::new(SizedBox::new(root))
        .with_props(Padding::all(Length::px(8.0)))
        .erased()
}

/// what draws the window
enum Painter {
    #[cfg(feature = "gpu")]
    Gpu(gpu::Gpu),
    Cpu {
        surface: softbuffer::Surface<Arc<Window>, Arc<Window>>,
        renderer: masonry_imaging::vello_cpu::Renderer,
        rgba: Vec<u8>,
    },
}

impl Painter {
    /// the GPU's, unless -p or there is none; else the processor's
    #[allow(unused_variables)]
    fn new(window: &Arc<Window>, cpu: bool) -> (Painter, String) {
        #[cfg(feature = "gpu")]
        if !cpu {
            match gpu::Gpu::new(window.clone()) {
                Ok(g) => {
                    let name = format!("gpu {}", g.name);
                    return (Painter::Gpu(g), name);
                }
                Err(e) => eprintln!("ui: no GPU ({e}): on the processor"),
            }
        }
        let ctx = softbuffer::Context::new(window.clone()).expect("softbuffer");
        let surface = softbuffer::Surface::new(&ctx, window.clone()).expect("softbuffer surface");
        let renderer = masonry_imaging::vello_cpu::new_headless_renderer().expect("vello_cpu");
        (Painter::Cpu { surface, renderer, rgba: Vec::new() }, "cpu".into())
    }

    fn paint(&mut self, mut frame: PreparedFrame<'_>) {
        match self {
            #[cfg(feature = "gpu")]
            Painter::Gpu(g) => {
                if let Err(e) = g.render(frame) {
                    eprintln!("ui: render: {e}");
                }
            }
            Painter::Cpu { surface, renderer, rgba } => {
                let (Some(w), Some(h)) = (NonZeroU32::new(frame.width), NonZeroU32::new(frame.height)) else {
                    return;
                };
                let n = w.get() as usize * h.get() as usize;
                rgba.resize(n * 4, 0);
                let target = ImageBufferTarget {
                    data: rgba,
                    width: w.get(),
                    height: h.get(),
                    bytes_per_row: w.get() as usize * 4,
                    format: ImageBufferFormat::Rgba8Unorm,
                };
                if let Err(e) = renderer.render_source_into(&mut frame, target) {
                    eprintln!("ui: render: {e}");
                    return;
                }
                if surface.resize(w, h).is_err() {
                    return;
                }
                let Ok(mut buf) = surface.buffer_mut() else { return };
                for (p, c) in buf.iter_mut().zip(rgba.chunks_exact(4)) {
                    *p = (c[0] as u32) << 16 | (c[1] as u32) << 8 | c[2] as u32;
                }
                let _ = buf.present();
            }
        }
    }
}

struct Gui {
    window: Arc<Window>,
    painter: Painter,
    root: RenderRoot,
    reducer: WindowEventReducer,
}

struct App {
    fs: Fs,
    out: Arc<Fd>, // to the clients
    gui: Option<Gui>,
    ids: Ids,
    signals: mpsc::Receiver<RenderRootSignal>,
    signal_tx: mpsc::Sender<RenderRootSignal>,
    srv: String,
    cpu: bool,   // -p: on the processor
    debug: bool, // $UIDEBUG: the window's events and Masonry's signals on stderr
}

impl App {
    fn send(&self, msgs: Vec<Vec<u8>>) {
        for m in msgs {
            if self.out.write1(&m).is_err() {
                break;
            }
        }
    }

    fn event(&mut self, line: String) {
        let r = self.fs.event(line);
        self.send(r);
    }

    /// what the clients changed, into the widgets
    fn apply_changes(&mut self) {
        let changes = std::mem::take(&mut self.fs.changes);
        let Some(g) = self.gui.as_mut() else { return };
        let mut structure = false;
        let mut created = BTreeSet::new();
        let mut props = Vec::new();
        for ch in changes {
            if let Some(t) = &ch.title {
                g.window.set_title(t);
            }
            structure |= ch.structure;
            created.extend(ch.created);
            props.extend(ch.props);
        }
        // the shape first: the nodes made have their properties already
        if structure {
            sync(&mut g.root, &self.fs.scene, &mut self.ids, &created);
        }
        {
            for id in props {
                if created.contains(&id) {
                    continue;
                }
                let Some(n) = self.fs.scene.nodes.get(&id) else { continue };
                let Some(&wid) = self.ids.widget.get(&id) else { continue };
                let text = n.prop("text").unwrap_or("").to_string();
                let (kind, n) = (n.kind, n.clone());
                g.root.edit_widget(wid, |mut w| match kind {
                    Kind::Label => Label::set_text(&mut w.downcast::<Label>(), text),
                    Kind::Button => {
                        let mut b = w.downcast::<Button>();
                        let mut child = Button::child_mut(&mut b);
                        Label::set_text(&mut child.downcast::<Label>(), text);
                    }
                    Kind::TextBox => {
                        let mut t = w.downcast::<TextInput>();
                        let mut area = TextInput::text_mut(&mut t);
                        if area.widget.text().to_string() != text {
                            TextArea::reset_text(&mut area, &text);
                        }
                        drop(area);
                        if let Some(p) = n.prop("placeholder") {
                            TextInput::set_placeholder(&mut t, p.to_string());
                        }
                    }
                    Kind::CheckBox => {
                        let mut c = w.downcast::<Checkbox>();
                        Checkbox::set_checked(&mut c, n.prop("checked") == Some("1"));
                        Checkbox::set_text(&mut c, text.into());
                    }
                    Kind::Progress => ProgressBar::set_progress(
                        &mut w.downcast::<ProgressBar>(),
                        n.num("value").map(|v| v.clamp(0.0, 1.0)),
                    ),
                    Kind::Slider => {
                        let mut s = w.downcast::<Slider>();
                        let (lo, hi) = (n.num("min").unwrap_or(0.0), n.num("max").unwrap_or(1.0));
                        Slider::set_range(&mut s, lo, hi);
                        if let Some(v) = n.num("value") {
                            Slider::set_value(&mut s, v);
                        }
                    }
                    Kind::Box => {
                        let mut b = w.downcast::<SizedBox>();
                        if let Some(x) = len(n.num("width")) {
                            SizedBox::set_width(&mut b, x);
                        }
                        if let Some(y) = len(n.num("height")) {
                            SizedBox::set_height(&mut b, y);
                        }
                    }
                    Kind::Column | Kind::Row => {
                        let mut f = w.downcast::<Flex>();
                        Flex::set_cross_axis_alignment(&mut f, match n.prop("align") {
                            Some("center") => CrossAxisAlignment::Center,
                            Some("end") => CrossAxisAlignment::End,
                            _ => CrossAxisAlignment::Start,
                        });
                        match len(n.num("gap")) {
                            Some(g) => {
                                f.insert_prop(Gap::new(g));
                            }
                            None => {
                                f.remove_prop::<Gap>();
                            }
                        }
                    }
                    Kind::Scroll | Kind::List => {}
                });
            }
        }
        g.window.request_redraw();
    }

    /// what the widgets did: events for the clients, the scene kept as it is shown
    fn signals(&mut self, el: &ActiveEventLoop) {
        while let Ok(s) = self.signals.try_recv() {
            if self.debug {
                eprintln!("ui: signal {s:?}");
            }
            match s {
                RenderRootSignal::Action(a, wid) => self.action(a, wid),
                RenderRootSignal::RequestRedraw | RenderRootSignal::RequestAnimFrame => {
                    if let Some(g) = &self.gui {
                        g.window.request_redraw();
                    }
                }
                RenderRootSignal::SetCursor(c) => {
                    if let Some(g) = &self.gui {
                        g.window.set_cursor(c);
                    }
                }
                RenderRootSignal::ClipboardStore(s) => {
                    let _ = p9win::set_snarf(&s);
                }
                RenderRootSignal::SetTitle(t) => {
                    if let Some(g) = &self.gui {
                        g.window.set_title(&t);
                    }
                }
                RenderRootSignal::Exit => el.exit(),
                _ => {}
            }
        }
    }

    /// a list's widget wants widgets for the children it is to show, no longer
    /// for others (scrolled)
    fn fetch(&mut self, id: &str, wid: WidgetId, a: ErasedAction) {
        let Ok(a) = a.downcast::<VirtualScrollAction>() else { return };
        let VirtualScrollAction::Fetch(f) = *a else { return };
        let Some(g) = self.gui.as_mut() else { return };
        let (sc, ids) = (&self.fs.scene, &mut self.ids);
        let Some(n) = sc.nodes.get(id) else { return };
        let children = n.children.clone();
        g.root.edit_widget(wid, |mut w| {
            let mut v = w.downcast::<VirtualScroll>();
            VirtualScroll::will_handle_action(&mut v, &f);
            let act = ids.active.get(id).cloned().unwrap_or_default();
            for (idx, (node, cw)) in act {
                if !f.target().contains(&idx) {
                    VirtualScroll::remove_child(&mut v, idx);
                    ids.forget(sc, &node, cw);
                    ids.active.entry(id.to_string()).or_default().remove(&idx);
                }
            }
            for idx in f.target().clone() {
                let have = ids.active.get(id).is_some_and(|a| a.contains_key(&idx));
                if let (false, Some(c)) = (have, children.get(idx)) {
                    let child = build(sc, c, ids);
                    let cid = child.id();
                    VirtualScroll::add_child(&mut v, idx, child);
                    ids.active.entry(id.to_string()).or_default().insert(idx, (c.clone(), cid));
                }
            }
        });
    }

    fn action(&mut self, a: ErasedAction, wid: WidgetId) {
        let Some(id) = self.ids.node.get(&wid).cloned() else { return };
        if a.is::<VirtualScrollAction>() {
            self.fetch(&id, wid, a);
            return;
        }
        let set = |fs: &mut Fs, k: &str, v: String| {
            if let Some(n) = fs.scene.nodes.get_mut(&id) {
                n.props.insert(k.to_string(), v);
            }
        };
        if a.is::<ButtonPress>() {
            self.event(format!("click {id}"));
        } else if a.is::<CheckboxToggled>() {
            let on = a.downcast::<CheckboxToggled>().unwrap().0;
            set(&mut self.fs, "checked", if on { "1" } else { "0" }.to_string());
            // Masonry leaves the box to its owner: shown as the scene says
            if let (Some(g), Some(&w)) = (self.gui.as_mut(), self.ids.widget.get(&id)) {
                g.root.edit_widget(w, |mut w| Checkbox::set_checked(&mut w.downcast::<Checkbox>(), on));
            }
            self.event(format!("check {id} {}", on as u8));
        } else if a.is::<SliderMoved>() {
            let v = a.downcast::<SliderMoved>().unwrap().value;
            set(&mut self.fs, "value", format!("{v}"));
            self.event(format!("slide {id} {v}"));
        } else if a.is::<TextAction>() {
            match *a.downcast::<TextAction>().unwrap() {
                TextAction::Changed(t) => {
                    set(&mut self.fs, "text", t.clone());
                    self.event(format!("text {id} {t}"));
                }
                TextAction::Entered(t) => self.event(format!("enter {id} {t}")),
                TextAction::Cancelled => {}
            }
        }
    }

    fn redraw(&mut self) {
        let Some(g) = self.gui.as_mut() else { return };
        let size = g.window.inner_size();
        let (Some(w), Some(h)) = (NonZeroU32::new(size.width), NonZeroU32::new(size.height)) else {
            return;
        };
        if g.root.needs_anim() {
            g.root.handle_window_event(WindowEvent::AnimFrame(std::time::Duration::from_millis(16)));
        }
        let (layers, _access) = g.root.redraw();
        let overlays: Vec<_> = layers
            .overlay_layers()
            .filter_map(|l| match &l.kind {
                VisualLayerKind::Scene(scene) => Some(ImagingLayer { scene, transform: l.transform }),
                _ => None,
            })
            .collect();
        let Some(root) = layers.root_layer() else { return };
        let VisualLayerKind::Scene(base) = &root.kind else { return };
        let frame = PreparedFrame::new(w.get(), h.get(), 1.0, masonry::theme::BACKGROUND_COLOR, base, &overlays);
        g.painter.paint(frame);
        // where each node is now, for nodes/id/rect
        self.fs.rects.clear();
        for (id, wid) in &self.ids.widget {
            if let Some(w) = g.root.get_widget(*wid) {
                let b = w.ctx().bounding_box();
                self.fs.rects.insert(id.clone(), [b.x0 as i32, b.y0 as i32, b.x1 as i32, b.y1 as i32]);
            }
        }
        if g.root.needs_anim() {
            g.window.request_redraw();
        }
    }
}

impl ApplicationHandler<UserEvent> for App {
    fn resumed(&mut self, el: &ActiveEventLoop) {
        if self.gui.is_some() {
            return;
        }
        let window = match el.create_window(Window::default_attributes().with_title(self.fs.scene.title.clone())) {
            Ok(w) => Arc::new(w),
            Err(e) => {
                eprintln!("ui: window: {e}");
                el.exit();
                return;
            }
        };
        let (painter, renderer) = Painter::new(&window, self.cpu);
        self.fs.renderer = renderer;
        let size = window.inner_size();
        let tx = self.signal_tx.clone();
        let mut fonts = fonts().into_iter();
        let first = fonts.next();
        if first.is_none() {
            eprintln!("ui: no fonts in /lib/font/ttf: no text");
        }
        let mut root = RenderRoot::new(
            content(&self.fs.scene, &mut self.ids),
            move |s| {
                let _ = tx.send(s);
            },
            RenderRootOptions {
                default_properties: Arc::new(masonry::theme::default_property_set()),
                use_system_fonts: false,
                size_policy: WindowSizePolicy::User,
                size,
                scale_factor: 1.0,
                test_font: first.map(|b| Blob::new(Arc::new(b))),
            },
        );
        for b in fonts {
            root.register_fonts(Blob::new(Arc::new(b)));
        }
        self.fs.size = (size.width, size.height);
        self.gui = Some(Gui { window, painter, root, reducer: WindowEventReducer::default() });
        println!("{}", self.srv);
    }

    fn user_event(&mut self, el: &ActiveEventLoop, ev: UserEvent) {
        match ev {
            UserEvent::NineP(m) => {
                let r = self.fs.handle(&m);
                self.send(r);
                self.apply_changes();
                self.signals(el);
            }
            UserEvent::Hangup => {}
        }
    }

    fn window_event(&mut self, el: &ActiveEventLoop, _id: WindowId, event: WinitWindowEvent) {
        if self.debug && !matches!(event, WinitWindowEvent::RedrawRequested) {
            eprintln!("ui: {event:?}");
        }
        let Some(g) = self.gui.as_mut() else { return };
        if let Some(t) = g.reducer.reduce(1.0, &event) {
            match t {
                WindowEventTranslation::Keyboard(k) => {
                    if self.debug {
                        eprintln!("ui: key {k:?}");
                    }
                    let h = g.root.handle_text_event(TextEvent::Keyboard(k));
                    if self.debug {
                        eprintln!("ui: key handled {h:?}");
                    }
                }
                WindowEventTranslation::Pointer(p) => {
                    if self.debug {
                        eprintln!("ui: pointer {p:?}");
                    }
                    let h = g.root.handle_pointer_event(p);
                    if self.debug {
                        eprintln!("ui: pointer handled {h:?}");
                    }
                }
            }
        }
        match event {
            WinitWindowEvent::Resized(s) => {
                g.root.handle_window_event(WindowEvent::Resize(PhysicalSize::new(s.width, s.height)));
                self.fs.size = (s.width, s.height);
                g.window.request_redraw();
            }
            WinitWindowEvent::Moved(p) => {
                self.fs.origin = (p.x, p.y);
            }
            WinitWindowEvent::Focused(f) => {
                g.root.handle_text_event(TextEvent::WindowFocusChange(f));
            }
            WinitWindowEvent::RedrawRequested => self.redraw(),
            WinitWindowEvent::CloseRequested => {
                self.event("close".to_string());
                el.exit();
            }
            _ => {}
        }
        self.signals(el);
    }
}

/// a client's messages as they come over the connection: one read a message
fn reader(fd: Fd, proxy: EventLoopProxy<UserEvent>) {
    let mut b = vec![0u8; 65536 + ninep::IOHDRSZ as usize];
    loop {
        match fd.read(&mut b) {
            Ok(n) if n > 0 => {
                if proxy.send_event(UserEvent::NineP(b[..n].to_vec())).is_err() {
                    break;
                }
            }
            _ => break,
        }
    }
    let _ = proxy.send_event(UserEvent::Hangup);
}

/// $UIDEBUG: what wgpu, Masonry, ... log, on stderr
struct Logger;

impl log::Log for Logger {
    fn enabled(&self, _: &log::Metadata<'_>) -> bool {
        true
    }
    fn log(&self, r: &log::Record<'_>) {
        eprintln!("ui: {} {}: {}", r.level(), r.target(), r.args());
    }
    fn flush(&self) {}
}

fn main() {
    if std::env::var_os("UIDEBUG").is_some() {
        log::set_logger(&Logger).ok();
        log::set_max_level(log::LevelFilter::Debug);
    }
    let args: Vec<String> = std::env::args().collect();
    let user = std::fs::read_to_string("/dev/user").unwrap_or_else(|_| "none".into());
    let mut srv = format!("ui.{}.{}", user.trim(), std::process::id());
    let mut cpu = false;
    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "-p" => cpu = true,
            "-s" if i + 1 < args.len() => {
                srv = args[i + 1].clone();
                i += 1;
            }
            _ => {
                eprintln!("usage: ui [-p] [-s srvname]");
                std::process::exit(1);
            }
        }
        i += 1;
    }
    let (ours, theirs) = match p9win::sys::pipe() {
        Ok(p) => p,
        Err(e) => {
            eprintln!("ui: pipe: {e}");
            std::process::exit(1);
        }
    };
    // held open: /srv/srv goes with ui (ORCLOSE), also when a note ends it
    let _posted = match p9win::sys::post(&srv, &theirs) {
        Ok(f) => f,
        Err(e) => {
            eprintln!("ui: /srv/{srv}: {e}");
            std::process::exit(1);
        }
    };
    drop(theirs);
    let el = match EventLoop::<UserEvent>::with_user_event().build() {
        Ok(el) => el,
        Err(e) => {
            eprintln!("ui: {e}");
            std::process::exit(1);
        }
    };
    let input = Fd::open(&format!("#d/{}", ours.0), OREAD).expect("ui: our end");
    let proxy = el.create_proxy();
    std::thread::spawn(move || reader(input, proxy));
    let (signal_tx, signals) = mpsc::channel();
    let mut app = App {
        fs: Fs::new(user.trim()),
        out: Arc::new(ours),
        gui: None,
        ids: Ids::default(),
        signals,
        signal_tx,
        srv: srv.clone(),
        cpu,
        debug: std::env::var_os("UIDEBUG").is_some(),
    };
    let r = el.run_app(&mut app);
    if let Err(e) = r {
        eprintln!("ui: {e}");
        std::process::exit(1);
    }
}
