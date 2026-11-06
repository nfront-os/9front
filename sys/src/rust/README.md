# Rust on 9front: window system crates

For Rust programs in windows of wsys or rio (../9front-ui PLAN §6.1, stage 4b):
winit, raw-window-handle and softbuffer with backends of their own for 9front
(`target_os = "plan9"`), on top of `p9win`.

- `p9win` (MIT, ours, only std): the process's window. `draw`: a draw(3)
  connection of its own (the window by `/dev/winname`, looked up again after
  rio renames it; pixels of x8r8g8b8 loaded into it, through an image of
  those where the window has other channels); `Inputs`: readers of
  `/dev/mouse`, `/dev/kbd` (kbd(3): the keys down, characters) and `/dev/wctl`
  (the window changed), threads of their own; `cursor`: `/dev/cursor` (the
  shapes of SDL's and GLFW's plan9 backends, RGBA made two bits); `/dev/label`,
  `/dev/wctl`, `/dev/snarf`.
- `crates/`: crates.io's crates as published, changed for 9front where it
  says "9front" (`diff -r` against `~/.cargo/registry/src/*/<crate>` shows it):
  - `raw-window-handle-0.6.2`: `RawWindowHandle::Plan9` and
    `RawDisplayHandle::Plan9` (`src/plan9.rs`): the process's window, draw(3).
  - `winit-0.30.13`: `src/platform_impl/plan9` (after the orbital backend):
    one window a process; the readers' input in a queue the event loop waits
    on (a condition variable: user events, redraws, `WaitUntil`); keys from
    kbd(3)'s k/K, a key press waiting up to 30 ms for the character kbd(3)
    sends after it (its `text`); the pointer in window coordinates, the wheel,
    `CursorGrabMode::Locked` (back to the middle, motion as device events);
    size, place, hiding, maximizing and fullscreen through `/dev/wctl`, the
    window system's answer seen as a change; cursor icons and RGBA cursors.
    `set_visible(false)` (eframe until its first frame) hides the window but
    is neither minimized nor occluded (egui would not draw it); hidden by the
    window system's menu is minimized (`Occluded`). `run_on_demand` and
    `pump_events` (eframe) from one round of the loop and a wait.
    `build.rs` (`plan9_platform`), `platform_impl/mod.rs`, `event.rs`,
    `platform/mod.rs`, `Cargo.toml` (p9win) name it.
  - `softbuffer-0.4.8`: `src/backends/plan9.rs`: the buffer loaded into the
    window (only the damage) through p9win's draw; `Cargo.toml` (p9win).
    Without its default features (X11, Wayland, KMS).
  - `wgpu-30.0.1`, `wgpu-core-30.0.1`, `wgpu-hal-30.0.1`: wgpu's Vulkan backend
    on plan9 (`vulkan` cfg alias; wgpu-core's `vulkan` feature turns on
    wgpu-hal's itself, plan9 having none of the deps crates). wgpu-hal: ash
    without `loaded` (no dlopen), the entry from the static libvulkan.a's
    `vkGetInstanceProcAddr` (`#[link(name = "vulkan")]`: after wgpu-hal on
    the link line); `VK_PLAN9_surface` asked for and `RawWindowHandle::Plan9`
    made a surface of. Programs link `-l draw -l 9 -l fmt -l utf -l stdc++`
    (Mesa's C++, its plan9 WSI) through RUSTFLAGS. eframe (egui's framework)
    runs on it unchanged.

A crate that uses them patches crates.io to these paths and vendors the rest
(`scripts/rust/vendor.sh`: only what the plan9 target builds keeps its
sources); `tests/winit` (winittest, eguitest with hdrshot-egui-backend: egui_software_backend for egui 0.37, eframetest on wgpu).

Not yet: glutin (egui's GL renderer; EGL's plan9 platform is there), wgpu
on 386/arm64, several windows a process, IME, the clipboard of egui-winit
(`/dev/snarf`: p9win has it).
