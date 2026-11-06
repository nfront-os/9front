// SPDX-License-Identifier: MIT
//
// eguitest: egui on 9front: winit (its plan9 backend), egui-winit for the
// input, hdrshot-egui-backend (egui_software_backend ported to egui 0.37) drawing on the processor into softbuffer (its
// plan9 backend: draw(3)), in a window of wsys or rio. Checks that a frame
// reaches the window (/dev/window: egui's panel colour, and text on it),
// then -e secs: a button and a text field for the host (tests/winit/run.sh):
// "button at X Y" says where to click on the screen, each click and each
// change of the text a line.
// Prints "ok ..." / "FAIL ..." lines and a summary.
//
// usage: eguitest [-e secs]

use std::num::NonZeroU32;
use std::rc::Rc;
use std::time::{Duration, Instant};

use egui_software_backend::{BufferMutRef, ColorFieldOrder, EguiSoftwareRender};
use winit::application::ApplicationHandler;
use winit::event::WindowEvent;
use winit::event_loop::{ActiveEventLoop, ControlFlow, EventLoop};
use winit::window::{Window, WindowId};

mod common;
use common::Report;

struct Gui {
    window: Rc<Window>,
    surface: softbuffer::Surface<Rc<Window>, Rc<Window>>,
    ctx: egui::Context,
    state: egui_winit::State,
    render: EguiSoftwareRender,
}

struct App {
    r: Report,
    gui: Option<Gui>,
    frames: u32,
    checked: bool,
    evsecs: u64,
    start: Option<Instant>,
    clicks: u32,
    text: String,
    told: bool,
    focus: bool,
}

impl App {
    fn frame(&mut self) {
        let Some(g) = self.gui.as_mut() else { return };
        let size = g.window.inner_size();
        let (Some(w), Some(h)) = (NonZeroU32::new(size.width), NonZeroU32::new(size.height)) else {
            return;
        };
        let input = g.state.take_egui_input(&g.window);
        let (mut clicks, mut text, mut told, focus) = (self.clicks, self.text.clone(), self.told, self.focus);
        let origin = g.window.inner_position().map(|p| (p.x, p.y)).unwrap_or((0, 0));
        let out = g.ctx.run_ui(input, |ui| {
            egui::CentralPanel::default().show(ui, |ui| {
                ui.heading("egui on 9front");
                ui.label("winit, softbuffer: draw(3); egui_software_backend");
                let b = ui.button("press me");
                if b.clicked() {
                    clicks += 1;
                    println!("event clicked {clicks}");
                }
                if !told {
                    let c = b.rect.center();
                    println!("button at {} {}", origin.0 + c.x as i32, origin.1 + c.y as i32);
                    told = true;
                }
                let t = ui.text_edit_singleline(&mut text);
                // -e: the keys go to the field, the click on the button too
                if focus {
                    t.request_focus();
                }
                if t.changed() {
                    println!("event text {text}");
                }
            });
        });
        self.clicks = clicks;
        self.text = text;
        self.told = told;
        g.state.handle_platform_output(&g.window, out.platform_output);
        let prims = g.ctx.tessellate(out.shapes, out.pixels_per_point);
        if let Err(e) = g.surface.resize(w, h) {
            self.r.fail(&format!("softbuffer resize: {e}"));
            return;
        }
        let mut buf = match g.surface.buffer_mut() {
            Ok(b) => b,
            Err(e) => {
                self.r.fail(&format!("softbuffer buffer_mut: {e}"));
                return;
            },
        };
        let px: &mut [u32] = &mut buf;
        // softbuffer's 0x00RRGGBB is bytes B G R X: Bgra for the renderer
        let px4 = unsafe { std::slice::from_raw_parts_mut(px.as_mut_ptr() as *mut [u8; 4], px.len()) };
        px4.fill([0, 0, 0, 0]);
        g.render.render(
            &mut BufferMutRef::new(px4, w.get() as usize, h.get() as usize),
            &prims,
            &out.textures_delta,
            out.pixels_per_point,
        );
        if let Err(e) = buf.present() {
            self.r.fail(&format!("softbuffer present: {e}"));
        }
        self.frames += 1;
        let again = out.viewport_output.values().any(|v| v.repaint_delay.is_zero());
        if again {
            g.window.request_redraw();
        }
    }

