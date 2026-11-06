// 9front: handles for 9front's window systems (rio, wsys) and draw(3).
// SPDX-License-Identifier: MIT OR Apache-2.0 OR Zlib (as the crate)

use super::DisplayHandle;

/// Raw display handle for 9front: draw(3) of the process's name space.
///
/// It names nothing: a process has one display, the draw(3) device its name
/// space has.
#[non_exhaustive]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Plan9DisplayHandle {}

impl Plan9DisplayHandle {
    /// Create a new display handle.
    ///
    /// # Example
    ///
    /// ```
    /// # use raw_window_handle::Plan9DisplayHandle;
    /// let handle = Plan9DisplayHandle::new();
    /// ```
    pub fn new() -> Self {
        Self {}
    }
}

impl DisplayHandle<'static> {
    /// Create a 9front display handle.
    ///
    /// As no data is borrowed by this handle, it is completely safe to create.
    pub fn plan9() -> Self {
        // SAFETY: No data is borrowed.
        unsafe { Self::borrow_raw(Plan9DisplayHandle::new().into()) }
    }
}

/// Raw window handle for 9front: the process's window.
///
/// A process of rio or wsys has one window, the one its name space's
/// `/dev/winname` names (the screen without a window system); the handle
/// stands for it. Vulkan's `VK_PLAN9_surface` and EGL's plan9 platform take
/// the same window.
#[non_exhaustive]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Plan9WindowHandle {}

impl Plan9WindowHandle {
    /// Create a new handle to the process's window.
    ///
    /// # Example
    ///
    /// ```
    /// # use raw_window_handle::Plan9WindowHandle;
    /// let handle = Plan9WindowHandle::new();
    /// ```
    pub fn new() -> Self {
        Self {}
    }
}
