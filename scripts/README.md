# scripts

Helpers of the gcc port. Run them from the tree root; most take
`objtype=386|amd64|arm64` from the environment (default amd64).
The build itself is `CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe`.

## gen/ — manifest generators

They read 9front's mkfiles and write the checked-in JSON manifests.
Run them after a mkfile changes (e.g. after rebasing onto `front`);
`tests/static/run.sh` checks that the manifests match.

| script | what it does |
|---|---|
| `genlibc.py` | libc manifest (`sys/src/libc/libc.json`, `<arch>/build.json`). `objtype=386 scripts/gen/genlibc.py` |
| `genlibs.py` | library manifests `sys/src/lib*/lib*.json`; `--list` prints the build order. `scripts/gen/genlibs.py libdraw` |
| `gencmds.py` | program manifests `cmds.json`; `--list` build order, `--probe` rebuilds `cmds-broken*.txt`. `scripts/gen/gencmds.py cmd/rio` |
| `genkernel.py` | kernel manifest from mkfile + CONF (pc, arm64; pc64's is hand-made). `scripts/gen/genkernel.py pc` |
| `genbuild.py` | top-level `build.json` and `build.<arch>.json`. `scripts/gen/genbuild.py` |
| `rustcrates.py` | the Rust crates of the tree (sys/src/cmd/*/rust.json: their programs), for genbuild.py and check-install.py. `scripts/gen/rustcrates.py` |
| `genmusl.py` | manifests of APE's musl part `sys/src/ape/lib/musl/musl[.386\|.arm64].json` (gcc only; sources per arch in `musl/<objtype>.list`); `--list` for genbuild. `scripts/gen/genmusl.py` |
| `genclean.py` | module: the files a manifest's Pre steps generate (its `Clean` key) |
| `arch.py` | module and tool: objtype, O, TOOLPREFIX, kernel conf, QEMU, manifest suffix. `objtype=386 python3 scripts/gen/arch.py` |
| `cmds-broken*.txt` | programs that do not build yet, per arch (written by `gencmds.py --probe`) |
| `i915-regs.py` | aux/intelgpu's register excerpts: the definitions of Linux's i915 the driver uses, verbatim from a Linux tree into excerpts of their headers (`sys/src/cmd/aux/gpu/intel/UPSTREAM`, lines with `*`). `scripts/gen/i915-regs.py ~/Devel/linux-6.6` |

## build/ — steps the manifests run

Called by `build.exe` (Pre/Post steps); rarely run by hand.

| script | what it does |
|---|---|
| `prep-objtype.sh` | install directories and gcc's `libgcc.a` for `$objtype` |
| `gen-rebootcode-i.sh`, `gen-rebootcode-pc-i.sh`, `gen-apbootstrap-i.sh`, `gen-etherm10g-i.sh`, `gen-sd53c8xx-i.sh`, `gen-initcode-i.sh` | the `.i` files the kernel includes, as the kernel mkfiles make them |
| `kheader.sh` | a kernel header that only includes another dir's (bcm64's mkfile rules) |
| `gen-bootfs.sh` | `/boot/boot` and `$CONF.bootfs.paq` embedded in the kernel. `scripts/build/gen-bootfs.sh pc64` |
| `gen-src.sh` | generated sources of single programs (Pre step in their directory) |
| `gen-9syscall.sh` | libc's system call stubs (GAS). `objtype=386 scripts/build/gen-9syscall.sh` |
| `libp9.sh` | `$objtype/lib.gcc/libp9.a`: the native libc with its symbols renamed `__p9_*`, for programs with APE in them too (Rust). libc's Post step |
| `musl-localize.sh` | in libap's musl members, the names libap's own members define too become local (i386 floor.s: ceil). Post step of the musl manifest |
| `gen-musl-bits.sh` | musl's `bits/alltypes.h` and `bits/syscall.h` for APE's musl part, as musl's Makefile makes them. `scripts/build/gen-musl-bits.sh x86_64` |
| `gen-mpc.sh`, `gen-runetype.sh`, `gen-latin1-h.sh`, `gen-jpg-tables.sh`, `gen-awk-proctab.sh`, `gen-upas-yacc.sh` | sources made by host tools (mpc, mkrunetype, mklatin, ...) as the mkfiles do |
| `yacc.sh`, `lex.sh` | 9front's yacc/lex templates with plan9port's tools |
| `mkinstall.py` | the install rules of the mkfiles (scripts, data files) |
| `elf2aout.py` | Plan 9 a.out of the ELF kernel, where 9boot loads it. `scripts/build/elf2aout.py amd64/bin/9pc64 amd64/9pc64` |
| `check-install.py` | is everything `mk install` installs there? `-r` removes stale files, `-i x.iso` checks a CD. `scripts/build/check-install.py` |
| `clean.sh` | remove the build's files from the source dirs (`-i`: installed files too). `scripts/build/clean.sh` |
| `cargo.sh` | a Rust crate of the tree (its programs in `rust.json`, `scripts/gen/rustcrates.py`; no network: `vendor/`), built with ../9front-rust's stage-1 rustc and linked against this tree; programs to `/$objtype/bin` (amd64); without Rust it skips (check-install: norust). build.exe's step `rust`; rc twin `rc/cargo.rc` (native cargo, rustc, gcc). `scripts/build/cargo.sh sys/src/cmd/wsys wsys` |
| `rc/*.rc` | the same steps in rc, for `build.exe` running on 9front itself (self-hosting): `build.exe` maps `bash …/X.sh` and `python3 …/X.py` to `rc …/rc/X.rc`. `lib.rc` holds the shared helpers (`hostcc`: helper programs with the native gcc, `bin2c`, `mkvar`) |

## hw/ — the hardware test stick (tests/hw/README.md)

| script | what it does |
|---|---|
| `mkstick.sh` | build the stick image (9fat with logs, hjfs root with the test runner and test programs) in a QEMU guest; `PACK=1`: a hand-off dir (packed image, collect.py, HOWTO.txt with the dd line). `scripts/hw/mkstick.sh [image]` |
| `collect.py` | copy `hwtest/` from the stick's 9fat (or the image) and print the summaries. `scripts/hw/collect.py /dev/sdX [dir]` |

## guest/ — QEMU guest, install media, boot loaders

| script | what it does |
|---|---|
| `boot-cli.sh` | boot the built kernel in QEMU; with arguments, type them and print the output. `scripts/guest/boot-cli.sh 'ls /bin'` |
| `vfio-gpu.sh` | the host's GPU for a guest (single-GPU VFIO): `sudo scripts/guest/vfio-gpu.sh bind [PID]` (GPU and HDMI audio to vfio-pci, /dev/vfio/N for the user, memlock of PID unlimited), `release` (back to amdgpu), `status`. `scripts/guest/vfio-gpu.sh` |
| `vfio-test.sh` | boots the build's kernel with the card passed through (KVM, `-cpu host`) and lists what the guest sees of it (`pci -bv`, configuration space). `scripts/guest/vfio-test.sh [cmds]` |
| `guest-cmd.sh` | one command line into a guest kept running (`HOLD=1 boot-cli.sh`). `scripts/guest/guest-cmd.sh 'ps'` |
| `mcp-guest.sh` | a guest (`HOLD=1 boot-cli.sh`) running ../9front-mcp's server, reachable from the host on 127.0.0.1:18091 (second NIC); `-disks`, no deny list. `scripts/guest/mcp-guest.sh aux/virtiogpu` |
| `mcp.py` | commands and files in that guest over HTTP, much faster than typing on the serial line. `scripts/guest/mcp.py wait; scripts/guest/mcp.py rc 'ps'` |
| `build-boot.sh` | build 9front's boot loaders in the guest with its own compilers (mbr, pbs, 9boot*, efi). `scripts/guest/build-boot.sh` |
| `build-kencc.sh` | check that the tree still builds with 9front's own compilers: in the guest `mk install` of `/sys/src` and the kernel pc64 with 6c/6a/6l (`boot/mkkencc.rc`). `scripts/guest/build-kencc.sh [kernel]` |
| `build-self.sh` | self-hosting: the guest builds the whole tree with `build.exe` and the native gcc of `../9front-gcc` (`boot/mkself.rc`) on an ext4 disk made from the copy in `$CLI_DIR/self.$objtype` (`mkfs.ext4 -d`, served by `ext4srv`, read back with `debugfs rdump`), then compares every object with the Linux build (`$CLI_DIR/self.diff`). `KEEP=1` reuses the copy. `[objtype=386] scripts/guest/build-self.sh [x.json ...]` |
| `build-uboot.sh` | u-boot for QEMU arm64 (used by `boot-cli.sh`) |
| `mkiso.sh` | install/live CD, made in the guest. `objtype=386 scripts/guest/mkiso.sh`; `PLAN9INI='monitor=i915'` sets lines of the CD's plan9.ini (a GPU driver as the screen from the start) |
| `mkdisk.sh` | installed disk image (hjfs). `scripts/guest/mkdisk.sh` |
| `stage-dist.sh` | the distribution tree the guest gets for mkiso/mkdisk |
| `gen-ufs-root.sh`, `proto.py` | root tree for the guest; files of a 9front proto |
| `boot/` | rc scripts run inside the guest (`mkboot.rc`, `mkiso.rc`, `mkdisk.rc`, `mkkencc.rc`, `mkself.rc`), `gccproto` (`/$objtype/lib.gcc`), `gnuproto` (`/$objtype/gnu`, the native gcc of `../9front-gcc`) `rustproto` (`/$objtype/rust`, Rust of `../9front-rust`, `scripts/install.sh` there; only with `RUST_OUT`) and `goproto` (`/$objtype/go`, Go of `../9front-go`, `scripts/install.sh` there; only with `GO_OUT`) |

## check/ — gcc vs. 9front's compilers

| script | what it does |
|---|---|
| `cmp-struct-layout.sh` | struct sizes against kencc's (long 32 bit, `#pragma pack`); no argument: all known file/wire formats. `scripts/check/cmp-struct-layout.sh` |
| `find-long-io.py` | candidates: `long` data written to or read from files, wire, devices. `scripts/check/find-long-io.py sys/src/cmd/cwfs` |
| `find-spinloops.sh` | busy-wait loops gcc made endless, in a kernel binary. `scripts/check/find-spinloops.sh amd64/bin/9pc64` |
| `find-shortened-members.py` | member paths shorter than in 9front (`&mc->Mouse` → `&mc`) |
| `find-added-casts.py`, `find-embedded-casts.py` | casts the port added, casts to embedded member types |
| `cmp-lp64.sh` | 6c/6a/6l output built as -m32 vs. -m64 host programs. `scripts/check/cmp-lp64.sh 6c file.c` |
| `i915-upstream.py` | aux/intelgpu and Linux's i915 (`UPSTREAM`): the hunks between two Linux trees that touch our functions, `-c` names still there, `-n` NOTICE's Linux part. `scripts/check/i915-upstream.py ~/Devel/linux-6.6 ~/Devel/linux-6.12` |

## fix/ — source rewriters for new upstream code

| script | what it does |
|---|---|
| `fixdesig.py` | add the `=` kencc lets designated initializers omit. `scripts/fix/fixdesig.py -n file.c` |
| `fixidents.py` | rename identifiers with runes gcc rejects. `scripts/fix/fixidents.py -n file.c` |
| `fixabsincludes.py` | `#include "/sys/..."` → relative path |
| `fixulongmax.py` | `x == ~0UL` with 32-bit ulong → `(ulong)~0UL` |
| `localize-pc-sources.py` | pc64 manifest: compile `../pc/*.c` as local copies |

## debug/ — looking into a running or hung guest

| script | what it does |
|---|---|
| `qemu-ctl.py` | keys, screendumps, monitor commands, and an rc on the serial line of a guest. `scripts/debug/qemu-ctl.py DIR ser 'ps -a'` |
| `thread-dump.py` | libthread procs and threads of a guest process with stacks. `scripts/debug/thread-dump.py DIR 307 amd64/bin/rio` |
| `shot2png.py` | the picture `tests/gpu/screenshot` prints in a guest (a passed-through GPU's screen, which QEMU cannot dump), out of a console log, as a PNG. `scripts/debug/shot2png.py ~/.cache/9front-cli/console.log shot.png` |
| `vncshot.py` | one frame from QEMU's VNC server as PNG, and colours at points: what the host shows, also with a GL display (virtio-gpu-gl), where QMP's screendump has no surface; `boot-cli.sh` uses it with `VNC=1`. `scripts/debug/vncshot.py SOCK out.png 200,450` |

## mesa/ — Mesa (venus Vulkan) for the gpu branch

Host-side tools for `sys/src/mesa` (release in `VERSION`, signing key in
`keys/`, new files in `overlay/`, changes as `patches/`, what APE lacks in
`plan9/`). Work directory `$MESA_WORK` (default `~/.cache/9front-gpu`);
cross compilers from `../9front-gcc`.

| script | what it does |
|---|---|
| `fetch.sh` | downloads the release tarball, checks sha256 and signature; a venv with meson and Mesa's python modules |
| `prepare.sh` | unpacks it as a git repository (tag `pristine`), commits the overlay (tag `overlay`), applies the patches (`git am`) |
| `patches.sh` | writes `sys/src/mesa/patches` from the commits after `overlay`, copies new files back to the overlay |
| `build.sh` | `libmesaplan9.a`, `libdrm.a` (`plan9/drm`, with pkg-config files for libdrm and libdrm_amdgpu), pkg-config files for libwayland/libffi/wayland-protocols (Wayland platform: Vulkan WSI, EGL; libwayland weakly via `plan9/include/plan9_wlweak.h`), the meson cross file (a changed one sets meson up anew), meson and ninja; the drivers (venus, hasvk, RADV) as one static archive `libvulkan_mesa.a`; the SDK `sdk/` (headers, `libvulkan.a`: the static loader of `sys/src/mesa/plan9/loader` with the drivers) |
| `vendor.sh` | Mesa's drivers (venus, hasvk, RADV) into the tree from the meson build (`vendor.py`): `sys/src/mesa/vendor` (sources, headers, meson's generated files), the Vulkan headers in `sys/include/ape`, one `mesa.json` per directory with meson's flags, `sys/src/mesa/build.json`: `build.exe` then builds `/$objtype/lib.gcc/ape/libvulkan.a` without meson or python, here with the cross gcc (`CC=x86_64-pc-plan9-gcc`) and on 9front with the native one |
| `vktools.sh` | Khronos' Vulkan-Tools (`sys/src/vktools`: version, patches): `prepare`, `patches`, `build` vkcube and vulkaninfo against the SDK |
| `common.sh` | settings the others source |

`tests/gpu/build-vktest.sh` links `tests/gpu/vktest.c` against it.

## linux/ — Linux's DRM drivers on the LinuxKPI for 9front

`sys/src/linux`: Linux's DRM code unchanged in `vendor/` (what `IMPORT` names, MIT-like files
only; `REJECTED` lists the others, `NOTICE` the notices), our layer in `kpi/` and `replace/`,
the kernel configuration in `kconfig`. The layer follows OpenBSD's LinuxKPI (`openbsd/`, unchanged,
at the commit in `OPENBSD`): `kpi/include` are OpenBSD's headers adapted to the unchanged Linux code
(changes marked `9front`), `kpi/obsd` the OpenBSD kernel API they build on (mutex, rwlock,
tsleep, timeout, task, pool, pci) on 9front, `kpi/linux/drm_linux.c` OpenBSD's; the rest of
`kpi/linux` are the 9front backends (pages, PCI on #G, IRQ proc, files, RCU).

| script | what it does |
|---|---|
| `import.py` | a Linux tree into `vendor/`, with the licence check, `VERSION`, `REJECTED`, `NOTICE`. `scripts/linux/import.py ~/Devel/linux-6.6` |
| `import-openbsd.py` | OpenBSD's LinuxKPI (its own headers, `drm_linux.c`, `linux_*.c`, its public-domain versions of GPL drm files, `sys/tree.h`, `sys/queue.h`) at the commit in `sys/src/linux/OPENBSD` (Linux 6.6.x era) unchanged into `sys/src/linux/openbsd/`, the template of `kpi/`; licence check, `openbsd/NOTICE`. `scripts/linux/import-openbsd.py ~/Devel/openbsd` |
| `kbuild.py` | the files Linux's Makefiles build with `kconfig`; `--autoconf` makes `kpi/include/generated/autoconf.h`. `scripts/linux/kbuild.py sys/src/linux/vendor drivers/gpu/drm` |
| `census.py` | compiles every chosen file (without those `OMIT` names) against the layer and counts what is missing: the measure of progress. `scripts/linux/census.py [-v]` |
| `depth.py` | past census' first error: stubs missing headers round after round, then counts every missing function, macro and type over all files (register names apart: rejected headers, files to OMIT); the measure of a new Linux version's KPI work. `scripts/linux/depth.py` |
| `undef.py` | compiles all (vendor, replace, kpi/linux, kpi/plan9), links them relocatably and lists the symbols nothing defines: what the layer still lacks. `scripts/linux/undef.py [outdir]` |
| `genbuild.py` | the build.exe manifests: `sys/src/linux/build.json` (libkpi9.a, libkpi.a, liblinux.a, one object per directory, -O2 as Linux) and `sys/src/cmd/aux/gpu/i915/build.json`; `-n` compares (tests/static). `scripts/linux/genbuild.py` |
| `gentrace.py` | no-op trace point headers (Linux's are GPL) for the `trace_*` the vendor code calls, with the originals' includes; i915's and drm's are OpenBSD's now (replace/include), for drivers OpenBSD has none for. `scripts/linux/gentrace.py` |
| `sdl3/vendor.sh` | SDL3 into the tree (sys/src/sdl3): the release (../9front-ui/src) with sys/src/sdl3/overlay over it, the files SOURCES names and their headers into vendor/, the public headers into /sys/include/ape/SDL3, build.json. `scripts/sdl3/vendor.sh` |
| `glfw/vendor.sh` | GLFW into the tree (sys/src/glfw): the release zip (../9front-ui/src) with sys/src/glfw/overlay over it, the files SOURCES names and their headers into vendor/, the public headers into /sys/include/ape/GLFW, build.json. `scripts/glfw/vendor.sh` |
| `libffi/vendor.sh` | libffi into the tree (sys/src/libffi): the release tarball (../9front-ui/src), the files SOURCES names and their headers into vendor/, ffi.h (from ffi.h.in, as configure for amd64) and ffitarget.h into /sys/include/ape, build.json (libffi.a). `scripts/libffi/vendor.sh` |
| `wayland/vendor.sh` | libwayland into the tree (sys/src/wayland): wayland and wayland-protocols (../9front-ui/src), the files SOURCES names into vendor/, the protocols' code from the host's wayland-scanner into gen/, the public headers into /sys/include/ape, build.json (libwayland-client.a, libwayland-server.a; both with the core and xdg-shell protocol code). `scripts/wayland/vendor.sh` |
| `wayland/build.sh` | libffi and libwayland (build.exe -D) and wlsys (sys/src/cmd/wlsys, the Wayland server in wsys) with the cross gcc; wlsys into the given directory (not in the full build yet). `scripts/wayland/build.sh outdir` |
| `rust/vendor.sh` | The crates.io crates of a Rust crate of the tree into its vendor/ (cargo vendor) for a build without network; only those the plan9 target builds keep their sources (no tests, benches, examples), the others become stubs. `scripts/rust/vendor.sh tests/winit` |
| `rust/crates.sh` | The Rust crates 9front changed (winit, softbuffer, raw-window-handle, wgpu, ...) and p9win into sys/src/rust, copied from ../9front-rust (its patches/crates are the master; `-n`: only compare, as tests/static does). `scripts/rust/crates.sh` |
