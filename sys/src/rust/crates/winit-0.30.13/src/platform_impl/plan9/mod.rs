//! 9front: winit's plan9 backend (target_os = "plan9"): the process's window
//! of wsys or rio, the screen without either, through p9win
//! (../9front-rust/plan9/p9win): draw(3) for its place, /dev/mouse, /dev/kbd and
//! /dev/wctl read by threads of their own, /dev/wctl, /dev/label and
//! /dev/cursor written. One window a process; pixels come from softbuffer's
//! plan9 backend, or Vulkan's VK_PLAN9_surface and EGL's plan9 platform (the
//! raw window handle names the process's window).

use std::fmt::{self, Display, Formatter};
use std::sync::{Arc, OnceLock};

use smol_str::SmolStr;

use crate::dpi::{PhysicalPosition, PhysicalSize};
use crate::keyboard::Key;

pub(crate) use self::event_loop::{ActiveEventLoop, EventLoop, EventLoopProxy, OwnedDisplayHandle};
mod event_loop;

pub use self::window::Window;
mod window;

#[derive(Default, Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub(crate) struct PlatformSpecificEventLoopAttributes {}

/// the one window: its id is 1
#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct WindowId(u64);

impl WindowId {
    pub const fn dummy() -> Self {
        WindowId(0)
    }
}

impl From<WindowId> for u64 {
    fn from(id: WindowId) -> Self {
        id.0
    }
}

impl From<u64> for WindowId {
    fn from(id: u64) -> Self {
        Self(id)
    }
}

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct DeviceId;

impl DeviceId {
    pub const fn dummy() -> Self {
        DeviceId
    }
}

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub struct PlatformSpecificWindowAttributes;

#[derive(Clone, Debug)]
pub struct OsError(Arc<std::io::Error>);

impl OsError {
    fn new(error: std::io::Error) -> Self {
        Self(Arc::new(error))
    }
}

impl Display for OsError {
    fn fmt(&self, fmt: &mut Formatter<'_>) -> Result<(), fmt::Error> {
        self.0.fmt(fmt)
    }
}

pub(crate) use crate::cursor::{
    OnlyCursorImage as PlatformCustomCursor, OnlyCursorImageSource as PlatformCustomCursorSource,
};
pub(crate) use crate::icon::NoIcon as PlatformIcon;

/// the screen's size, once a draw(3) connection has seen it
static SCREEN: OnceLock<(u32, u32)> = OnceLock::new();

fn screen() -> (u32, u32) {
    *SCREEN.get_or_init(|| match p9win::draw::Draw::open() {
        Ok(d) => {
            let r = d.screen();
            (r.dx().max(0) as u32, r.dy().max(0) as u32)
        },
        Err(_) => (0, 0),
    })
}

/// draw(3)'s screen: one
#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
pub struct MonitorHandle;

impl MonitorHandle {
    pub fn name(&self) -> Option<String> {
        Some("screen".to_owned())
    }

    pub fn size(&self) -> PhysicalSize<u32> {
        screen().into()
    }

    pub fn position(&self) -> PhysicalPosition<i32> {
        (0, 0).into()
    }

    pub fn scale_factor(&self) -> f64 {
        1.0
    }

    pub fn refresh_rate_millihertz(&self) -> Option<u32> {
        None
    }

    pub fn video_modes(&self) -> impl Iterator<Item = VideoModeHandle> {
        // the mode the screen is in: modes are set elsewhere (aux/vga, kms)
        std::iter::once(VideoModeHandle {
            size: screen(),
            bit_depth: 32,
            refresh_rate_millihertz: 60000,
            monitor: self.clone(),
        })
    }
}

#[derive(Clone, Debug, Eq, Hash, PartialEq)]
pub struct VideoModeHandle {
    size: (u32, u32),
    bit_depth: u16,
    refresh_rate_millihertz: u32,
    monitor: MonitorHandle,
}

impl VideoModeHandle {
    pub fn size(&self) -> PhysicalSize<u32> {
        self.size.into()
    }

    pub fn bit_depth(&self) -> u16 {
        self.bit_depth
    }

    pub fn refresh_rate_millihertz(&self) -> u32 {
        self.refresh_rate_millihertz
    }

    pub fn monitor(&self) -> MonitorHandle {
        self.monitor.clone()
    }
}

#[derive(Debug, Clone, Eq, PartialEq, Hash)]
pub struct KeyEventExtra {
    pub key_without_modifiers: Key,
    pub text_with_all_modifiers: Option<SmolStr>,
}
