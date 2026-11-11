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
mod image;
mod look;
mod ninep;
mod scene;

use std::collections::{BTreeSet, HashMap};
use std::num::NonZeroU32;
use std::sync::{mpsc, Arc};

use masonry::app::{RenderRoot, RenderRootOptions, RenderRootSignal, VisualLayerKind, WindowSizePolicy};
use masonry::core::{CollectionWidget, ErasedAction, NewWidget, TextEvent, Widget, WidgetId, WindowEvent};
use masonry::peniko::Blob;
use masonry::properties::{Background, BorderColor, BorderWidth, ContentColor, CornerRadius, Gap, Padding};
use masonry::core::{PropertySet, WidgetMut};
use masonry::peniko::color::{AlphaColor, Srgb};
use masonry::properties::types::CrossAxisAlignment;
use masonry::layout::Length;
use masonry::widgets::{
    Button, ButtonPress, Checkbox, CheckboxToggled, Flex, Label, Portal, ProgressBar, SizedBox, Slider,
    SliderMoved, TextAction, TextArea, TextInput, VirtualScroll, VirtualScrollAction, Image, Selector,
    SelectionChanged, Switch, SwitchToggled, Divider, Spinner, Grid, GridParams, GridTrackSize,
    Align, ZStack, ChildAlignment, IndexedStack, Split, Badge, Badged, BadgePlacement, CollapsePanel, Prose,
    RadioButton, RadioButtonSelected, RadioGroup, StepInput, Step, SplitPoint, Svg, InsertNewline, VariableLabel, Pagination,
    PageChanged,
};
use masonry::layout::UnitPoint;
use masonry::kurbo::Axis;
use masonry::peniko::{ImageAlphaType, ImageBrush, ImageData, ImageFormat};
use masonry_imaging::{Layer as ImagingLayer, PreparedFrame};
use imaging::render::{ImageBufferFormat, ImageBufferTarget, ImageRenderer};
use p9win::sys::{Fd, OREAD};
use ui_events_winit::{WindowEventReducer, WindowEventTranslation};
use winit::application::ApplicationHandler;
use winit::dpi::PhysicalSize;
use winit::event::WindowEvent as WinitWindowEvent;
use winit::event_loop::{ActiveEventLoop, ControlFlow, EventLoop, EventLoopProxy};
use winit::window::{Window, WindowId};

use fs::Fs;
use scene::{Kind, Scene, ROOT};

/// what comes to the window's loop from elsewhere
enum UserEvent {
    NineP(Vec<u8>), // a client's message
    Hangup,         // the clients' connection ended
}

