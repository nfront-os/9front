//! 9front: softbuffer's plan9 backend. The pixels (0x00RRGGBB, draw(3)'s
//! x8r8g8b8 in memory) go to the process's window of wsys or rio (the screen
//! without either) through a draw(3) connection of the surface's own
//! (p9win): loaded into the window where it is now, those of the damage only.

use crate::backend_interface::*;
use crate::error::InitError;
use crate::{util, Rect, SoftBufferError};
use p9win::draw::{Draw, Rect as DRect};
use raw_window_handle::{HasDisplayHandle, HasWindowHandle, RawWindowHandle};
use std::marker::PhantomData;
use std::num::NonZeroU32;

pub struct Plan9Impl<D, W> {
    draw: Draw,
    width: u32,
    height: u32,
    buffer: util::PixelBuffer,
    presented: bool,
    window_handle: W,
    _display: PhantomData<D>,
}

impl<D, W> std::fmt::Debug for Plan9Impl<D, W> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("Plan9Impl").field("width", &self.width).field("height", &self.height).finish()
    }
}

impl<D: HasDisplayHandle, W: HasWindowHandle> SurfaceInterface<D, W> for Plan9Impl<D, W> {
    type Context = D;
    type Buffer<'a>
        = BufferImpl<'a, D, W>
    where
        Self: 'a;

    fn new(window: W, _display: &D) -> Result<Self, InitError<W>> {
        let raw = window.window_handle()?.as_raw();
        let RawWindowHandle::Plan9(_) = raw else {
            return Err(InitError::Unsupported(window));
        };
        let draw = Draw::open().map_err(|e| {
            SoftBufferError::PlatformError(Some("plan9: draw(3)".into()), Some(Box::new(e)))
        })?;
        Ok(Self {
            draw,
            width: 0,
            height: 0,
            buffer: util::PixelBuffer(Vec::new()),
            presented: false,
            window_handle: window,
            _display: PhantomData,
        })
    }

    #[inline]
    fn window(&self) -> &W {
        &self.window_handle
    }

    fn resize(&mut self, width: NonZeroU32, height: NonZeroU32) -> Result<(), SoftBufferError> {
        let (width, height) = (width.get(), height.get());
        if width != self.width || height != self.height {
            self.presented = false;
            self.width = width;
            self.height = height;
            self.buffer = util::PixelBuffer(vec![0; width as usize * height as usize]);
        }
        Ok(())
    }

    fn buffer_mut(&mut self) -> Result<BufferImpl<'_, D, W>, SoftBufferError> {
        if self.width == 0 || self.height == 0 {
            return Err(SoftBufferError::PlatformError(
                Some("plan9: buffer_mut before resize".into()),
                None,
            ));
        }
        Ok(BufferImpl { imp: self })
    }

    fn fetch(&mut self) -> Result<Vec<u32>, SoftBufferError> {
        Ok(self.buffer.to_vec())
    }
}

#[derive(Debug)]
pub struct BufferImpl<'a, D, W> {
    imp: &'a mut Plan9Impl<D, W>,
}

impl<D: HasDisplayHandle, W: HasWindowHandle> BufferInterface for BufferImpl<'_, D, W> {
    fn width(&self) -> NonZeroU32 {
        NonZeroU32::new(self.imp.width).unwrap()
    }

    fn height(&self) -> NonZeroU32 {
        NonZeroU32::new(self.imp.height).unwrap()
    }

    #[inline]
    fn pixels(&self) -> &[u32] {
        &self.imp.buffer
    }

    #[inline]
    fn pixels_mut(&mut self) -> &mut [u32] {
        &mut self.imp.buffer
    }

    fn age(&self) -> u8 {
        // the buffer is kept: last frame's pixels
        if self.imp.presented { 1 } else { 0 }
    }

    fn present(self) -> Result<(), SoftBufferError> {
        self.present_with_damage(&[])
    }

    fn present_with_damage(self, damage: &[Rect]) -> Result<(), SoftBufferError> {
        let imp = self.imp;
        let damage: Vec<DRect> = damage
            .iter()
            .map(|r| {
                DRect::new(
                    r.x as i32,
                    r.y as i32,
                    r.x as i32 + r.width.get() as i32,
                    r.y as i32 + r.height.get() as i32,
                )
            })
            .collect();
        // the window where it is now (rio names it anew when it changes)
        let put = imp.draw.attach().and_then(|_| {
            imp.draw.put(&imp.buffer, imp.width as usize, imp.height as usize, &damage)
        });
        put.map_err(|e| {
            SoftBufferError::PlatformError(Some("plan9: draw(3)".into()), Some(Box::new(e)))
        })?;
        imp.presented = true;
        Ok(())
    }
}
