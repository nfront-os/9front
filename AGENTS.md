# AGENTS.md

## What this is
Full source tree of the **9front** operating system (a Plan 9 fork). Mirror of `github.com/9front/9front`, branch `front`. There is no README; the docs are the papers in `sys/doc/*.ms` (troff) and the man pages in `sys/man` (sections 1–8), which are the API source of truth.

## Layout (top levels mirror the running filesystem, not a "src/" layout)
- `386/`, `amd64/`, `arm/`, ... : one dir per arch. Each holds `include/u.h` and `include/ureg.h` (checked in per-arch, *not* in `sys/include`) plus a tiny `mkfile` giving that arch's compilers. Installed binaries land here (`/amd64/bin`, ...).
- `lib/` fonts and data; `rc/` rc-shell boot/init scripts (`rc/lib/rcmain`, `rc/bin`); `acme/` the editor; `adm/timezone` admin data; `sys/` everything else.
- `sys/src/` all source: `sys/src/lib*` (libc, libdraw, libsec, ...), `sys/src/cmd` + `sys/src/games` (userspace), `sys/src/9` (kernel), `sys/src/boot`, `sys/src/ape` (POSIX layer).
- `sys/include/` shared headers; compilers embed the include paths, so source writes bare `<u.h>`, `<libc.h>`.

## Build
- Build tool is Plan 9 **`mk`** (mkfile), **not** make. Mkfiles are **rc-shell syntax** (blocks `@{}`, `for(i in list)`, backquotes) — they are not sh.
- Every buildable dir has a `mkfile` that starts with `<$objtype/mkfile>` to get `CC/LD/O/AS`. `objtype` selects arch (suffix): 386=`8c`, amd64=`6c`, arm=`5c`, arm64=`7c`. Compile flags are Plan 9 C flags (`CFLAGS=-FTVw`), not gcc flags.
- Compilers live in `sys/src/cmd/cc` (front end) + per-arch drivers (`sys/src/cmd/8c`, `6c`, ...). Nothing here builds with gcc/make on other hosts; it only builds on a running 9front (or a 9p/Ape cross toolchain) — most work here is source-level edits.
- Common targets: in a dir, `mk` builds in place, `mk install` installs to `/386/bin`, `/sys/include`, etc. In `sys/src`: `mk all|install|clean|nuke`, plus `mk libs`, `mk kernels`, `mk release` (see `sys/src/mkfile`). Kernels: `cd sys/src/9 && mk all|install` loops over `sys/src/9/<port>`.
- Small single-file tools: put `<name>.c` directly in `sys/src/cmd/` — the aggregate `sys/src/cmd/mkfile` derives targets from `*.c` and provides `mk $O.<name>` / `mk <name>.install`. Multi-file programs get their own subdir + `mkfile`. Yacc grammars: `%.c %.tab.h` from `%.y`.
- Install media (ISO/qcow2): `sys/lib/dist/mkfile` (`mk` there; expects the tree mounted at `/n/src9`).

## Kernel
- `sys/src/9/port/` = arch-independent core; `sys/src/9/<port>/` per machine (pc, pc64, bcm, bcm64, arm64, ...), each a kernel config (`CONF`/`CONFLIST` in its `mkfile`).
- The device table `port/master` is **generated** from `dev*.c` plus `port/master.local` via `mk master` (a sam script scans `devtab` entries). Regenerate after adding/renaming drivers; don't hand-edit `master`.

## C dialect (Plan 9 C, differs from hosted C)
- Headers `<u.h>` + `<libc.h>`; no `<stdio.h>` outside `sys/src/ape`. `nil`, not NULL; `void main(int argc, char *argv[])`. Errors are set via `errstr` (`werrstr`) and read with `%r`; `sysfatal` for fatal errors.
- Code is old-style (tab-indented, often K&R declarations, no C99-isms); match the surrounding file. Keep `sys/man` in sync when changing a public API/header.

## GCC cross-build toolchain (work in progress; git branch `gcc`)
A port of Harvey's Go build tool lives in `sys/src/cmd/build/` (`build.go`, `codegen.go`, `genheaders.go`, `paths.go`, `doc.go`, `mkfile`). It builds the 9front kernels and their libraries **with gcc** instead of `mk`/8c/6c, driven by JSON manifests instead of mkfiles. Commits: `97b4440c8` (tool), `66d185072` (klib manifests).