    /// egui's panel in the window, text on it
    fn check(&mut self) {
        let Some(g) = self.gui.as_ref() else { return };
        let size = g.window.inner_size();
        let bg = g.ctx.global_style().visuals.panel_fill;
        let want = (bg.r() as u32) << 16 | (bg.g() as u32) << 8 | bg.b() as u32;
        common::check(&mut self.r, "egui's panel", size.width as i32 - 4, size.height as i32 - 4, want);
        // the heading's row: not all of it the panel's colour
        let mut other = 0;
        for x in (8..200).step_by(2) {
            for y in 8..30 {
                if matches!(common::pixel(x, y), Ok(p) if p != want) {
                    other += 1;
                }
            }
        }
        if other > 20 {
            self.r.ok(&format!("egui's heading drawn: {other} pixels of text"));
        } else {
            self.r.fail(&format!("egui's heading: {other} pixels of text"));
        }
    }
}

impl ApplicationHandler for App {
    fn resumed(&mut self, el: &ActiveEventLoop) {
        if self.gui.is_some() {
            return;
        }
        let window = match el.create_window(Window::default_attributes().with_title("eguitest")) {
            Ok(w) => Rc::new(w),
            Err(e) => {
                self.r.fail(&format!("create_window: {e}"));
                el.exit();
                return;
            },
        };
        let ctx = egui::Context::default();
        let state = egui_winit::State::new(ctx.clone(), egui::ViewportId::ROOT, &window, None, None, None);
        let sb = softbuffer::Context::new(window.clone()).unwrap();
        let surface = softbuffer::Surface::new(&sb, window.clone()).unwrap();
        self.gui = Some(Gui { window, surface, ctx, state, render: EguiSoftwareRender::new(ColorFieldOrder::Bgra) });
        self.r.ok("window, egui context, softbuffer surface");
    }

    fn window_event(&mut self, el: &ActiveEventLoop, _id: WindowId, event: WindowEvent) {
        let Some(g) = self.gui.as_mut() else { return };
        let resp = g.state.on_window_event(&g.window, &event);
        if resp.repaint {
            g.window.request_redraw();
        }
        match event {
            WindowEvent::RedrawRequested => {
                let t0 = Instant::now();
                self.frame();
                if self.frames == 3 && !self.checked {
                    self.r.ok(&format!("3 frames, the last in {:.0} ms", t0.elapsed().as_secs_f64() * 1e3));
                    std::thread::sleep(Duration::from_millis(100));
                    self.check();
                    self.checked = true;
                    if self.evsecs > 0 {
                        self.focus = true;
                        self.start = Some(Instant::now());
                        println!("ready for events");
                        self.gui.as_ref().unwrap().window.request_redraw();
                    }
                }
            },
            WindowEvent::CloseRequested => el.exit(),
            _ => {},
        }
    }

    fn about_to_wait(&mut self, el: &ActiveEventLoop) {
        if self.checked && self.start.is_none_or(|s| s.elapsed() > Duration::from_secs(self.evsecs)) {
            el.exit();
            return;
        }
        if self.frames < 3 {
            if let Some(g) = &self.gui {
                g.window.request_redraw();
            }
        }
        el.set_control_flow(ControlFlow::WaitUntil(Instant::now() + Duration::from_millis(100)));
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let evsecs = if args.len() > 2 && args[1] == "-e" { args[2].parse().unwrap_or(0) } else { 0 };
    let mut app = App {
        r: Report::default(),
        gui: None,
        frames: 0,
        checked: false,
        evsecs,
        start: None,
        clicks: 0,
        text: String::new(),
        told: false,
        focus: false,
    };
    match EventLoop::new() {
        Ok(el) => {
            if let Err(e) = el.run_app(&mut app) {
                app.r.fail(&format!("run_app: {e}"));
            }
        },
        Err(e) => app.r.fail(&format!("EventLoop::new: {e}")),
    }
    if app.evsecs > 0 {
        println!("clicks {} text {:?}", app.clicks, app.text);
    }
    app.gui = None;
    std::process::exit(app.r.summary("eguitest"));
}
