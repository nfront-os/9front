// SPDX-License-Identifier: MIT
//
// winittest: winit on 9front (its plan9 backend, sys/src/rust/crates/winit-*)
// in a window of wsys or rio: the window's first size and title (/dev/label),
// softbuffer's pixels (left half red, right half blue) read back from the
// window (/dev/window, image(6)), a resize asked for and the Resized that
// follows, cursors (/dev/cursor read back where wsys gives it: an icon, one
// of RGBA pixels, hidden, the default).
// -e secs: then the input events for secs seconds (keys, text, the mouse), a
// line each (the host types and moves the mouse: tests/winit/run.sh).
// Prints "ok ..." / "FAIL ..." lines and a summary.
//
// usage: winittest [-e secs]

use std::num::NonZeroU32;
use std::rc::Rc;
use std::time::{Duration, Instant};

use winit::application::ApplicationHandler;
use winit::dpi::PhysicalSize;
use winit::event::{ElementState, MouseScrollDelta, WindowEvent};
use winit::event_loop::{ActiveEventLoop, ControlFlow, EventLoop};
use winit::window::{CursorIcon, CustomCursor, Window, WindowId};

mod common;
use common::{readcursor, Report};

#[derive(PartialEq, Clone, Copy, Debug)]
enum Step {
    First,    // the first frame drawn: check it
    Resizing, // asked for a new size: wait for Resized
    Events,   // -e: the input
    Done,
}

struct App {
    r: Report,
    window: Option<Rc<Window>>,
    surface: Option<softbuffer::Surface<Rc<Window>, Rc<Window>>>,
    step: Step,
    size: (u32, u32),
    since: Instant,
    evsecs: u64,
}

impl App {
    fn draw(&mut self) {
        let (w, h) = self.size;
        let (Some(surface), Some(nw), Some(nh)) = (self.surface.as_mut(), NonZeroU32::new(w), NonZeroU32::new(h)) else {
            return;
        };
        if let Err(e) = surface.resize(nw, nh) {
            self.r.fail(&format!("softbuffer resize: {e}"));
            return;
        }
        let mut buf = match surface.buffer_mut() {
            Ok(b) => b,
            Err(e) => {
                self.r.fail(&format!("softbuffer buffer_mut: {e}"));
                return;
            },
        };
        for y in 0..h as usize {
            for x in 0..w as usize {
                buf[y * w as usize + x] = if x < w as usize / 2 { 0xff0000 } else { 0x0000ff };
            }
        }
        if let Err(e) = buf.present() {
            self.r.fail(&format!("softbuffer present: {e}"));
        }
    }

    fn cursors(&mut self, el: &ActiveEventLoop) {
        let win = self.window.clone().unwrap();
        win.set_cursor(CursorIcon::Crosshair);
        let Some(c) = readcursor() else {
            self.r.ok("cursor set; /dev/cursor not readable here (rio)");
            return;
        };
        let le = |o: usize| i32::from_le_bytes(c[o..o + 4].try_into().unwrap());
        if c.len() == 72 && le(0) == -7 && le(4) == -7 && c[40 + 2 * 7] == 0xf8 {
            self.r.ok("crosshair cursor in /dev/cursor");
        } else {
            self.r.fail(&format!("crosshair cursor: /dev/cursor has {} bytes", c.len()));
        }
        // 16x16 of RGBA: the top half black, the bottom half clear; hot spot 3 5
        let mut rgba = vec![0u8; 16 * 16 * 4];
        for p in rgba.chunks_mut(4).take(16 * 8) {
            p[3] = 255;
        }
        match CustomCursor::from_rgba(rgba, 16, 16, 3, 5) {
            Ok(src) => {
                win.set_cursor(el.create_custom_cursor(src));
                let c = readcursor().unwrap_or_default();
                let le = |o: usize| i32::from_le_bytes(c[o..o + 4].try_into().unwrap());
                if c.len() == 72 && le(0) == -3 && le(4) == -5 && c[40] == 0xff && c[40 + 2 * 8] == 0 && c[8] == 0 {
                    self.r.ok("custom cursor of RGBA pixels in /dev/cursor");
                } else {
                    self.r.fail("custom cursor: not the pixels'");
                }
            },
            Err(e) => self.r.fail(&format!("CustomCursor::from_rgba: {e}")),
        }
        win.set_cursor_visible(false);
        let c = readcursor().unwrap_or_default();
        if c.len() == 72 && c[8..].iter().all(|&b| b == 0) {
            self.r.ok("hidden cursor: no pixels");
        } else {
            self.r.fail(&format!("hidden cursor: /dev/cursor has {} bytes", c.len()));
        }
        win.set_cursor_visible(true);
        win.set_cursor(CursorIcon::Default);
        let c = readcursor().unwrap_or_default();
        if c.is_empty() {
            self.r.ok("default cursor: the window system's arrow");
        } else {
            self.r.fail(&format!("default cursor: /dev/cursor has {} bytes", c.len()));
        }
    }
}

