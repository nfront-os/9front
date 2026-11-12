# nfront: the 9front tree

This is the 9front tree of **nfront**, a distribution built on
[9front](https://9front.org) (upstream: https://github.com/9front/9front,
mirrored here as the branch `front`). The branch `gcc` builds 9front with
**gcc** instead of 9front's own compilers; `main` adds the GPU stack (gpu(4),
the LinuxKPI with Linux's DRM drivers, Mesa), the window system wsys and the
ui framework. Branches, toolchains and how to build everything:
https://github.com/nfront-os/nfront (and its wiki).

9front's own licence applies to its code (MIT); see the notices in the tree.

This README explains how to produce the install/live CD (ISO image).

## Prerequisites

- `gcc`, `go` (for the build tool), `git`, `bash`, `qemu` (used for the guest steps)
- The tree itself (a git checkout of this port)

## 1. Build the build tool (once)

```sh
cd sys/src/cmd/build && GO111MODULE=off go build -o build.exe .
cd -
```

## 2. Build the system

Builds everything for one architecture (libraries, commands, kernel):

```sh
CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe
```

(`ARCH=386` or `ARCH=arm64` for the other architectures.
Add `-D` to only rebuild what is out of date.)
A successful run ends with `NNN files expected, ok`.

## 3. Build the boot loaders

The ISO needs 9front's boot loaders (`mbr`, `pbs`, `9bootiso`, ...),
which are still built the 9front way (with 8c/6c/7c) — inside a QEMU
guest running the system built in step 2:

```sh
scripts/guest/build-boot.sh
```

The results are installed into `386/` and `arm64/`.

## 4. Make the ISO

```sh
scripts/guest/mkiso.sh
```

This stages the distribution tree, boots it in a QEMU guest and makes
the CD there (hybrid ISO: BIOS/El Torito + UEFI, also boots from disk or
USB). The result is written to
`~/.cache/9front-cli/9front.amd64.iso` (override the output as the first
argument). For the 386 image: `objtype=386 scripts/guest/mkiso.sh`.

That's it — burn or `dd` the ISO and boot it.

## Notes

- See `scripts/README.md` for what every script in `scripts/` does.
- See `MEMORY.md` for the status and open items of the port.
