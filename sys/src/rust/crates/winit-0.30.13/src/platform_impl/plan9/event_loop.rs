//! 9front: the event loop of winit's plan9 backend. The readers of p9win
//! (threads) put what they read into a queue; the loop takes it out and makes
//! winit's events of it, and waits on the queue (a condition variable) for
//! more, for a user event, a redraw or the time.

use std::cell::Cell;
use std::collections::VecDeque;
use std::marker::PhantomData;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::{mpsc, Arc, Condvar, Mutex};
use std::time::{Duration, Instant};

use p9win::draw::{Draw, Rect};
use p9win::keys as k;
use p9win::{Input, Inputs};
use smol_str::SmolStr;

use crate::error::EventLoopError;
use crate::event::{self, Modifiers, StartCause};
use crate::event_loop::{self, ControlFlow, DeviceEvents};
use crate::platform::pump_events::PumpStatus;
use crate::keyboard::{
    Key, KeyCode, KeyLocation, ModifiersKeys, ModifiersState, NamedKey, NativeKey, NativeKeyCode,
    PhysicalKey,
};
use crate::window::{
    CursorGrabMode, CustomCursor as RootCustomCursor, CustomCursorSource, Theme,
    WindowId as RootWindowId,
};

use super::{
    DeviceId, KeyEventExtra, MonitorHandle, PlatformCustomCursor,
    PlatformSpecificEventLoopAttributes, WindowId,
};

/// a key press waits this long for the character it types (kbd(3) sends the
/// key, then the character as a message of its own)
const CHARWAIT: Duration = Duration::from_millis(30);

pub(super) const THE_WINDOW: WindowId = WindowId(1);

/// what the readers and the window put in for the loop
#[derive(Default)]
pub(super) struct Queue {
    pub inputs: VecDeque<Input>,
    pub wake: bool,
    pub redraw: bool,
    pub created: bool,
    pub destroyed: bool,
}

pub(super) struct Shared {
    pub queue: Mutex<Queue>,
    pub cv: Condvar,
    pub win: Mutex<Option<Arc<WinState>>>,
}

impl Shared {
    pub fn poke(&self, f: impl FnOnce(&mut Queue)) {
        let mut q = self.queue.lock().unwrap();
        f(&mut q);
        self.cv.notify_one();
    }
}

/// the window's cursor as the program set it
#[derive(Clone)]
pub(super) enum CursorShape {
    Arrow,
    Shape(p9win::cursor::Cursor),
}

pub(super) struct WinState {
    pub draw: Mutex<Draw>,
    pub inputs: Inputs,
    pub content: Mutex<Rect>,
    pub cursor: Mutex<CursorShape>,
    pub cursor_visible: AtomicBool,
    pub grab: Mutex<CursorGrabMode>,
    pub focused: AtomicBool,
    /// hidden by the window system (its menu: what a user minimizes)
    pub hidden: AtomicBool,
    /// hidden by the program (set_visible(false): eframe until its first
    /// frame); neither minimized nor occluded
    pub invisible: AtomicBool,
    /// before maximizing or fullscreen: the window's rectangle (with its border)
    pub restore: Mutex<Option<Rect>>,
    pub fullscreen: AtomicBool,
}

impl WinState {
    /// the cursor shown now: none, the program's, the arrow
    pub fn show_cursor(&self) {
        let r = if !self.cursor_visible.load(Ordering::Relaxed) {
            p9win::cursor::set(Some(&p9win::cursor::HIDDEN))
        } else {
            match &*self.cursor.lock().unwrap() {
                CursorShape::Arrow => p9win::cursor::set(None),
                CursorShape::Shape(c) => p9win::cursor::set(Some(c)),
            }
        };
        if let Err(e) = r {
            tracing::warn!("plan9: /dev/cursor: {e}");
        }
    }

    /// the window looked up again: its contents now
    pub fn relook(&self) -> Rect {
        let mut d = self.draw.lock().unwrap();
        if let Err(e) = d.attach() {
            tracing::warn!("plan9: the window: {e}");
        }
        let c = d.content();
        *self.content.lock().unwrap() = c;
        c
    }
}

