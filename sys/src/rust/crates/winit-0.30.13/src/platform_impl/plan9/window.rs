//! 9front: the window of winit's plan9 backend, the process's window of wsys
//! or rio (the screen without either). Size, place and hiding go to
//! /dev/wctl (rio(4)); the window system's answer comes back as a change the
//! readers see (/dev/mouse's r, a line of /dev/wctl).

use std::collections::VecDeque;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::{Arc, Mutex};

use p9win::cursor::{self, Shape};
use p9win::draw::{Draw, Rect};
use p9win::Inputs;

use crate::cursor::Cursor;
use crate::dpi::{PhysicalPosition, PhysicalSize, Position, Size};
use crate::platform_impl::Fullscreen;
use crate::window::{CursorGrabMode, CursorIcon, ImePurpose};
use crate::{error, window};

use super::event_loop::{CursorShape, Shared, WinState, THE_WINDOW};
use super::{ActiveEventLoop, MonitorHandle, OsError, WindowId};

pub struct Window {
    shared: Arc<Shared>,
    st: Arc<WinState>,
}

fn os(e: std::io::Error) -> error::OsError {
    os_error!(OsError::new(e))
}

fn wctl(msg: &str) {
    if let Err(e) = p9win::wctl(msg) {
        tracing::warn!("plan9: wctl {msg}: {e}");
    }
}

/// the shapes drawn in p9win for winit's cursor icons (the default: the arrow)
fn shape(icon: CursorIcon) -> Option<Shape> {
    Some(match icon {
        CursorIcon::Text | CursorIcon::VerticalText => Shape::Text,
        CursorIcon::Wait | CursorIcon::Progress => Shape::Wait,
        CursorIcon::Crosshair | CursorIcon::Cell => Shape::Crosshair,
        CursorIcon::Pointer | CursorIcon::Grab | CursorIcon::Grabbing => Shape::Pointer,
        CursorIcon::Move | CursorIcon::AllScroll | CursorIcon::AllResize => Shape::Move,
        CursorIcon::NotAllowed | CursorIcon::NoDrop => Shape::NotAllowed,
        CursorIcon::EResize | CursorIcon::WResize | CursorIcon::EwResize | CursorIcon::ColResize => {
            Shape::EwResize
        },
        CursorIcon::NResize | CursorIcon::SResize | CursorIcon::NsResize | CursorIcon::RowResize => {
            Shape::NsResize
        },
        CursorIcon::NeResize | CursorIcon::SwResize | CursorIcon::NeswResize => Shape::NeswResize,
        CursorIcon::NwResize | CursorIcon::SeResize | CursorIcon::NwseResize => Shape::NwseResize,
        _ => return None,
    })
}

impl Window {
    pub(crate) fn new(
        el: &ActiveEventLoop,
        attrs: window::WindowAttributes,
    ) -> Result<Self, error::OsError> {
        let shared = el.shared.clone();
        if shared.win.lock().unwrap().is_some() {
            return Err(os(std::io::Error::other(
                "plan9: one window a process (its window of the window system)",
            )));
        }
        let mut draw = Draw::open().map_err(os)?;
        let b = draw.border();
        let screen = draw.screen();
        let mut restore = None;
        let mut fullscreen = false;
        if attrs.fullscreen.is_some() {
            // the whole screen, the border outside it
            restore = Some(draw.content().inset(-b));
            fullscreen = true;
            wctl(&format!(
                "resize -r {} {} {} {}",
                screen.x0 - b,
                screen.y0 - b,
                screen.x1 + b,
                screen.y1 + b
            ));
        } else {
            if let Some(size) = attrs.inner_size {
                let (w, h): (u32, u32) = size.to_physical::<u32>(1.0).into();
                wctl(&format!("resize -dx {} -dy {}", w as i32 + 2 * b, h as i32 + 2 * b));
            }
            if let Some(pos) = attrs.position {
                let (x, y): (i32, i32) = pos.to_physical::<i32>(1.0).into();
                wctl(&format!("move -minx {x} -miny {y}"));
            }
        }
        draw.attach().map_err(os)?;
        let content = draw.content();
        if !attrs.title.is_empty() {
            let _ = p9win::set_label(&attrs.title);
        }

        let feed = shared.clone();
        let inputs = Inputs::start(move |input| feed.poke(|q| q.inputs.push_back(input)))
            .map_err(os)?;
        let st = Arc::new(WinState {
            draw: Mutex::new(draw),
            inputs,
            content: Mutex::new(content),
            cursor: Mutex::new(CursorShape::Arrow),
            cursor_visible: AtomicBool::new(true),
            grab: Mutex::new(CursorGrabMode::None),
            focused: AtomicBool::new(true),
            hidden: AtomicBool::new(false),
            restore: Mutex::new(restore),
            fullscreen: AtomicBool::new(fullscreen),
        });
        *shared.win.lock().unwrap() = Some(st.clone());
        let w = Window { shared: shared.clone(), st };
        w.set_cursor(attrs.cursor.clone());
        if attrs.maximized && !fullscreen {
            w.set_maximized(true);
        }
        if !attrs.visible {
            w.set_visible(false);
        }
        if attrs.active {
            wctl("current");
        }
        shared.poke(|q| {
            q.created = true;
            q.redraw = true;
        });
        Ok(w)
    }

