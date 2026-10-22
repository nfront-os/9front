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
| `genmusl.py` | manifests of APE's musl part `sys/src/ape/lib/musl/musl[.386\|.arm64].json` (gcc only; sources per arch in `musl/<objtype>.list`); `--list` for genbuild. `scripts/gen/genmusl.py` |
| `genclean.py` | module: the files a manifest's Pre steps generate (its `Clean` key) |
| `arch.py` | module and tool: objtype, O, TOOLPREFIX, kernel conf, QEMU, manifest suffix. `objtype=386 python3 scripts/gen/arch.py` |
| `cmds-broken*.txt` | programs that do not build yet, per arch (written by `gencmds.py --probe`) |

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
| `guest-cmd.sh` | one command line into a guest kept running (`HOLD=1 boot-cli.sh`). `scripts/guest/guest-cmd.sh 'ps'` |
| `build-boot.sh` | build 9front's boot loaders in the guest with its own compilers (mbr, pbs, 9boot*, efi). `scripts/guest/build-boot.sh` |
| `build-kencc.sh` | check that the tree still builds with 9front's own compilers: in the guest `mk install` of `/sys/src` and the kernel pc64 with 6c/6a/6l (`boot/mkkencc.rc`). `scripts/guest/build-kencc.sh [kernel]` |
| `build-self.sh` | self-hosting: the guest builds the whole tree with `build.exe` and the native gcc of `../9front-gcc` (`boot/mkself.rc`) on an ext4 disk made from the copy in `$CLI_DIR/self.$objtype` (`mkfs.ext4 -d`, served by `ext4srv`, read back with `debugfs rdump`), then compares every object with the Linux build (`$CLI_DIR/self.diff`). `KEEP=1` reuses the copy. `[objtype=386] scripts/guest/build-self.sh [x.json ...]` |
| `build-uboot.sh` | u-boot for QEMU arm64 (used by `boot-cli.sh`) |
| `mkiso.sh` | install/live CD, made in the guest. `objtype=386 scripts/guest/mkiso.sh` |
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
| `build.sh` | `libmesaplan9.a`, the meson cross file, meson and ninja; the venus driver as one static archive `libvulkan_venus.a` |
| `common.sh` | settings the others source |

`tests/gpu/build-vktest.sh` links `tests/gpu/vktest.c` against it.