/// a key of kbd(3), by the rune it has without modifiers (key.b in kbdfs)
fn keycode(r: char) -> (PhysicalKey, Option<NamedKey>) {
    let c = r.to_ascii_lowercase();
    let code = match c {
        'a'..='z' => {
            const L: [KeyCode; 26] = [
                KeyCode::KeyA, KeyCode::KeyB, KeyCode::KeyC, KeyCode::KeyD, KeyCode::KeyE,
                KeyCode::KeyF, KeyCode::KeyG, KeyCode::KeyH, KeyCode::KeyI, KeyCode::KeyJ,
                KeyCode::KeyK, KeyCode::KeyL, KeyCode::KeyM, KeyCode::KeyN, KeyCode::KeyO,
                KeyCode::KeyP, KeyCode::KeyQ, KeyCode::KeyR, KeyCode::KeyS, KeyCode::KeyT,
                KeyCode::KeyU, KeyCode::KeyV, KeyCode::KeyW, KeyCode::KeyX, KeyCode::KeyY,
                KeyCode::KeyZ,
            ];
            return (PhysicalKey::Code(L[(c as u8 - b'a') as usize]), None);
        },
        '0'..='9' => {
            const D: [KeyCode; 10] = [
                KeyCode::Digit0, KeyCode::Digit1, KeyCode::Digit2, KeyCode::Digit3,
                KeyCode::Digit4, KeyCode::Digit5, KeyCode::Digit6, KeyCode::Digit7,
                KeyCode::Digit8, KeyCode::Digit9,
            ];
            return (PhysicalKey::Code(D[(c as u8 - b'0') as usize]), None);
        },
        '-' => KeyCode::Minus,
        '=' => KeyCode::Equal,
        '[' => KeyCode::BracketLeft,
        ']' => KeyCode::BracketRight,
        '\\' => KeyCode::Backslash,
        ';' => KeyCode::Semicolon,
        '\'' => KeyCode::Quote,
        '`' => KeyCode::Backquote,
        ',' => KeyCode::Comma,
        '.' => KeyCode::Period,
        '/' => KeyCode::Slash,
        _ => {
            let n = c as u32;
            let (code, named) = match n {
                0x20 => (KeyCode::Space, NamedKey::Space),
                0x0a => (KeyCode::Enter, NamedKey::Enter),
                0x09 => (KeyCode::Tab, NamedKey::Tab),
                k::BS => (KeyCode::Backspace, NamedKey::Backspace),
                k::ESC => (KeyCode::Escape, NamedKey::Escape),
                k::DEL => (KeyCode::Delete, NamedKey::Delete),
                k::INS => (KeyCode::Insert, NamedKey::Insert),
                k::HOME => (KeyCode::Home, NamedKey::Home),
                k::END => (KeyCode::End, NamedKey::End),
                k::PGUP => (KeyCode::PageUp, NamedKey::PageUp),
                k::PGDOWN => (KeyCode::PageDown, NamedKey::PageDown),
                k::UP => (KeyCode::ArrowUp, NamedKey::ArrowUp),
                k::DOWN => (KeyCode::ArrowDown, NamedKey::ArrowDown),
                k::LEFT => (KeyCode::ArrowLeft, NamedKey::ArrowLeft),
                k::RIGHT => (KeyCode::ArrowRight, NamedKey::ArrowRight),
                k::SHIFT => (KeyCode::ShiftLeft, NamedKey::Shift),
                k::CTL => (KeyCode::ControlLeft, NamedKey::Control),
                k::ALT => (KeyCode::AltLeft, NamedKey::Alt),
                k::ALTGR => (KeyCode::AltRight, NamedKey::AltGraph),
                k::MOD4 => (KeyCode::SuperLeft, NamedKey::Super),
                k::CAPS => (KeyCode::CapsLock, NamedKey::CapsLock),
                k::NUM => (KeyCode::NumLock, NamedKey::NumLock),
                k::SCROLL => (KeyCode::ScrollLock, NamedKey::ScrollLock),
                k::PRINT => (KeyCode::PrintScreen, NamedKey::PrintScreen),
                k::BREAK => (KeyCode::Pause, NamedKey::Pause),
                n if (k::KF + 1..=k::KF + 12).contains(&n) => {
                    const F: [(KeyCode, NamedKey); 12] = [
                        (KeyCode::F1, NamedKey::F1), (KeyCode::F2, NamedKey::F2),
                        (KeyCode::F3, NamedKey::F3), (KeyCode::F4, NamedKey::F4),
                        (KeyCode::F5, NamedKey::F5), (KeyCode::F6, NamedKey::F6),
                        (KeyCode::F7, NamedKey::F7), (KeyCode::F8, NamedKey::F8),
                        (KeyCode::F9, NamedKey::F9), (KeyCode::F10, NamedKey::F10),
                        (KeyCode::F11, NamedKey::F11), (KeyCode::F12, NamedKey::F12),
                    ];
                    F[(n - k::KF - 1) as usize]
                },
                _ => return (PhysicalKey::Unidentified(NativeKeyCode::Unidentified), None),
            };
            return (PhysicalKey::Code(code), Some(named));
        },
    };
    (PhysicalKey::Code(code), None)
}