    pub(crate) fn maybe_queue_on_main(&self, f: impl FnOnce(&Self) + Send + 'static) {
        f(self)
    }

    pub(crate) fn maybe_wait_on_main<R: Send>(&self, f: impl FnOnce(&Self) -> R + Send) -> R {
        f(self)
    }

    fn content(&self) -> Rect {
        *self.st.content.lock().unwrap()
    }

    fn border(&self) -> i32 {
        self.st.draw.lock().unwrap().border()
    }

    fn screen(&self) -> Rect {
        self.st.draw.lock().unwrap().screen()
    }

    #[inline]
    pub fn id(&self) -> WindowId {
        THE_WINDOW
    }

    #[inline]
    pub fn primary_monitor(&self) -> Option<MonitorHandle> {
        Some(MonitorHandle)
    }

    #[inline]
    pub fn available_monitors(&self) -> VecDeque<MonitorHandle> {
        let mut v = VecDeque::with_capacity(1);
        v.push_back(MonitorHandle);
        v
    }

    #[inline]
    pub fn current_monitor(&self) -> Option<MonitorHandle> {
        Some(MonitorHandle)
    }

    #[inline]
    pub fn scale_factor(&self) -> f64 {
        1.0
    }

    #[inline]
    pub fn request_redraw(&self) {
        self.shared.poke(|q| q.redraw = true);
    }

    #[inline]
    pub fn pre_present_notify(&self) {}

    #[inline]
    pub fn reset_dead_keys(&self) {}

    #[inline]
    pub fn inner_position(&self) -> Result<PhysicalPosition<i32>, error::NotSupportedError> {
        let c = self.content();
        Ok((c.x0, c.y0).into())
    }

    #[inline]
    pub fn outer_position(&self) -> Result<PhysicalPosition<i32>, error::NotSupportedError> {
        let (c, b) = (self.content(), self.border());
        Ok((c.x0 - b, c.y0 - b).into())
    }

    #[inline]
    pub fn set_outer_position(&self, position: Position) {
        let (x, y): (i32, i32) = position.to_physical::<i32>(1.0).into();
        wctl(&format!("move -minx {x} -miny {y}"));
        self.st.relook();
    }

    #[inline]
    pub fn inner_size(&self) -> PhysicalSize<u32> {
        let c = self.content();
        (c.dx().max(0) as u32, c.dy().max(0) as u32).into()
    }

    #[inline]
    pub fn request_inner_size(&self, size: Size) -> Option<PhysicalSize<u32>> {
        let (w, h): (u32, u32) = size.to_physical::<u32>(1.0).into();
        let b = self.border();
        wctl(&format!("resize -dx {} -dy {}", w as i32 + 2 * b, h as i32 + 2 * b));
        // the window system's answer comes as a resize the event loop sees
        None
    }

    #[inline]
    pub fn outer_size(&self) -> PhysicalSize<u32> {
        let (c, b) = (self.content(), self.border());
        ((c.dx() + 2 * b).max(0) as u32, (c.dy() + 2 * b).max(0) as u32).into()
    }