impl ApplicationHandler for App {
    fn resumed(&mut self, el: &ActiveEventLoop) {
        if self.window.is_some() {
            return;
        }
        let attrs = Window::default_attributes()
            .with_title("winittest")
            .with_inner_size(PhysicalSize::new(320, 240));
        let win = match el.create_window(attrs) {
            Ok(w) => Rc::new(w),
            Err(e) => {
                self.r.fail(&format!("create_window: {e}"));
                el.exit();
                return;
            },
        };
        let ctx = softbuffer::Context::new(win.clone()).unwrap();
        match softbuffer::Surface::new(&ctx, win.clone()) {
            Ok(s) => self.surface = Some(s),
            Err(e) => self.r.fail(&format!("softbuffer surface: {e}")),
        }
        let s = win.inner_size();
        self.size = (s.width, s.height);
        if (s.width, s.height) == (320, 240) {
            self.r.ok("window 320x240");
        } else {
            self.r.fail(&format!("window {}x{}, want 320x240", s.width, s.height));
        }
        if common::label() == "winittest" {
            self.r.ok("title in /dev/label");
        } else {
            self.r.fail(&format!("/dev/label: {:?}", common::label()));
        }
        self.window = Some(win);
    }

    fn window_event(&mut self, el: &ActiveEventLoop, _id: WindowId, event: WindowEvent) {
        match event {
            WindowEvent::Resized(s) => {
                self.size = (s.width, s.height);
                if self.step == Step::Resizing && (s.width, s.height) == (360, 260) {
                    self.r.ok("resized to 360x260: Resized");
                    self.step = if self.evsecs > 0 { Step::Events } else { Step::Done };
                    self.since = Instant::now();
                    if self.step == Step::Events {
                        println!("ready for events");
                    }
                }
                if let Some(w) = &self.window {
                    w.request_redraw();
                }
            },
            WindowEvent::RedrawRequested => {
                self.draw();
                if self.step == Step::First {
                    std::thread::sleep(Duration::from_millis(100));
                    let (w, h) = (self.size.0 as i32, self.size.1 as i32);
                    common::check(&mut self.r, "surface left", w / 4, h / 2, 0xff0000);
                    common::check(&mut self.r, "surface right", 3 * w / 4, h / 2, 0x0000ff);
                    self.cursors(el);
                    self.step = Step::Resizing;
                    self.since = Instant::now();
                    let _ = self.window.as_ref().unwrap().request_inner_size(PhysicalSize::new(360, 260));
                }
            },
            WindowEvent::CloseRequested => el.exit(),
            _ if self.step != Step::Events => {},
            WindowEvent::KeyboardInput { event, .. } => {
                println!(
                    "event key {} {:?} {:?} text {:?}",
                    if event.state == ElementState::Pressed { "down" } else { "up" },
                    event.physical_key,
                    event.logical_key,
                    event.text.as_deref().unwrap_or("")
                );
            },
            WindowEvent::ModifiersChanged(m) => println!("event modifiers {:?}", m.state()),
            WindowEvent::CursorMoved { position, .. } => println!("event motion {:.0},{:.0}", position.x, position.y),
            WindowEvent::MouseInput { state, button, .. } => {
                println!("event button {button:?} {}", if state == ElementState::Pressed { "down" } else { "up" })
            },
            WindowEvent::MouseWheel { delta: MouseScrollDelta::LineDelta(_, y), .. } => println!("event wheel {y:.0}"),
            WindowEvent::CursorEntered { .. } => println!("event entered"),
            _ => {},
        }
    }

    fn about_to_wait(&mut self, el: &ActiveEventLoop) {
        match self.step {
            Step::Resizing if self.since.elapsed() > Duration::from_secs(3) => {
                self.r.fail(&format!("resize: no Resized to 360x260 (size {:?})", self.size));
                self.step = Step::Done;
            },
            Step::Events if self.since.elapsed() > Duration::from_secs(self.evsecs) => self.step = Step::Done,
            _ => {},
        }
        if self.step == Step::Done {
            el.exit();
        } else {
            el.set_control_flow(ControlFlow::WaitUntil(Instant::now() + Duration::from_millis(100)));
        }
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let evsecs = if args.len() > 2 && args[1] == "-e" { args[2].parse().unwrap_or(0) } else { 0 };
    let mut app = App {
        r: Report::default(),
        window: None,
        surface: None,
        step: Step::First,
        size: (0, 0),
        since: Instant::now(),
        evsecs,
    };
    match EventLoop::new() {
        Ok(el) => {
            if let Err(e) = el.run_app(&mut app) {
                app.r.fail(&format!("run_app: {e}"));
            }
        },
        Err(e) => app.r.fail(&format!("EventLoop::new: {e}")),
    }
    // the window gone before the summary (Drop of the surface, then the window)
    app.surface = None;
    app.window = None;
    std::process::exit(app.r.summary("winittest"));
}