/// a key that types (its press waits for the character)
fn types(r: char) -> bool {
    let n = r as u32;
    n >= 0x20 && n != k::DEL && !(k::KF..0xF900).contains(&n)
}

/// a character of kbd(3)'s c that is text: not a control character, not a
/// key's private rune
fn text(c: char) -> bool {
    types(c)
}

fn element_state(pressed: bool) -> event::ElementState {
    if pressed {
        event::ElementState::Pressed
    } else {
        event::ElementState::Released
    }
}

/// what the loop knows of the window's input
#[derive(Default)]
struct State {
    keys: Vec<char>,
    pending: Option<(char, Instant)>, // a key press waiting for its character
    held: Option<char>,               // the last key down that types
    buttons: u32,
    last: Option<(i32, i32)>, // the pointer on the screen
    inside: bool,
    content: Rect,
    theme: Option<String>, // the desktop's theme as last read (/dev/theme)
}

impl State {
    fn modifiers(&self) -> Modifiers {
        let mut state = ModifiersState::empty();
        let mut pressed = ModifiersKeys::empty();
        for &key in &self.keys {
            match key as u32 {
                k::SHIFT => {
                    state |= ModifiersState::SHIFT;
                    pressed |= ModifiersKeys::LSHIFT;
                },
                k::CTL => {
                    state |= ModifiersState::CONTROL;
                    pressed |= ModifiersKeys::LCONTROL;
                },
                k::ALT => {
                    state |= ModifiersState::ALT;
                    pressed |= ModifiersKeys::LALT;
                },
                k::ALTGR => {
                    state |= ModifiersState::ALT;
                    pressed |= ModifiersKeys::RALT;
                },
                k::MOD4 => {
                    state |= ModifiersState::SUPER;
                    pressed |= ModifiersKeys::LSUPER;
                },
                _ => {},
            }
        }
        Modifiers { state, pressed_mods: pressed }
    }
}

pub struct EventLoop<T: 'static> {
    window_target: event_loop::ActiveEventLoop,
    user_events_sender: mpsc::Sender<T>,
    user_events_receiver: mpsc::Receiver<T>,
    st: State,
    start_cause: StartCause,
    started: bool, // pump_events: Init sent
}

type Handler<'a, T> = dyn FnMut(event::Event<T>) + 'a;

fn window_event<T>(h: &mut Handler<'_, T>, e: event::WindowEvent) {
    h(event::Event::WindowEvent { window_id: RootWindowId(THE_WINDOW), event: e });
}

fn key_event<T>(
    h: &mut Handler<'_, T>,
    base: char,
    pressed: bool,
    typed: Option<char>,
    repeat: bool,
) {
    let (physical_key, named) = keycode(base);
    let (logical_key, key_without_modifiers, text) = match named {
        Some(n) => (Key::Named(n), Key::Named(n), None),
        None => {
            let c = typed.unwrap_or(base);
            let s: SmolStr = c.encode_utf8(&mut [0u8; 4]).into();
            let base: SmolStr = SmolStr::from_iter(base.to_lowercase());
            let text = if pressed { typed.filter(|&c| text(c)).map(|_| s.clone()) } else { None };
            (Key::Character(s), Key::Character(base), text)
        },
    };
    let text = match named {
        // what these type, as other platforms give it
        Some(NamedKey::Space) if pressed => Some(SmolStr::new(" ")),
        Some(NamedKey::Enter) if pressed => Some(SmolStr::new("\r")),
        Some(NamedKey::Tab) if pressed => Some(SmolStr::new("\t")),
        _ => text,
    };
    window_event(h, event::WindowEvent::KeyboardInput {
        device_id: event::DeviceId(DeviceId),
        event: event::KeyEvent {
            logical_key,
            physical_key,
            location: KeyLocation::Standard,
            state: element_state(pressed),
            repeat,
            text: text.clone(),
            platform_specific: KeyEventExtra {
                key_without_modifiers,
                text_with_all_modifiers: text,
            },
        },
        is_synthetic: false,
    });
}

