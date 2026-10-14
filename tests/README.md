# tests

Tests of the gcc port, by category. Run them from the tree root.

	bash tests/run.sh                  # all categories, in the order below
	bash tests/run.sh static build     # some of them
	objtype=386 bash tests/run.sh iso  # another architecture (default amd64)

Each category prints `PASS`/`FAIL`/`SKIP` lines and a summary; the
script exits non-zero if a category failed. A category whose
prerequisite is missing (no kernel, no CD, no OVMF) is skipped.

## Before testing

1. Build everything for the architecture:

		cd sys/src/cmd/build && GO111MODULE=off go build -o build.exe . && cd -
		CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe

2. For `iso`, `disk`, `rio` and `usb` also 9front's boot loaders (built in a QEMU guest) and, for `rio`/`usb`, a CD (`scripts/guest/mkiso.sh`):

		scripts/guest/build-boot.sh

Host tools: gcc (and `aarch64-linux-musl-gcc` for arm64), Go, binutils,
plan9port, bison, bwrap, QEMU (KVM is used if `/dev/kvm` is writable),
grub-mkrescue, xorriso, OVMF for the UEFI cases, Harvey's `ufs` (root
file system of the guest: in `PATH` or `~/go/bin/ufs`).

## Categories

| category | what it tests | needs | time |
|---|---|---|---|
| `static` | source checks: manifests match their generators, no `ulong == ~0UL`, no unreviewed casts, member paths as in 9front, struct layouts of file/wire formats as kencc's (`scripts/check/cmp-struct-layout.sh`), GAS ports beside 9front's `.s` | nothing | seconds |
| `asm` | the GAS ports of libc's amd64 assembly, as host unit tests | gcc | seconds |
| `syscall` | the generated system call stubs (`scripts/build/gen-9syscall.sh`) | gcc | seconds |
| `build` | the build's results: libraries, programs, kernel; kernel without endless busy-waits (`scripts/check/find-spinloops.sh`) | a build | seconds |
| `guest` | unit tests built with the port and run inside 9front: libc, libthread, libmp, auth, pak, APE (`tests/guest/*.c`) | a build, QEMU | ~1 min |
| `boot` | boot to the console rc (cpu service), run commands | a build, QEMU | ~1 min |
| `gui` | terminal boot: VESA via aux/realemu, rio, typing into a window | a build, QEMU | ~1 min |
| `disk` | install image with hjfs (`scripts/guest/mkdisk.sh`), booted on its own | a build, boot loaders | minutes |
| `iso` | install/live CD (`scripts/guest/mkiso.sh`) booted as CD and as USB stick, BIOS and UEFI, up to rio, then typing into rio | a build, boot loaders, OVMF | minutes |
| `rio` | mouse and keyboard in rio on the CD: select text, type, New + sweep a window, type into it | a CD | ~1 min |
| `usb` | a USB stick on xHCI, EHCI, UHCI and OHCI: root hub in `#u`, the stick as `/dev/sdU*`, sector 0 read back | a CD | ~1 min per controller |

Options (environment):

- `ISO=file.iso` — `iso` tests this CD instead of building one; `rio` and `usb` use it, else the last CD `scripts/guest/mkiso.sh` made (`~/.cache/9front-cli/9front.$objtype.iso`). `objtype=386 ISO=~/.cache/9front-cli/9front.386.iso bash tests/run.sh iso`
- `USB_CTLRS="xhci ehci"` — `usb` tests only these controllers.
- `CPU`, `SMP`, `MEM` — QEMU's `-cpu`, `-smp`, `-m` for `rio` and `usb` (e.g. `CPU=n270 SMP=2` for an Atom netbook).
- `DISKIMG=file.img` — `disk` boots this image instead of building one.
- `GUI_SCREENSHOT=file.png` — `gui` keeps the screen dump.
- `KVM=0` — QEMU without KVM (TCG; slower, but checks instructions against the emulated CPU).

`static` is the quick check before every commit; it needs no build.

## By hand

- Boot the built kernel and get a shell on the serial console:

		scripts/guest/boot-cli.sh                 # interactive, Ctrl-A x quits
		scripts/guest/boot-cli.sh 'ls /bin' 'ps'  # type the lines, print the console

- The guest unit tests in a running system: `/tests/run [test...]`.
- `tests/vm.py`: the Python module `rio` and `usb` use — boots a CD to rio, keys and mouse through QMP, an rc on the serial line; a base for new guest tests.
- Boot a CD in QEMU like `iso` does and see where it stops: `tests/iso/drive.py`
  (`python3 tests/iso/drive.py file.iso cd bios workdir`; the screen dumps
  `screen.ppm` and `final.ppm` end up in `workdir`).
- A guest that hangs: `scripts/debug/qemu-ctl.py` and
  `scripts/debug/thread-dump.py` (see `scripts/README.md`).
- Real hardware: write the CD to a USB stick (`dd if=9front.amd64.iso of=/dev/sdX bs=1M`)
  or boot it as a CD.
