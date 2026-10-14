# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository. `AGENTS.md` (included below) is the short version, kept compact for small local models; the details are here.

@AGENTS.md

## Branches
- `front`: mirror of upstream `github.com/9front/9front`, branch `front`. Treat as pristine 9front source; update it by fast-forward to `origin/front`, then rebase `gcc` onto it.
- `gcc` (work branch): the gcc port. Keep diffs against `front` minimal and reviewable. After a rebase, regenerate the manifests (upstream mkfile changes add sources) and run `tests/static`.

## gcc port status: MEMORY.md
`MEMORY.md` (repo root, German) is the checked-in log of the port: fixes per part ("Teil N"), open items, and porting notes. Check it before starting gcc-port work, update it when you land or find something.

## Build tool and manifests
- Go sources in `sys/src/cmd/build/` (`build.go`, `codegen.go`, `genheaders.go`, `paths.go`, `doc.go`); `build.exe` is built there and not checked in. Format of the manifests: `doc.go`.
- From `ARCH` the tool sets, unless already in the environment: `objtype`, `O` (8/6/7), `SUFFIX` (none for amd64, `.386`, `.arm64`: names of the per-arch manifests), `CONF` (pc, pc64, qemu), `TOOLPREFIX` (arm64: `aarch64-linux-musl-`), `CPUS`. `PLAN9` defaults to the git checkout. Without an argument it builds `build.json` of the current directory, else the tree's.
- `build.json` → `build.$ARCH.json` (from `scripts/genbuild.py`), in order: `prep` (`scripts/prep-objtype.sh`: install dirs, `libgcc.a`), libc, the libraries (`genlibs.py --list`), every `sys/src/lib*/klib.json` (kernel archives `klib*.a`), the programs (`gencmds.py --list`), `kernelgen` (`scripts/gen-*.sh`: rebootcode.i, apbootstrap.i, etherm10g/sd53c8xx .i, initcode, `/boot/boot` + `$CONF.bootfs.paq`), the kernel, `install` (`elf2aout.py`: the a.out where 9boot loads it; `check-install.py -r`). `Pre`/`Post` steps run without a shell (split on spaces), hence `bash scripts/...`. `-clean` runs only the `Projects` (`scripts/clean.sh`).
- Kernel manifests: `sys/src/9/pc64/build.json` (hand-made), `pc/build.json` and `arm64/build.json` (`scripts/genkernel.py`). `Config` holds the CONF sections, each element one CONF line; the tool generates `<conf>.c` (mkdevc + mkrootc), `errstr.h`, `../port/systab.h`. The `link` section calls only the first name's link function (as mkdevc).
- Library manifests: `sys/src/lib<x>/lib<x>[.arch].json` (userspace) and `klib.json` + `<arch>.json` (kernel); 9front's `reduce` (port file dropped when the machine dir has one) is replicated.
- Generators read the mkfiles: `genlibc.py`, `genlibs.py`, `gencmds.py` (`--probe` rebuilds `scripts/cmds-broken*.txt`), `genkernel.py`, `genbuild.py`; per arch with `objtype=386|arm64`. `tests/static` fails if a checked-in manifest differs from its generator.
- 9front's boot loaders are not built by gcc: `scripts/build-boot.sh` boots the built system and runs 9front's own mkfiles with 8c/6c/7c there.

## Guest, CD, tests
- `scripts/boot-cli.sh` boots the built kernel in QEMU (serial console, Harvey's ufs as root); `scripts/mkiso.sh` / `scripts/mkdisk.sh` make the install CD / disk image in the guest, as 9front's `sys/lib/dist/mkfile` does. Work dir `$CLI_DIR` (`~/.cache/9front-cli`).
- `tests/run.sh` categories: static, asm, syscall, build, guest, boot, gui, disk, iso. `tests/iso/drive.py` boots the CD to rio and types into it.

## Debugging a hung guest
- `scripts/port/qemu-ctl.py DIR type|ser|shot|hmp`: keys via QMP, screendumps, monitor commands, and a second rc on the serial line (QEMU needs `-qmp unix:DIR/qmp.sock,...` and the serial line as `DIR/serial.sock`; socket paths < 108 bytes).
- `scripts/port/thread-dump.py DIR PID BINARY`: libthread procs and threads of a guest process with stacks (needs the unstripped binary from `/$objtype/bin`).
- Kernel: `qemu-ctl.py DIR hmp 'gdbserver tcp::1234'`, then `gdb amd64/bin/9pc64` and `target remote :1234`.

## Checks for gcc-vs-kencc differences (scripts/port)
`cmp-struct-layout.sh` (struct sizes against a kencc reference: long 32 bit, pack honoured), `find-long-io.py` (long data in raw I/O), `find-spinloops.sh` (endless busy-waits in a kernel binary), `find-shortened-members.py` (member paths shorter than in 9front), `cmp-lp64.sh` (6c/6a/6l output under -m32 vs -m64), `fixulongmax.py`.

`.opencode/` at the repo root is unrelated tooling (opencode CLI agent), not part of the build.