impl<T: 'static> EventLoop<T> {
    pub(crate) fn new(_: &PlatformSpecificEventLoopAttributes) -> Result<Self, EventLoopError> {
        let (user_events_sender, user_events_receiver) = mpsc::channel();
        Ok(Self {
            window_target: event_loop::ActiveEventLoop {
                p: ActiveEventLoop {
                    control_flow: Cell::new(ControlFlow::default()),
                    exit: Cell::new(false),
                    shared: Arc::new(Shared {
                        queue: Mutex::new(Queue::default()),
                        cv: Condvar::new(),
                        win: Mutex::new(None),
                    }),
                },
                _marker: PhantomData,
            },
            user_events_sender,
            user_events_receiver,
            st: State::default(),
            start_cause: StartCause::Init,
            started: false,
        })
    }

    /// the window looked up again ('r': it changed, it is to be drawn again, as
    /// rio and wsys mean it: a new place, a new wsys, a new theme): Resized and
    /// Moved if it did, ThemeChanged if /dev/theme did, a redraw in any case
    fn relook(win: &WinState, st: &mut State, h: &mut Handler<'_, T>, shared: &Shared) {
        let c = win.relook();
        let old = st.content;
        st.content = c;
        if c.dx() != old.dx() || c.dy() != old.dy() {
            window_event(h, event::WindowEvent::Resized(
                (c.dx().max(0) as u32, c.dy().max(0) as u32).into(),
            ));
        }
        if let Some((theme, text)) = super::window::desktop_theme() {
            if st.theme.as_deref().is_some_and(|t| t != text) {
                window_event(h, event::WindowEvent::ThemeChanged(theme));
            }
            st.theme = Some(text);
        }
        shared.poke(|q| q.redraw = true);
        if c.x0 != old.x0 || c.y0 != old.y0 {
            window_event(h, event::WindowEvent::Moved((c.x0, c.y0).into()));
        }
    }

    /// the key press that waited for its character, without one
    fn flush_pending(st: &mut State, h: &mut Handler<'_, T>) {
        if let Some((key, _)) = st.pending.take() {
            key_event(h, key, true, None, false);
        }
    }

    fn input(
        win: &WinState,
        st: &mut State,
        input: Input,
        h: &mut Handler<'_, T>,
        shared: &Shared,
    ) {
        if !matches!(input, Input::Char(_)) {
            Self::flush_pending(st, h);
        }
        match input {
            Input::Mouse { x, y, buttons, resized } => {
                if resized {
                    Self::relook(win, st, h, shared);
                }
                let c = st.content;
                let inside = c.contains(x, y);
                if inside != st.inside {
                    st.inside = inside;
                    let device_id = event::DeviceId(DeviceId);
                    window_event(h, if inside {
                        event::WindowEvent::CursorEntered { device_id }
                    } else {
                        event::WindowEvent::CursorLeft { device_id }
                    });
                }
                if *win.grab.lock().unwrap() == CursorGrabMode::Locked {
                    // only the motion: the pointer back to the middle
                    let (cx, cy) = (c.x0 + c.dx() / 2, c.y0 + c.dy() / 2);
                    if (x, y) != (cx, cy) {
                        h(event::Event::DeviceEvent {
                            device_id: event::DeviceId(DeviceId),
                            event: event::DeviceEvent::MouseMotion {
                                delta: ((x - cx) as f64, (y - cy) as f64),
                            },
                        });
                        let _ = win.inputs.warp(cx, cy);
                    }
                    st.last = Some((cx, cy));
                } else if st.last != Some((x, y)) {
                    if let Some((lx, ly)) = st.last {
                        h(event::Event::DeviceEvent {
                            device_id: event::DeviceId(DeviceId),
                            event: event::DeviceEvent::MouseMotion {
                                delta: ((x - lx) as f64, (y - ly) as f64),
                            },
                        });
                    }
                    st.last = Some((x, y));
                    window_event(h, event::WindowEvent::CursorMoved {
                        device_id: event::DeviceId(DeviceId),
                        position: ((x - c.x0) as f64, (y - c.y0) as f64).into(),
                    });
                }
                let changed = buttons ^ st.buttons;
                for (bit, button) in [
                    (1, event::MouseButton::Left),
                    (2, event::MouseButton::Middle),
                    (4, event::MouseButton::Right),
                ] {
                    if changed & bit != 0 {
                        window_event(h, event::WindowEvent::MouseInput {
                            device_id: event::DeviceId(DeviceId),
                            state: element_state(buttons & bit != 0),
                            button,
                        });
                    }
                }
                // buttons 4 and 5: the wheel, one step each press
                for (bit, dy) in [(8, 1.0), (16, -1.0)] {
                    if changed & bit != 0 && buttons & bit != 0 {
                        window_event(h, event::WindowEvent::MouseWheel {
                            device_id: event::DeviceId(DeviceId),
                            delta: event::MouseScrollDelta::LineDelta(0.0, dy),
                            phase: event::TouchPhase::Moved,
                        });
                    }
                }
                st.buttons = buttons;
            },
            Input::Keys(now) => {
                let mods = st.modifiers();
                let old = std::mem::take(&mut st.keys);
                for &key in old.iter().filter(|key| !now.contains(key)) {
                    key_event(h, key, false, None, false);
                    if st.held == Some(key) {
                        st.held = None;
                    }
                }
                st.keys = old.iter().copied().filter(|x| now.contains(x)).collect();
                for &key in now.iter().filter(|key| !old.contains(key)) {
                    st.keys.push(key);
                    if types(key) {
                        Self::flush_pending(st, h);
                        st.pending = Some((key, Instant::now()));
                        st.held = Some(key);
                    } else {
                        key_event(h, key, true, None, false);
                    }
                }
                let m = st.modifiers();
                if m != mods {
                    window_event(h, event::WindowEvent::ModifiersChanged(m));
                }
            },
            Input::Char(c) => {
                if let Some((key, _)) = st.pending.take() {
                    key_event(h, key, true, Some(c), false);
                } else if let Some(key) = st.held {
                    // a key held down: kbdfs repeats its character
                    key_event(h, key, true, Some(c), true);
                } else if text(c) {
                    // composed (kbdfs' Alt sequences): a character without a key of its own
                    let s: SmolStr = c.encode_utf8(&mut [0u8; 4]).into();
                    for pressed in [true, false] {
                        window_event(h, event::WindowEvent::KeyboardInput {
                            device_id: event::DeviceId(DeviceId),
                            event: event::KeyEvent {
                                logical_key: Key::Character(s.clone()),
                                physical_key: PhysicalKey::Unidentified(
                                    NativeKeyCode::Unidentified,
                                ),
                                location: KeyLocation::Standard,
                                state: element_state(pressed),
                                repeat: false,
                                text: pressed.then(|| s.clone()),
                                platform_specific: KeyEventExtra {
                                    key_without_modifiers: Key::Unidentified(
                                        NativeKey::Unidentified,
                                    ),
                                    text_with_all_modifiers: pressed.then(|| s.clone()),
                                },
                            },
                            is_synthetic: false,
                        });
                    }
                } else if keycode(c).1.is_some() {
                    // a key's character without the key (written to /dev/kbdin:
                    // vncs, a test): Enter, Tab, Backspace, ... pressed and let go
                    key_event(h, c, true, Some(c), false);
                    key_event(h, c, false, None, false);
                }
            },
            Input::Wctl(line) => {
                // minx miny maxx maxy (with the border) current|notcurrent visible|hidden
                Self::relook(win, st, h, shared);
                let focused = line.contains("current") && !line.contains("notcurrent");
                if focused != win.focused.swap(focused, Ordering::Relaxed) {
                    window_event(h, event::WindowEvent::Focused(focused));
                }
                let hidden = line.contains("hidden");
                if hidden != win.hidden.swap(hidden, Ordering::Relaxed)
                    && !win.invisible.load(Ordering::Relaxed)
                {
                    window_event(h, event::WindowEvent::Occluded(hidden));
                }
            },
            Input::Closed => window_event(h, event::WindowEvent::CloseRequested),
        }
    }

    pub fn run<F>(mut self, event_handler: F) -> Result<(), EventLoopError>
    where
        F: FnMut(event::Event<T>, &event_loop::ActiveEventLoop),
    {
        self.run_on_demand(event_handler)
    }

    /// until the program asks to exit (again after that: from Init)
    pub fn run_on_demand<F>(&mut self, mut event_handler: F) -> Result<(), EventLoopError>
    where
        F: FnMut(event::Event<T>, &event_loop::ActiveEventLoop),
    {
        self.start_cause = StartCause::Init;
        loop {
            self.round(&mut event_handler);
            if self.window_target.p.exiting() {
                break;
            }
            self.wait(None);
        }
        event_handler(event::Event::LoopExiting, &self.window_target);
        Ok(())
    }

    /// one round, waiting first (not longer than timeout) unless it is the first
    pub fn pump_events<F>(&mut self, timeout: Option<Duration>, mut event_handler: F) -> PumpStatus
    where
        F: FnMut(event::Event<T>, &event_loop::ActiveEventLoop),
    {
        if !self.started {
            self.started = true;
            self.start_cause = StartCause::Init;
        } else {
            self.wait(Some(timeout.unwrap_or(Duration::MAX)));
        }
        self.round(&mut event_handler);
        if self.window_target.p.exiting() {
            event_handler(event::Event::LoopExiting, &self.window_target);
            self.started = false;
            return PumpStatus::Exit(0);
        }
        PumpStatus::Continue
    }

    /// what came: winit's events of it, from NewEvents to AboutToWait
    fn round<F>(&mut self, event_handler_inner: &mut F)
    where
        F: FnMut(event::Event<T>, &event_loop::ActiveEventLoop),
    {
        let shared = self.window_target.p.shared.clone();
        let st = &mut self.st;
        let start_cause = self.start_cause;
        let target = &self.window_target;
        let mut h = |e: event::Event<T>| event_handler_inner(e, target);
        h(event::Event::NewEvents(start_cause));
        if start_cause == StartCause::Init {
            h(event::Event::Resumed);
        }

        let (inputs, created, destroyed) = {
            let mut q = shared.queue.lock().unwrap();
            q.wake = false;
            (
                std::mem::take(&mut q.inputs),
                std::mem::take(&mut q.created),
                std::mem::take(&mut q.destroyed),
            )
        };
        let win = shared.win.lock().unwrap().clone();
        if let Some(win) = &win {
            if created {
                // the first size and place
                *st = State::default();
                st.content = *win.content.lock().unwrap();
                let c = st.content;
                window_event(&mut h, event::WindowEvent::Resized(
                    (c.dx().max(0) as u32, c.dy().max(0) as u32).into(),
                ));
                window_event(&mut h, event::WindowEvent::Moved((c.x0, c.y0).into()));
                window_event(&mut h, event::WindowEvent::Focused(true));
            }
            for input in inputs {
                Self::input(win, st, input, &mut h, &shared);
            }
            if matches!(st.pending, Some((_, t)) if t.elapsed() >= CHARWAIT) {
                Self::flush_pending(st, &mut h);
            }
        }
        if destroyed {
            window_event(&mut h, event::WindowEvent::Destroyed);
            *shared.win.lock().unwrap() = None;
        }

        while let Ok(event) = self.user_events_receiver.try_recv() {
            h(event::Event::UserEvent(event));
        }

        let redraw = std::mem::take(&mut shared.queue.lock().unwrap().redraw);
        if redraw && shared.win.lock().unwrap().is_some() {
            window_event(&mut h, event::WindowEvent::RedrawRequested);
        }

        h(event::Event::AboutToWait);
    }

    /// until something comes, as the control flow says (not longer than limit);
    /// the cause of the next round
    fn wait(&mut self, limit: Option<Duration>) {
        let shared = self.window_target.p.shared.clone();
        let start = Instant::now();
        let requested_resume = match self.window_target.p.control_flow() {
            ControlFlow::Poll => {
                self.start_cause = StartCause::Poll;
                return;
            },
            ControlFlow::Wait => None,
            ControlFlow::WaitUntil(instant) => Some(instant),
        };
        // a key press waiting for its character: not longer than that
        let mut until = match (requested_resume, self.st.pending) {
            (r, Some((_, t))) => Some(r.map_or(t + CHARWAIT, |r| r.min(t + CHARWAIT))),
            (r, None) => r,
        };
        if let Some(l) = limit.and_then(|l| start.checked_add(l)) {
            until = Some(until.map_or(l, |u| u.min(l)));
        }

        let mut q = shared.queue.lock().unwrap();
        let mut timed_out = false;
        while q.inputs.is_empty() && !q.wake && !q.redraw && !q.created && !q.destroyed {
            match until {
                None => q = shared.cv.wait(q).unwrap(),
                Some(until) => {
                    let now = Instant::now();
                    if now >= until {
                        timed_out = true;
                        break;
                    }
                    q = shared.cv.wait_timeout(q, until - now).unwrap().0;
                },
            }
        }
        drop(q);
        self.start_cause = match requested_resume {
            Some(requested_resume) if timed_out && Instant::now() >= requested_resume => {
                StartCause::ResumeTimeReached { start, requested_resume }
            },
            _ => StartCause::WaitCancelled { start, requested_resume },
        };
    }

    pub fn window_target(&self) -> &event_loop::ActiveEventLoop {
        &self.window_target
    }

    pub fn create_proxy(&self) -> EventLoopProxy<T> {
        EventLoopProxy {
            user_events_sender: self.user_events_sender.clone(),
            shared: self.window_target.p.shared.clone(),
        }
    }
}