    #[inline]
    pub fn set_min_inner_size(&self, _: Option<Size>) {}

    #[inline]
    pub fn set_max_inner_size(&self, _: Option<Size>) {}

    #[inline]
    pub fn title(&self) -> String {
        p9win::label()
    }

    #[inline]
    pub fn set_title(&self, title: &str) {
        let _ = p9win::set_label(title);
    }

    #[inline]
    pub fn set_transparent(&self, _transparent: bool) {}

    #[inline]
    pub fn set_blur(&self, _blur: bool) {}

    #[inline]
    pub fn set_visible(&self, visible: bool) {
        wctl(if visible { "unhide" } else { "hide" });
        self.st.hidden.store(!visible, Ordering::Relaxed);
    }

    #[inline]
    pub fn is_visible(&self) -> Option<bool> {
        Some(!self.st.hidden.load(Ordering::Relaxed))
    }

    #[inline]
    pub fn resize_increments(&self) -> Option<PhysicalSize<u32>> {
        None
    }

    #[inline]
    pub fn set_resize_increments(&self, _increments: Option<Size>) {}

    #[inline]
    pub fn set_resizable(&self, _resizeable: bool) {}

    #[inline]
    pub fn is_resizable(&self) -> bool {
        true
    }

    #[inline]
    pub fn set_minimized(&self, minimized: bool) {
        self.set_visible(!minimized);
    }

    #[inline]
    pub fn is_minimized(&self) -> Option<bool> {
        Some(self.st.hidden.load(Ordering::Relaxed))
    }

    #[inline]
    pub fn set_maximized(&self, maximized: bool) {
        let mut restore = self.st.restore.lock().unwrap();
        if maximized && restore.is_none() {
            let s = self.screen();
            *restore = Some(self.content().inset(-self.border()));
            wctl(&format!("resize -r {} {} {} {}", s.x0, s.y0, s.x1, s.y1));
        } else if !maximized {
            if let Some(r) = restore.take() {
                wctl(&format!("resize -r {} {} {} {}", r.x0, r.y0, r.x1, r.y1));
            }
        }
        drop(restore);
        self.st.relook();
    }

    #[inline]
    pub fn is_maximized(&self) -> bool {
        self.st.restore.lock().unwrap().is_some() && !self.st.fullscreen.load(Ordering::Relaxed)
    }

    #[inline]
    pub(crate) fn set_fullscreen(&self, monitor: Option<Fullscreen>) {
        let mut restore = self.st.restore.lock().unwrap();
        if monitor.is_some() {
            let (s, b) = (self.screen(), self.border());
            if restore.is_none() {
                *restore = Some(self.content().inset(-b));
            }
            wctl(&format!("resize -r {} {} {} {}", s.x0 - b, s.y0 - b, s.x1 + b, s.y1 + b));
            self.st.fullscreen.store(true, Ordering::Relaxed);
        } else if let Some(r) = restore.take() {
            wctl(&format!("resize -r {} {} {} {}", r.x0, r.y0, r.x1, r.y1));
            self.st.fullscreen.store(false, Ordering::Relaxed);
        }
        drop(restore);
        self.st.relook();
    }

    #[inline]
    pub(crate) fn fullscreen(&self) -> Option<Fullscreen> {
        self.st.fullscreen.load(Ordering::Relaxed).then(|| Fullscreen::Borderless(Some(MonitorHandle)))
    }

    #[inline]
    pub fn set_decorations(&self, _decorations: bool) {}

    #[inline]
    pub fn is_decorated(&self) -> bool {
        self.border() > 0
    }

    #[inline]
    pub fn set_window_level(&self, _level: window::WindowLevel) {}

    #[inline]
    pub fn set_window_icon(&self, _window_icon: Option<crate::icon::Icon>) {}

    #[inline]
    pub fn set_ime_cursor_area(&self, _position: Position, _size: Size) {}

    #[inline]
    pub fn set_ime_allowed(&self, _allowed: bool) {}

    #[inline]
    pub fn set_ime_purpose(&self, _purpose: ImePurpose) {}