### Manifests
- Root `build.json`: `Projects` = klib projects first (so archives build before the kernel link), then `/sys/src/9/pc`, `/sys/src/9/pc64`.
- Kernel: `sys/src/9/pc/build.json` and `sys/src/9/pc64/build.json`. Each entry is a kernel object: `Kernel`/`Newkern` name, `SourceFiles`, `Cflags`, `Oflags` (`-Ttext`, `-R`, `-N`), `Pre` (copies arch headers like `usbehci.h` into place), `Systab`, `ConfFile`, `Config`, `Ramfiles`. `Code`/`Dev`/`Link`/`Ip`/`Misc` each element is ONE complete CONF line (space-separated tokens); `buildkernel` turns them into the `<kern>.c` (mkdevc + mkrootc/data2c equivalent, incl. bootlinks and ramfile arrays inline), `configfile[]` = raw CONF bytes, `kerndate` = build-time epoch.
- Generated headers: `errstr.h` (from `../port/error.h`), `../port/systab.h` (from `sys.h`; sysctab keeps `_` entries, gaps stay nil) — done by `genheaders.go`; cached per arch as `<arch>.c` in `sys/src/9/port` via a `go:generate`-style stamp.
- Libraries: `sys/src/lib<name>/klib.json` (archive def: `Install /$ARCH/lib/`, `Library klib*.a`, `Include $ARCH.json`) + `sys/src/lib<name>/386.json` / `amd64.json` (Cflags + SourceFiles). The 9front `reduce` semantics are replicated: port sources whose basename also exists in the per-machine dir (`memcpy`, `mpvecadd`, `md5block`, ...) are dropped.

### Invoking on a dev host
- Build: `go build -o build.exe .` (needs `GO111MODULE=off`; no module).
- Run: `PLAN9=<tree root> ARCH=386|amd64 CPUS=N CC=gcc TOOLPREFIX= TOOLPFX= ./build.exe <manifest>`; `$ARCH` and `$PLAN9` are expanded in manifest paths at load time.
- Manifest generation is NOT automated in-tree: sources come from `C:\msys64\tmp\opencode\genmanifest.py` (kernel: CONF/parse) and `genklibs.py` (libs: mkfile OFILES + reduce). Re-run those, never hand-edit the JSONs. Configs: `CFG_386.json`, `CFG_amd64.json`.

### Status / gotchas
- The pipeline runs end-to-end (manifest → include merge → gcc compile; smoke-tested both arches); gcc-clean issues in kernel/library C are the follow-up: `USED` macro in `386/include/u.h` (uses `...`), duplicate `Lock` member in kernel `portdat.h`, K&R declarations.
- Plan 9 assembly (`.s`: `setjmp.s`, `tas.s`, `mpvecadd.s`, `aesni.s`, ...) is **not** in the manifests — needs a gas/yasm bridge or C fallbacks (libip keeps `ptclbsum.c` as the portable path).
- Local tool tests use mingw gcc 15 (no `-mcmodel=kernel`; temporarily sed to `-mcmodel=large`), so Cflags stay at the real value in manifests.
- On real 9front, plan9-`ranlib`/`strip`/GNU-binutils are absent; native builds need the gcc binutils available (not yet resolved).

### Porting rules
- Stick as close as possible to the original 9front source to make future syncs as easy as possible
- dont alter logic but you are allowed to pick a modern soluttion
- look at the original 9front build system to see if missing files or definitions are auto generated
- rewritting assembly to fit the gcc/clang system v api is a must
- keep helper scripts in ./scripts

### gcc Port Status
- Siehe `MEMORY.md` für aktuellen Stand, Todos und offene Punkte der gcc-Portierung

### GCC Port Rules (Originaltreue)
- **Never alter logic** — only fix compilation errors, never change behavior
- **Minimal changes only** — touch only what gcc complains about
- **Check original build system first** — if mkfile uses preprocessor (like `na` for sd53c8xx.n), don't hand-edit generated files
- **Preserve Plan9 idioms** — keep `nil`, `errstr`, `sysfatal`, old-style C, tab indentation
- **Document every deviation** — add `// GCC port: ...` comment explaining why change was needed
- **Prefer casts over rewrites** — e.g., `(void*)&ptr` instead of changing function signatures
- **Lock/Unlock fixes** — Plan9 uses `Lock*` type, gcc needs `&c->lock` address; cast only where 8c implicitly converts
- **Enum/struct scope** — gcc requires explicit enum names; add `enum { ... }` only if original 8c would accept it
- **Generated files** — files like `sd53c8xx.c` come from `sd53c8xx.n` via `na` tool; mark as "na-generated" in comments
- **Sync-ready** — every change must be reviewable as a diff against `main` branch; avoid reformatting
- **Test with gcc** — verify each fix compiles cleanly with `gcc -c` before moving on
- **No modern C** — no C99/C11 features, no `//` comments in code (keep `/* */`), no designated initializers
- **Headers first** — check `<u.h>`, `<libc.h>`, and arch-specific headers before adding new includes
- **mkfile awareness** — read `mkfile` in each dir to understand build dependencies before editing source
- **MEMORY.md updates** — log all changes and blockers in MEMORY.md for continuity