pub struct EventLoopProxy<T: 'static> {
    user_events_sender: mpsc::Sender<T>,
    shared: Arc<Shared>,
}

impl<T> EventLoopProxy<T> {
    pub fn send_event(&self, event: T) -> Result<(), event_loop::EventLoopClosed<T>> {
        self.user_events_sender
            .send(event)
            .map_err(|mpsc::SendError(x)| event_loop::EventLoopClosed(x))?;
        self.shared.poke(|q| q.wake = true);
        Ok(())
    }
}

impl<T> Clone for EventLoopProxy<T> {
    fn clone(&self) -> Self {
        Self { user_events_sender: self.user_events_sender.clone(), shared: self.shared.clone() }
    }
}

impl<T> Unpin for EventLoopProxy<T> {}

pub struct ActiveEventLoop {
    control_flow: Cell<ControlFlow>,
    exit: Cell<bool>,
    pub(super) shared: Arc<Shared>,
}

impl ActiveEventLoop {
    pub fn create_custom_cursor(&self, source: CustomCursorSource) -> RootCustomCursor {
        RootCustomCursor { inner: PlatformCustomCursor(Arc::new(source.inner.0)) }
    }

    pub fn primary_monitor(&self) -> Option<MonitorHandle> {
        Some(MonitorHandle)
    }