    #[inline]
    pub fn focus_window(&self) {
        wctl("current");
    }

    #[inline]
    pub fn request_user_attention(&self, _request_type: Option<window::UserAttentionType>) {}

    #[inline]
    pub fn set_cursor(&self, c: Cursor) {
        let shape = match c {
            Cursor::Icon(icon) => match shape(icon) {
                Some(s) => CursorShape::Shape(cursor::shape(s)),
                None => CursorShape::Arrow,
            },
            Cursor::Custom(c) => {
                let i = &c.inner.0;
                CursorShape::Shape(cursor::from_rgba(
                    &i.rgba,
                    i.width as usize,
                    i.height as usize,
                    i.hotspot_x as i32,
                    i.hotspot_y as i32,
                ))
            },
        };
        *self.st.cursor.lock().unwrap() = shape;
        self.st.show_cursor();
    }

    #[inline]
    pub fn set_cursor_position(&self, position: Position) -> Result<(), error::ExternalError> {
        let (x, y): (i32, i32) = position.to_physical::<i32>(1.0).into();
        let c = self.content();
        self.st
            .inputs
            .warp(c.x0 + x, c.y0 + y)
            .map_err(|e| error::ExternalError::Os(os(e)))
    }

    #[inline]
    pub fn set_cursor_grab(&self, mode: CursorGrabMode) -> Result<(), error::ExternalError> {
        // Locked: the pointer kept in the middle, its motion as device events;
        // Confined: nothing to confine it with
        *self.st.grab.lock().unwrap() = mode;
        if mode == CursorGrabMode::Locked {
            let c = self.content();
            let _ = self.st.inputs.warp(c.x0 + c.dx() / 2, c.y0 + c.dy() / 2);
        }
        Ok(())
    }

    #[inline]
    pub fn set_cursor_visible(&self, visible: bool) {
        self.st.cursor_visible.store(visible, Ordering::Relaxed);
        self.st.show_cursor();
    }

    #[inline]
    pub fn drag_window(&self) -> Result<(), error::ExternalError> {
        Err(error::ExternalError::NotSupported(error::NotSupportedError::new()))
    }

    #[inline]
    pub fn drag_resize_window(
        &self,
        _direction: window::ResizeDirection,
    ) -> Result<(), error::ExternalError> {
        Err(error::ExternalError::NotSupported(error::NotSupportedError::new()))
    }

    #[inline]
    pub fn show_window_menu(&self, _position: Position) {}

    #[inline]
    pub fn set_cursor_hittest(&self, _hittest: bool) -> Result<(), error::ExternalError> {
        Err(error::ExternalError::NotSupported(error::NotSupportedError::new()))
    }

    #[cfg(feature = "rwh_06")]
    #[inline]
    pub fn raw_window_handle_rwh_06(&self) -> Result<rwh_06::RawWindowHandle, rwh_06::HandleError> {
        Ok(rwh_06::RawWindowHandle::Plan9(rwh_06::Plan9WindowHandle::new()))
    }

    #[cfg(feature = "rwh_06")]
    #[inline]
    pub fn raw_display_handle_rwh_06(
        &self,
    ) -> Result<rwh_06::RawDisplayHandle, rwh_06::HandleError> {
        Ok(rwh_06::RawDisplayHandle::Plan9(rwh_06::Plan9DisplayHandle::new()))
    }

    #[inline]
    pub fn set_enabled_buttons(&self, _buttons: window::WindowButtons) {}

    #[inline]
    pub fn enabled_buttons(&self) -> window::WindowButtons {
        window::WindowButtons::all()
    }

    #[inline]
    pub fn theme(&self) -> Option<window::Theme> {
        None
    }

    #[inline]
    pub fn has_focus(&self) -> bool {
        self.st.focused.load(Ordering::Relaxed)
    }

    #[inline]
    pub fn set_theme(&self, _theme: Option<window::Theme>) {}

    pub fn set_content_protected(&self, _protected: bool) {}
}

impl Drop for Window {
    fn drop(&mut self) {
        self.shared.poke(|q| q.destroyed = true);
    }
}
