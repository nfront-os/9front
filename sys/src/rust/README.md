# Rust crates changed for 9front (a copy)

The crates.io crates 9front changed for its window systems, and our own
`p9win`, as full sources, so that the tree builds them offline, on Linux and
on 9front itself (`build-self.sh`, cargo on 9front). **A copy:** the master
is ../9front-rust (`patches/crates/<crate>.patch` over crates.io's release,
`plan9/p9win`; its README: "Window system crates"). Change them there, then

    scripts/rust/crates.sh        # this copy again, from ../9front-rust/crates
    scripts/rust/crates.sh -n     # only say what differs (tests/static does)

`CRATES` names the crates (name-version); the layout is ../9front-rust's,
`crates/<crate>` and `plan9/p9win`, so the paths between them (winit and
softbuffer depend on p9win) are the same in both.

A crate of the tree uses them by `[patch.crates-io]` with paths here
(`tests/winit/Cargo.toml`) and vendors the rest (`scripts/rust/vendor.sh`).

- `raw-window-handle-0.6.2`: `RawWindowHandle::Plan9`, `RawDisplayHandle::Plan9`.
- `winit-0.30.13`: `src/platform_impl/plan9`, the process's window of wsys or rio.
- `softbuffer-0.4.8`: `src/backends/plan9.rs` (without its default features).
- `wgpu-30.0.1`, `wgpu-core-30.0.1`, `wgpu-hal-30.0.1`: Vulkan on plan9
  (the static libvulkan.a, `VK_PLAN9_surface`); programs link
  `-l draw -l 9 -l fmt -l utf -l stdc++`.
- `plan9/p9win` (MIT, ours): draw(3), `/dev/mouse`, `/dev/kbd`, `/dev/wctl`,
  `/dev/cursor`, label, wctl, snarf.