    pub fn available_monitors(&self) -> VecDeque<MonitorHandle> {
        let mut v = VecDeque::with_capacity(1);
        v.push_back(MonitorHandle);
        v
    }

    #[inline]
    pub fn listen_device_events(&self, _allowed: DeviceEvents) {}

    #[inline]
    pub fn system_theme(&self) -> Option<Theme> {
        None
    }

    #[cfg(feature = "rwh_06")]
    #[inline]
    pub fn raw_display_handle_rwh_06(
        &self,
    ) -> Result<rwh_06::RawDisplayHandle, rwh_06::HandleError> {
        Ok(rwh_06::RawDisplayHandle::Plan9(rwh_06::Plan9DisplayHandle::new()))
    }

    pub fn set_control_flow(&self, control_flow: ControlFlow) {
        self.control_flow.set(control_flow)
    }

    pub fn control_flow(&self) -> ControlFlow {
        self.control_flow.get()
    }

    pub(crate) fn exit(&self) {
        self.exit.set(true);
    }

    pub(crate) fn exiting(&self) -> bool {
        self.exit.get()
    }

    pub(crate) fn clear_exit(&self) {
        self.exit.set(false);
    }

    pub(crate) fn owned_display_handle(&self) -> OwnedDisplayHandle {
        OwnedDisplayHandle
    }
}

#[derive(Clone)]
pub(crate) struct OwnedDisplayHandle;

impl OwnedDisplayHandle {
    #[cfg(feature = "rwh_06")]
    #[inline]
    pub fn raw_display_handle_rwh_06(
        &self,
    ) -> Result<rwh_06::RawDisplayHandle, rwh_06::HandleError> {
        Ok(rwh_06::Plan9DisplayHandle::new().into())
    }
}