/// the fonts of the system: /lib/font/ttf (Plan 9's fonts are files); the
/// theme's uifont first (the default face), else the first by name
fn fonts(first: &str) -> Vec<Vec<u8>> {
    let mut v = Vec::new();
    if let Ok(d) = std::fs::read_dir("/lib/font/ttf") {
        let mut names: Vec<_> = d.flatten().map(|e| e.path()).collect();
        names.sort_by_key(|p| (p.to_str() != Some(first), p.clone()));
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

/// the text of the label or text box at p in the window, if one is there:
/// searched down from the root through what holds p (Masonry's own search
/// skips labels, which take no pointer), the deepest
fn textat(root: &RenderRoot, p: (f64, f64)) -> Option<String> {
    fn text(w: &masonry::core::WidgetRef<'_, dyn masonry::core::Widget>) -> Option<String> {
        if let Some(l) = w.downcast::<masonry::widgets::Label>() {
            Some(l.inner().text().to_string())
        } else if let Some(a) = w.downcast::<masonry::widgets::TextArea<true>>() {
            Some(a.inner().text().to_string())
        } else {
            w.downcast::<masonry::widgets::TextArea<false>>().map(|a| a.inner().text().to_string())
        }
    }
    fn down(w: masonry::core::WidgetRef<'_, dyn masonry::core::Widget>, pt: masonry::kurbo::Point) -> Option<String> {
        for c in w.children() {
            if c.ctx().bounding_box().contains(pt) {
                if let Some(t) = down(c, pt) {
                    return Some(t);
                }
            }
        }
        text(&w)
    }
    let t = down(root.get_layer_root(0), masonry::kurbo::Point::new(p.0, p.1))?;
    let t = t.trim().to_string();
    (!t.is_empty() && t.len() <= 8192).then_some(t)
}

/// text to the plumber (plumb(6)), from ui's directory
fn plumb(data: &str) {
    let wdir = std::env::current_dir().map(|d| d.to_string_lossy().into_owned()).unwrap_or_default();
    let msg = format!("ui\n\n{wdir}\ntext\n\n{}\n{data}", data.len());
    if let Ok(mut f) = std::fs::OpenOptions::new().write(true).open("/mnt/plumb/send") {
        use std::io::Write;
        let _ = f.write_all(msg.as_bytes());
    }
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

/// an image node's picture (nodes/id/image), none: a pixel one can see through
fn picture(sc: &Scene, id: &str) -> ImageBrush {
    let (w, h, px) = match sc.images.get(id) {
        Some(p) => (p.width, p.height, p.rgba.clone()),
        None => (1, 1, vec![0, 0, 0, 0]),
    };
    ImageBrush::from(ImageData {
        data: Blob::from(px),
        format: ImageFormat::Rgba8,
        alpha_type: ImageAlphaType::Alpha,
        width: w,
        height: h,
    })
}

/// a select's options: options a|b|c
fn options(n: &scene::Node) -> Vec<String> {
    n.prop("options").unwrap_or("").split('|').map(|s| s.trim().to_string()).filter(|s| !s.is_empty()).collect()
}

/// a grid's columns (default 2), and where its child i goes: row by row
fn gridcols(n: &scene::Node) -> usize {
    n.num("columns").map(|c| c as usize).filter(|c| *c > 0 && *c <= 64).unwrap_or(2)
}

fn gridpos(i: usize, cols: usize) -> GridParams {
    GridParams::new().with_pos((i % cols) as u16, (i / cols) as u16)
}

/// a colour: #rrggbb or #rrggbbaa
fn colour(s: &str) -> Option<AlphaColor<Srgb>> {
    let h = s.trim().strip_prefix('#')?;
    if !(h.len() == 6 || h.len() == 8) || !h.bytes().all(|c| c.is_ascii_hexdigit()) {
        return None;
    }
    let b = |i: usize| u8::from_str_radix(&h[i..i + 2], 16).unwrap_or(0);
    Some(AlphaColor::from_rgba8(b(0), b(2), b(4), if h.len() == 8 { b(6) } else { 255 }))
}

/// a node's look, any kind: background, border (colour), borderwidth, radius,
/// padding (pixels), color (a label's text, a divider's line)
fn styles(n: &scene::Node) -> PropertySet {
    let mut p = PropertySet::new();
    if let Some(c) = n.prop("background").and_then(colour) {
        p = p.with(Background::Color(c));
    }
    if let Some(c) = n.prop("border").and_then(colour) {
        p = p.with(BorderColor { color: c });
    }
    if let Some(w) = len(n.num("borderwidth")) {
        p = p.with(BorderWidth::all(w));
    }
    if let Some(r) = len(n.num("radius")) {
        p = p.with(CornerRadius::all(r));
    }
    if let Some(x) = len(n.num("padding")) {
        p = p.with(Padding::all(x));
    }
    if let Some(c) = n.prop("color").and_then(colour) {
        p = p.with(ContentColor::new(c));
    }
    p
}

/// the look of a widget there already, as the node's keys say now (a key gone: the theme's)
fn restyle(w: &mut WidgetMut<'_, dyn Widget>, n: &scene::Node) {
    match n.prop("background").and_then(colour) {
        Some(c) => {
            w.insert_prop(Background::Color(c));
        }
        None => {
            w.remove_prop::<Background>();
        }
    }
    match n.prop("border").and_then(colour) {
        Some(c) => {
            w.insert_prop(BorderColor { color: c });
        }
        None => {
            w.remove_prop::<BorderColor>();
        }
    }
    match len(n.num("borderwidth")) {
        Some(x) => {
            w.insert_prop(BorderWidth::all(x));
        }
        None => {
            w.remove_prop::<BorderWidth>();
        }
    }
    match len(n.num("radius")) {
        Some(x) => {
            w.insert_prop(CornerRadius::all(x));
        }
        None => {
            w.remove_prop::<CornerRadius>();
        }
    }
    match len(n.num("padding")) {
        Some(x) => {
            w.insert_prop(Padding::all(x));
        }
        None => {
            w.remove_prop::<Padding>();
        }
    }
    match n.prop("color").and_then(colour) {
        Some(c) => {
            w.insert_prop(ContentColor::new(c));
        }
        None => {
            w.remove_prop::<ContentColor>();
        }
    }
}

fn len(v: Option<f64>) -> Option<Length> {
    v.filter(|x| *x >= 0.0).map(Length::px)
}

/// a place in a box: align's words (center the default)
fn unitpoint(s: Option<&str>) -> UnitPoint {
    match s {
        Some("left") => UnitPoint::LEFT,
        Some("right") => UnitPoint::RIGHT,
        Some("top") => UnitPoint::TOP,
        Some("bottom") => UnitPoint::BOTTOM,
        Some("topleft") => UnitPoint::TOP_LEFT,
        Some("topright") => UnitPoint::TOP_RIGHT,
        Some("bottomleft") => UnitPoint::BOTTOM_LEFT,
        Some("bottomright") => UnitPoint::BOTTOM_RIGHT,
        _ => UnitPoint::CENTER,
    }
}

/// child i of node id as widgets, else an empty box (a kind that wants one)
fn nth(sc: &Scene, id: &str, i: usize, ids: &mut Ids) -> NewWidget<dyn Widget> {
    match sc.nodes[id].children.get(i) {
        Some(c) => build(sc, c, ids),
        None => NewWidget::new(SizedBox::empty()).erased(),
    }
}

/// an svg node's picture (written to nodes/id/image), else nothing to draw
fn svgtree(sc: &Scene, id: &str) -> Arc<resvg::usvg::Tree> {
    sc.svgs.get(id).cloned().unwrap_or_else(|| {
        let empty = r#"<svg xmlns="http://www.w3.org/2000/svg" width="1" height="1"/>"#;
        Arc::new(resvg::usvg::Tree::from_str(empty, &resvg::usvg::Options::default()).expect("an empty svg"))
    })
}

/// a stepper's numbers: value step min max (0 1 0 100 unless said)
fn stepnums(n: &scene::Node) -> (f64, f64, f64, f64) {
    let (lo, hi) = (n.num("min").unwrap_or(0.0), n.num("max").unwrap_or(100.0));
    let (lo, hi) = (lo.min(hi), hi.max(lo));
    (n.num("value").unwrap_or(lo).clamp(lo, hi), n.num("step").filter(|s| *s > 0.0).unwrap_or(1.0), lo, hi)
}

fn placement(s: Option<&str>) -> BadgePlacement {
    match s {
        Some("topleft") => BadgePlacement::TopLeft,
        Some("bottomleft") => BadgePlacement::BottomLeft,
        Some("bottomright") => BadgePlacement::BottomRight,
        _ => BadgePlacement::TopRight,
    }
}

/// a badge: its count (count), else its text
fn badge(n: &scene::Node, text: &str) -> NewWidget<dyn Widget> {
    match n.num("count") {
        Some(c) => NewWidget::new(Badge::count(c.max(0.0) as u32)).erased(),
        None => NewWidget::new(Badge::with_text(text.to_string())).erased(),
    }
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
        Kind::Image => NewWidget::new(Image::new(picture(sc, id))).erased(),
        Kind::Select => {
            let opts = options(n);
            let sel = n.num("selected").map(|v| v as usize).unwrap_or(0).min(opts.len().saturating_sub(1));
            NewWidget::new(Selector::new(opts).with_selected_option(sel)).erased()
        }
        Kind::Switch => NewWidget::new(Switch::new(n.prop("checked") == Some("1"))).erased(),
        Kind::Divider => NewWidget::new(if n.prop("axis") == Some("vertical") { Divider::vertical() } else { Divider::horizontal() }).erased(),
        Kind::Spinner => NewWidget::new(Spinner::new()).erased(),
        Kind::Grid => {
            let cols = gridcols(n);
            let rows = n.children.len().div_ceil(cols).max(1);
            let mut g = Grid::with_dimensions(cols, rows);
            for (i, c) in n.children.iter().enumerate() {
                g = g.with(build(sc, c, ids), gridpos(i, cols));
            }
            ids.shown.insert(id.to_string(), n.children.clone());
            NewWidget::new(g).erased()
        }
        Kind::List => {
            // no children yet: VirtualScroll asks for those it shows (fetch)
            ids.shown.insert(id.to_string(), n.children.clone());
            ids.active.insert(id.to_string(), Default::default());
            NewWidget::new(VirtualScroll::new(0, n.children.len())).erased()
        }
        Kind::Align => {
            ids.shown.insert(id.to_string(), n.children.clone());
            NewWidget::new(Align::new(unitpoint(n.prop("align")), nth(sc, id, 0, ids))).erased()
        }
        Kind::ZStack => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let mut z = ZStack::new().with_alignment(unitpoint(n.prop("align")));
            for i in 0..n.children.len() {
                z = z.with(nth(sc, id, i, ids), ChildAlignment::ParentAligned);
            }
            NewWidget::new(z).erased()
        }
        Kind::Stack => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let mut st = IndexedStack::new();
            for i in 0..n.children.len() {
                st = st.with(nth(sc, id, i, ids));
            }
            let a = n.num("active").map_or(0, |a| a.max(0.0) as usize).min(n.children.len().saturating_sub(1));
            NewWidget::new(st.with_active_child(a)).erased()
        }
        Kind::Split => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let (a, b) = (nth(sc, id, 0, ids), nth(sc, id, 1, ids));
            let axis = if n.prop("axis") == Some("vertical") { Axis::Vertical } else { Axis::Horizontal };
            let sp = Split::new(a, b)
                .split_axis(axis)
                .split_fraction(n.num("fraction").unwrap_or(0.5).clamp(0.0, 1.0))
                .draggable(n.prop("draggable") != Some("0"));
            NewWidget::new(sp).erased()
        }
        Kind::Badge => badge(n, &text),
        Kind::Badged => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let content = nth(sc, id, 0, ids);
            let b = match n.prop("badge") {
                Some(t) => Badged::new(content, NewWidget::new(Badge::with_text(t.to_string()))),
                None => Badged::new_optional(content, None),
            };
            NewWidget::new(b.with_badge_placement(placement(n.prop("placement")))).erased()
        }
        Kind::Collapse => {
            ids.shown.insert(id.to_string(), n.children.clone());
            let c = CollapsePanel::new(n.prop("collapsed") == Some("1"), text, nth(sc, id, 0, ids));
            NewWidget::new(c).erased()
        }
        Kind::Prose => NewWidget::new(Prose::new(&text)).erased(),
        Kind::Radio => NewWidget::new(RadioButton::new(n.prop("checked") == Some("1"), text)).erased(),
        Kind::RadioGroup => {
            ids.shown.insert(id.to_string(), n.children.clone());
            NewWidget::new(RadioGroup::new(nth(sc, id, 0, ids))).erased()
        }
        Kind::Stepper => {
            let (v, st, lo, hi) = stepnums(n);
            NewWidget::new(StepInput::<f64>::new(v, st, lo, hi).with_wrap(n.prop("wrap") == Some("1"))).erased()
        }
        Kind::Svg => NewWidget::new(Svg::new(svgtree(sc, id))).erased(),
        Kind::TextArea => {
            // as a text box, more lines: Enter makes a new one
            let area = NewWidget::new(TextArea::new_editable(&text).with_insert_newline(InsertNewline::OnEnter));
            ids.node.insert(area.id(), id.to_string());
            let mut t = TextInput::from_text_area(area);
            if let Some(p) = n.prop("placeholder") {
                t = t.with_placeholder(p.to_string());
            }
            NewWidget::new(t).erased()
        }
        Kind::VarLabel => NewWidget::new(VariableLabel::new(text).with_initial_weight(n.num("weight").unwrap_or(400.0) as f32)).erased(),
        Kind::Pages => {
            let count = n.num("count").map_or(1, |c| c.max(1.0) as usize);
            let page = n.num("page").map_or(0, |p| p.max(0.0) as usize).min(count - 1);
            NewWidget::new(Pagination::new(count).active_page(page)).erased()
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
    let w = w.with_props(styles(n));
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
            Kind::Grid => {
                let mut gr = w.downcast::<Grid>();
                for i in (0..old.len()).rev() {
                    if !ko[i] {
                        Grid::remove(&mut gr, i);
                    }
                }
                let cols = gridcols(n);
                for (j, c) in new.iter().enumerate() {
                    if kn[j] {
                        Grid::set_params(&mut gr, j, gridpos(j, cols));
                    } else {
                        let child = build(sc, c, ids);
                        Grid::insert(&mut gr, j, child, gridpos(j, cols));
                    }
                }
                Grid::set_columns(&mut gr, vec![GridTrackSize::FRACTION; cols]);
                Grid::set_rows(&mut gr, vec![GridTrackSize::FRACTION; new.len().div_ceil(cols).max(1)]);
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
            // one child, made again when it is another
            Kind::Align | Kind::Collapse | Kind::Badged => {
                if !kn.first().copied().unwrap_or(old.is_empty()) {
                    let child = nth(sc, &p, 0, ids);
                    match kind {
                        Kind::Align => Align::set_child(&mut w.downcast::<Align>(), child),
                        Kind::Collapse => CollapsePanel::set_child(&mut w.downcast::<CollapsePanel>(), child),
                        _ => Badged::set_content(&mut w.downcast::<Badged>(), child),
                    }
                }
            }
            // two: each in its place
            Kind::Split => {
                let mut sp = w.downcast::<Split<dyn Widget, dyn Widget>>();
                for i in 0..2 {
                    let same = old.get(i) == new.get(i) && new.get(i).is_none_or(|c| !created.contains(c));
                    if !same {
                        let child = nth(sc, &p, i, ids);
                        if i == 0 {
                            Split::set_child1(&mut sp, child);
                        } else {
                            Split::set_child2(&mut sp, child);
                        }
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

/// how far an animation is, eased (slow at both ends)
fn ease(a: &Anim) -> f64 {
    let t = (a.start.elapsed().as_secs_f64() / a.secs).clamp(0.0, 1.0);
    t * t * (3.0 - 2.0 * t)
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
    background: masonry::peniko::Color, // the frame's, the theme's window colour (look.rs)
}

/// a property moving to its new value (animate), from as shown when it changed
struct Anim {
    from: f64,
    to: f64,
    start: std::time::Instant,
    secs: f64,
}

/// the properties a kind animates (a node's animate: milliseconds)
fn animated(kind: Kind) -> &'static [&'static str] {
    match kind {
        Kind::Progress | Kind::Slider => &["value"],
        Kind::Box => &["width", "height"],
        _ => &[],
    }
}

/// a property of a widget there, as the animation has it now
fn setnum(w: WidgetMut<'_, dyn Widget>, kind: Kind, key: &str, v: f64) {
    let mut w = w;
    match (kind, key) {
        (Kind::Progress, _) => ProgressBar::set_progress(&mut w.downcast::<ProgressBar>(), Some(v.clamp(0.0, 1.0))),
        (Kind::Slider, _) => Slider::set_value(&mut w.downcast::<Slider>(), v),
        (Kind::Box, "width") => SizedBox::set_width(&mut w.downcast::<SizedBox>(), Length::px(v.max(0.0))),
        (Kind::Box, _) => SizedBox::set_height(&mut w.downcast::<SizedBox>(), Length::px(v.max(0.0))),
        _ => {}
    }
}

/// the time between frames while something moves (animations, Masonry's, a
/// scroll): the display's 60 Hz. Through draw(3) nothing paces a client (Vulkan's
/// swapchain does, wsys' s events): without it, frames as fast as they draw.
const FRAME: std::time::Duration = std::time::Duration::from_micros(16_667);
/// a step of the wheel, in pixels (Masonry's line), given out over frames: a
/// part of what is left each frame, so a scroll starts at once and slows down
const WHEELLINE: f64 = 120.0;
const WHEELPART: f64 = 0.35;

struct App {
    anims: HashMap<(String, &'static str), Anim>,
    shownv: HashMap<(String, &'static str), f64>, // the animated properties as the widgets show them
    fs: Fs,
    out: Arc<Fd>, // to the clients
    gui: Option<Gui>,
    ids: Ids,
    signals: mpsc::Receiver<RenderRootSignal>,
    signal_tx: mpsc::Sender<RenderRootSignal>,
    srv: String,
    cpu: bool,   // -p: on the processor
    debug: bool, // $UIDEBUG: the window's events and Masonry's signals on stderr
    pointer: (f64, f64),             // where the pointer is in the window
    b3at: Option<(f64, f64)>,        // button 3 pressed there: a click plumbs (as acme)
    lastframe: Option<std::time::Instant>, // the last frame of something moving (Masonry's AnimFrame: the time since)
    due: Option<std::time::Instant>,       // the next frame of something moving, not before then (FRAME)
    wheel: (f64, f64),                     // the wheel's pixels not given to the widgets yet
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
        // kinds whose widgets cannot take or give up a child (zstack, stack,
        // radiogroup): made again when their children change, and so those
        // they are in that cannot either
        if structure {
            let sc = &self.fs.scene;
            let again = |k: Kind| matches!(k, Kind::ZStack | Kind::Stack | Kind::RadioGroup);
            for (id, n) in &sc.nodes {
                if again(n.kind) && self.ids.shown.get(id).is_some_and(|old| *old != n.children) {
                    created.insert(id.clone());
                    let mut p = n.parent.clone();
                    while let Some(pid) = p {
                        match sc.nodes.get(&pid) {
                            Some(pn) if again(pn.kind) => {
                                created.insert(pid.clone());
                                p = pn.parent.clone();
                            }
                            _ => break,
                        }
                    }
                }
            }
        }
        // the shape first: the nodes made have their properties already
        if structure {
            sync(&mut g.root, &self.fs.scene, &mut self.ids, &created);
        }
        {
            // what the new nodes show: where their animations start from
            for id in &created {
                if let Some(n) = self.fs.scene.nodes.get(id) {
                    for k in animated(n.kind) {
                        if let Some(v) = n.num(k) {
                            self.shownv.insert((id.clone(), *k), v);
                        }
                    }
                }
            }
            self.shownv.retain(|(id, _), _| self.fs.scene.nodes.contains_key(id));
            self.anims.retain(|(id, _), _| self.fs.scene.nodes.contains_key(id));
            for id in props {
                if created.contains(&id) {
                    continue;
                }
                let Some(n) = self.fs.scene.nodes.get(&id) else { continue };
                let Some(&wid) = self.ids.widget.get(&id) else { continue };
                let text = n.prop("text").unwrap_or("").to_string();
                let (kind, mut n) = (n.kind, n.clone());
                // animate: an animated property goes there from where it is shown,
                // a frame at a time (tick); the widget keeps that for now
                let ms = n.num("animate").filter(|m| *m > 0.0);
                for k in animated(kind) {
                    let Some(to) = n.num(k) else { continue };
                    let key = (id.clone(), *k);
                    let now = self.anims.get(&key).map(|a| a.from + (a.to - a.from) * ease(a)).or(self.shownv.get(&key).copied());
                    match (ms, now) {
                        (Some(ms), Some(from)) if from != to => {
                            self.anims.insert(key, Anim { from, to, start: std::time::Instant::now(), secs: ms / 1000.0 });
                            n.props.insert(k.to_string(), format!("{from}"));
                        }
                        _ => {
                            self.anims.remove(&key);
                            self.shownv.insert(key, to);
                        }
                    }
                }
                g.root.edit_widget(wid, |mut w| {
                    restyle(&mut w, &n);
                    match kind {
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
                    Kind::Image => Image::set_image_data(&mut w.downcast::<Image>(), picture(&self.fs.scene, &id)),
                    Kind::Select => {
                        let mut sl = w.downcast::<Selector>();
                        let opts = options(&n);
                        let sel = n.num("selected").map(|v| v as usize).unwrap_or(0).min(opts.len().saturating_sub(1));
                        Selector::set_options(&mut sl, opts);
                        Selector::select_option(&mut sl, sel);
                    }
                    Kind::Switch => Switch::set_on(&mut w.downcast::<Switch>(), n.prop("checked") == Some("1")),
                    Kind::Grid => {
                        let mut gr = w.downcast::<Grid>();
                        let cols = gridcols(&n);
                        for j in 0..n.children.len() {
                            Grid::set_params(&mut gr, j, gridpos(j, cols));
                        }
                        Grid::set_columns(&mut gr, vec![GridTrackSize::FRACTION; cols]);
                        Grid::set_rows(&mut gr, vec![GridTrackSize::FRACTION; n.children.len().div_ceil(cols).max(1)]);
                    }
                    Kind::Align => Align::set_alignment(&mut w.downcast::<Align>(), unitpoint(n.prop("align"))),
                    Kind::ZStack => ZStack::set_alignment(&mut w.downcast::<ZStack>(), unitpoint(n.prop("align"))),
                    Kind::Stack => {
                        let a = n.num("active").map_or(0, |a| a.max(0.0) as usize).min(n.children.len().saturating_sub(1));
                        IndexedStack::set_active_child(&mut w.downcast::<IndexedStack>(), a);
                    }
                    Kind::Split => {
                        let mut sp = w.downcast::<Split<dyn Widget, dyn Widget>>();
                        let axis = if n.prop("axis") == Some("vertical") { Axis::Vertical } else { Axis::Horizontal };
                        Split::set_split_axis(&mut sp, axis);
                        if let Some(f) = n.num("fraction") {
                            Split::set_split_point(&mut sp, SplitPoint::Fraction(f.clamp(0.0, 1.0)));
                        }
                        Split::set_draggable(&mut sp, n.prop("draggable") != Some("0"));
                    }
                    Kind::Badge => {
                        let t = n.num("count").map_or(text.clone(), |c| format!("{}", c.max(0.0) as u32));
                        Badge::set_child(&mut w.downcast::<Badge>(), NewWidget::new(Label::new(t)));
                    }
                    Kind::Badged => {
                        let mut b = w.downcast::<Badged>();
                        match n.prop("badge") {
                            Some(t) => Badged::set_badge(&mut b, NewWidget::new(Badge::with_text(t.to_string()))),
                            None => Badged::clear_badge(&mut b),
                        }
                        Badged::set_badge_placement(&mut b, placement(n.prop("placement")));
                    }
                    Kind::Collapse => {
                        let mut c = w.downcast::<CollapsePanel>();
                        CollapsePanel::set_collapsed(&mut c, n.prop("collapsed") == Some("1"));
                        CollapsePanel::set_text(&mut c, text.into());
                    }
                    Kind::Prose => {
                        let mut pr = w.downcast::<Prose>();
                        let mut area = Prose::text_mut(&mut pr);
                        if area.widget.text().to_string() != text {
                            TextArea::reset_text(&mut area, &text);
                        }
                    }
                    Kind::Radio => {
                        let mut r = w.downcast::<RadioButton>();
                        RadioButton::set_checked(&mut r, n.prop("checked") == Some("1"));
                        RadioButton::set_text(&mut r, text.into());
                    }
                    Kind::Stepper => {
                        let mut st = w.downcast::<StepInput<f64>>();
                        let (v, step, lo, hi) = stepnums(&n);
                        StepInput::set_bounds(&mut st, lo, hi);
                        StepInput::set_step(&mut st, step);
                        StepInput::set_base(&mut st, v);
                        StepInput::set_wrap(&mut st, n.prop("wrap") == Some("1"));
                    }
                    Kind::Svg => Svg::set_tree(&mut w.downcast::<Svg>(), svgtree(&self.fs.scene, &id)),
                    Kind::TextArea => {
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
                    Kind::VarLabel => {
                        let mut v = w.downcast::<VariableLabel>();
                        VariableLabel::set_text(&mut v, text);
                        let ms = n.num("animate").filter(|m| *m > 0.0).unwrap_or(0.0) as f32;
                        VariableLabel::set_target_weight(&mut v, n.num("weight").unwrap_or(400.0) as f32, ms);
                    }
                    Kind::Pages => {
                        let mut pg = w.downcast::<Pagination>();
                        let count = n.num("count").map_or(1, |c| c.max(1.0) as usize);
                        Pagination::set_page_count(&mut pg, count);
                        Pagination::set_active_page(&mut pg, n.num("page").map_or(0, |p| p.max(0.0) as usize).min(count - 1));
                    }
                    Kind::Scroll | Kind::List | Kind::Divider | Kind::Spinner | Kind::RadioGroup => {}
                    }
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
                // while something moves its frames are paced: a widget changed by
                // the last one is drawn with the next (else each frame asked at once)
                RenderRootSignal::RequestRedraw if self.lastframe.is_some() => self.another(),
                RenderRootSignal::RequestRedraw => {
                    if let Some(g) = &self.gui {
                        g.window.request_redraw();
                    }
                }
                RenderRootSignal::RequestAnimFrame => self.another(),
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
        } else if a.is::<SwitchToggled>() {
            let on = a.downcast::<SwitchToggled>().unwrap().0;
            set(&mut self.fs, "checked", if on { "1" } else { "0" }.to_string());
            if let (Some(g), Some(&w)) = (self.gui.as_mut(), self.ids.widget.get(&id)) {
                g.root.edit_widget(w, |mut w| Switch::set_on(&mut w.downcast::<Switch>(), on));
            }
            self.event(format!("check {id} {}", on as u8));
        } else if a.is::<SelectionChanged>() {
            let c = a.downcast::<SelectionChanged>().unwrap();
            set(&mut self.fs, "selected", format!("{}", c.index));
            self.event(format!("select {id} {} {}", c.index, c.selected_content));
        } else if a.is::<SliderMoved>() {
            let v = a.downcast::<SliderMoved>().unwrap().value;
            set(&mut self.fs, "value", format!("{v}"));
            self.event(format!("slide {id} {v}"));
        } else if a.is::<RadioButtonSelected>() {
            // the one on in its group (the radiogroup it is in), the others off
            let group = {
                let sc = &self.fs.scene;
                let mut p = sc.nodes.get(&id).and_then(|n| n.parent.clone());
                while let Some(pid) = p.clone() {
                    if sc.nodes.get(&pid).is_some_and(|n| n.kind == Kind::RadioGroup) {
                        break;
                    }
                    p = sc.nodes.get(&pid).and_then(|n| n.parent.clone());
                }
                p
            };
            let mut radios = vec![id.clone()];
            if let Some(gid) = group {
                let mut todo = vec![gid];
                while let Some(x) = todo.pop() {
                    if let Some(n) = self.fs.scene.nodes.get(&x) {
                        if n.kind == Kind::Radio && x != id {
                            radios.push(x.clone());
                        }
                        todo.extend(n.children.iter().cloned());
                    }
                }
            }
            for r in &radios {
                let on = *r == id;
                if let Some(n) = self.fs.scene.nodes.get_mut(r) {
                    n.props.insert("checked".to_string(), if on { "1" } else { "0" }.to_string());
                }
                if let (Some(g), Some(&w)) = (self.gui.as_mut(), self.ids.widget.get(r)) {
                    g.root.edit_widget(w, |mut w| RadioButton::set_checked(&mut w.downcast::<RadioButton>(), on));
                }
            }
            self.event(format!("radio {id}"));
        } else if a.is::<Step<f64>>() {
            let v = a.downcast::<Step<f64>>().unwrap().value;
            set(&mut self.fs, "value", format!("{v}"));
            self.event(format!("step {id} {v}"));
        } else if a.is::<PageChanged>() {
            let pg = a.downcast::<PageChanged>().unwrap().0;
            set(&mut self.fs, "page", format!("{pg}"));
            self.event(format!("page {id} {pg}"));
        } else if a.is::<TextAction>() {
            match *a.downcast::<TextAction>().unwrap() {
                TextAction::Changed(t) => {
                    self.event(format!("text {id} {}", scene::evquote(&t)));
                    set(&mut self.fs, "text", t);
                }
                TextAction::Entered(t) => self.event(format!("enter {id} {}", scene::evquote(&t))),
                TextAction::Cancelled => {}
            }
        }
    }

    /// the animations a frame on; the done ones end where they were going
    fn tick(&mut self) {
        let Some(g) = self.gui.as_mut() else { return };
        let mut done = Vec::new();
        for ((id, k), a) in &self.anims {
            let (Some(n), Some(&wid)) = (self.fs.scene.nodes.get(id), self.ids.widget.get(id)) else {
                done.push((id.clone(), *k));
                continue;
            };
            let t = a.start.elapsed().as_secs_f64() / a.secs;
            let v = if t >= 1.0 { a.to } else { a.from + (a.to - a.from) * ease(a) };
            let kind = n.kind;
            g.root.edit_widget(wid, |w| setnum(w, kind, k, v));
            self.shownv.insert((id.clone(), *k), v);
            if t >= 1.0 {
                done.push((id.clone(), *k));
            }
        }
        for d in done {
            self.anims.remove(&d);
        }
    }

    fn redraw(&mut self) {
        self.tick();
        let Some(g) = self.gui.as_mut() else { return };
        let size = g.window.inner_size();
        let (Some(w), Some(h)) = (NonZeroU32::new(size.width), NonZeroU32::new(size.height)) else {
            return;
        };
        // the wheel's part for this frame, at the pointer (as the wheel's event)
        let (wx, wy) = self.wheel;
        if wx != 0.0 || wy != 0.0 {
            // a part of what is left, whole pixels, at least one; the last bit as it is
            let part = |v: f64| match v {
                v if v.abs() <= 1.0 => v,
                v => match (v * WHEELPART).round() {
                    0.0 => v.signum(),
                    p => p,
                },
            };
            let (px, py) = (part(wx), part(wy));
            self.wheel = (wx - px, wy - py);
            let ev = WinitWindowEvent::MouseWheel {
                device_id: winit::event::DeviceId::dummy(),
                delta: winit::event::MouseScrollDelta::PixelDelta(winit::dpi::PhysicalPosition::new(px, py)),
                phase: winit::event::TouchPhase::Moved,
            };
            if let Some(WindowEventTranslation::Pointer(p)) = g.reducer.reduce(1.0, &ev) {
                g.root.handle_pointer_event(p);
            }
        }
        // Masonry's animations: the time since the last frame of something moving
        // (not more than a few frames' worth after a pause)
        let now = std::time::Instant::now();
        if g.root.needs_anim() {
            let dt = self.lastframe.map_or(FRAME, |t| now.duration_since(t).min(FRAME * 4));
            g.root.handle_window_event(WindowEvent::AnimFrame(dt));
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
        let frame = PreparedFrame::new(w.get(), h.get(), 1.0, g.background, base, &overlays);
        g.painter.paint(frame);
        self.fs.frames += 1;
        // something moving: another frame, a FRAME after this one began
        let moving = !self.anims.is_empty() || g.root.needs_anim() || self.wheel != (0.0, 0.0);
        self.lastframe = moving.then_some(now);
        // where each node is now, for nodes/id/rect
        self.fs.rects.clear();
        for (id, wid) in &self.ids.widget {
            if let Some(w) = g.root.get_widget(*wid) {
                let b = w.ctx().bounding_box();
                self.fs.rects.insert(id.clone(), [b.x0 as i32, b.y0 as i32, b.x1 as i32, b.y1 as i32]);
            }
        }
        if moving {
            self.another();
        }
    }

    /// another frame for what moves: not before FRAME after the last one
    /// (about_to_wait waits for it)
    fn another(&mut self) {
        let at = self.lastframe.map_or_else(std::time::Instant::now, |t| t + FRAME);
        self.due = Some(self.due.map_or(at, |d| d.min(at)));
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
        // the desktop's theme: colours (look.rs), the UI font
        let theme = desk::theme::current();
        let palette = look::Palette::from_theme(&theme);
        let mut fonts = fonts(&theme.uifont).into_iter();
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
                default_properties: Arc::new(look::property_set(&palette)),
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
        self.gui = Some(Gui { window, painter, root, reducer: WindowEventReducer::default(), background: palette.background });
        println!("{}", self.srv);
    }

    /// a frame due (another): asked for when its time came, else waited for
    fn about_to_wait(&mut self, el: &ActiveEventLoop) {
        match self.due {
            Some(t) if std::time::Instant::now() >= t => {
                self.due = None;
                if let Some(g) = &self.gui {
                    g.window.request_redraw();
                }
                el.set_control_flow(ControlFlow::Wait);
            }
            Some(t) => el.set_control_flow(ControlFlow::WaitUntil(t)),
            None => el.set_control_flow(ControlFlow::Wait),
        }
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
        // the wheel's steps (lines) given to the widgets a part each frame (redraw)
        if let WinitWindowEvent::MouseWheel { delta: winit::event::MouseScrollDelta::LineDelta(x, y), .. } = event {
            self.wheel.0 += x as f64 * WHEELLINE;
            self.wheel.1 += y as f64 * WHEELLINE;
            self.another();
            self.signals(el);
            return;
        }
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
            WinitWindowEvent::CursorMoved { position, .. } => {
                self.pointer = (position.x, position.y);
            }
            // a click of button 3 on text plumbs it, as acme: Masonry tells neither a
            // word nor a selection, so the label's or text box's text, all of it
            WinitWindowEvent::MouseInput { state, button: winit::event::MouseButton::Right, .. } => {
                if state.is_pressed() {
                    self.b3at = Some(self.pointer);
                } else if let Some(at) = self.b3at.take() {
                    if (at.0 - self.pointer.0).abs() < 3.0 && (at.1 - self.pointer.1).abs() < 3.0 {
                        let text = textat(&g.root, at);
                        if let Some(t) = text {
                            plumb(&t);
                        }
                    }
                }
            }
            // the desktop's theme changed (/dev/theme, written): its colours now
            WinitWindowEvent::ThemeChanged(_) => {
                let palette = look::Palette::from_theme(&desk::theme::current());
                g.root.set_default_properties(Arc::new(look::property_set(&palette)));
                g.background = palette.background;
                g.window.request_redraw();
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
        anims: HashMap::new(),
        shownv: HashMap::new(),
        fs: Fs::new(user.trim()),
        out: Arc::new(ours),
        gui: None,
        ids: Ids::default(),
        signals,
        signal_tx,
        srv: srv.clone(),
        cpu,
        debug: std::env::var_os("UIDEBUG").is_some(),
        pointer: (0.0, 0.0),
        b3at: None,
        lastframe: None,
        due: None,
        wheel: (0.0, 0.0),
    };
    let r = el.run_app(&mut app);
    if let Err(e) = r {
        eprintln!("ui: {e}");
        std::process::exit(1);
    }
}
