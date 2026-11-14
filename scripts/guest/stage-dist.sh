#!/bin/bash
# Stage the 9front distribution tree on the build host: what
# sys/lib/dist/mkfile copies onto an install disk (disk/mkfs -s /n/src9
# with distproto and 9bootproto), after its binds:
#
#	binds:V:
#		test -d /n/src9/.git && bind /n/src9 /n/src9/dist/9front
#		bind cfg /n/src9/cfg
#		bind mail/lib /n/src9/mail/lib
#		bind ndb /n/src9/lib/ndb
#		bind -a adm/timezone /n/src9/adm/timezone
#		bind usr /n/src9/usr
#
# The binds become a view of symlinks (dist/9front, the repository
# itself, is left out: the disk gets the system, not a second source
# tree); /sys/src holds the repository's files only, not the gcc build's
# objects in the working tree; scripts/guest/boot/gccproto adds /amd64/lib.gcc,
# the gcc build's archives (distproto ships /amd64/lib empty, as 9front
# does: libraries are built with mk on the installed system), and
# scripts/guest/boot/gnuproto /amd64/gnu, the native gcc and binutils of
# ../9front-gcc (its out/$objtype; GCC_OUT, if it exists), and
# scripts/guest/boot/rustproto /amd64/rust, Rust of ../9front-rust (its
# out/$objtype from scripts/install.sh; only if RUST_OUT is set: an optional
# install, otherwise from its archives on the installed system) and
# scripts/guest/boot/goproto /amd64/go, Go of ../9front-go (likewise: its
# out/$objtype from scripts/install.sh, only if GO_OUT is set);
# scripts/guest/proto.py expands the protos. Our
# binaries carry DWARF (8l writes none) and are stripped. The guest's install script
# (scripts/guest/boot/mkdisk.rc) reads the result as /n/src9.
#
# PROFILE (scripts/guest/profiles: minimal, gpu, ui, full; default full) leaves the
# files of the groups it does not keep off; DEV (a list of gcc, rust, go, e.g.
# DEV=gcc,rust; DEV= none) says which languages come along: gcc as /$objtype/gnu with
# /$objtype/lib.gcc, Rust from RUST_OUT (default its out/$objtype), Go from GO_OUT
# (likewise); Rust links with gcc, so rust takes gcc along. Without DEV, as before:
# gcc if it is built, Rust and Go if RUST_OUT, GO_OUT are set. MCP=1: the MCP server
# (MCP_DIR, tools/mcp) as /$objtype/bin/9front-mcp, built with nfront's Go.
#
# REPLICA_DIR (with REPLICA_KEY, REPLICA_URL): the stage is published there for
# pull(1), updates over HTTP (scripts/replica/publish.py; REPLICA_DIR is what a web
# server serves as REPLICA_URL, REPLICA_KEY the private key that signs its index,
# kept outside nfront), and the CD gets the client: /dist/replica (the URL, the
# public key, the replica's db and log as of this CD). dir.list: each path's
# mode and owners as the CD has them (scripts/guest/proto.py -l).
#
# usage: [objtype=386] [PROFILE=p] [DEV=list] [MCP=1] [REPLICA_DIR=d REPLICA_KEY=k REPLICA_URL=u]
#        scripts/guest/stage-dist.sh dir
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
OUT=${1:?usage: stage-dist.sh dir}
DIST=$ROOT/sys/lib/dist
VIEW=$(mktemp -d)
trap 'rm -rf "$VIEW"' EXIT

# view: every top-level entry of the tree, with the bound ones replaced
for f in "$ROOT"/*; do
	ln -s "$f" "$VIEW/$(basename "$f")"
done
rm -f "$VIEW/cfg" "$VIEW/usr" "$VIEW/mail" "$VIEW/lib" "$VIEW/adm"
ln -s "$DIST/cfg" "$VIEW/cfg"
ln -s "$DIST/usr" "$VIEW/usr"
mkdir "$VIEW/mail" "$VIEW/lib" "$VIEW/adm" "$VIEW/adm/timezone"
for f in "$ROOT"/mail/*; do [ -e "$f" ] && ln -s "$f" "$VIEW/mail/$(basename "$f")"; done
rm -f "$VIEW/mail/lib"; ln -s "$DIST/mail/lib" "$VIEW/mail/lib"
for f in "$ROOT"/lib/*; do ln -s "$f" "$VIEW/lib/$(basename "$f")"; done
rm -f "$VIEW/lib/ndb"; ln -s "$DIST/ndb" "$VIEW/lib/ndb"
for f in "$ROOT"/adm/*; do [ "$(basename "$f")" = timezone ] || ln -s "$f" "$VIEW/adm/$(basename "$f")"; done
# bind -a: the tree's zones first, then dist's
for f in "$DIST"/adm/timezone/* "$ROOT"/adm/timezone/*; do
	ln -sfn "$f" "$VIEW/adm/timezone/$(basename "$f")"
done
# /sys/src: the sources as the repository has them -- the working tree
# also holds the gcc build's objects, archives and generated files
rm "$VIEW/sys"
mkdir "$VIEW/sys"
for f in "$ROOT"/sys/*; do
	[ "$(basename "$f")" = src ] || ln -s "$f" "$VIEW/sys/$(basename "$f")"
done
git -C "$ROOT" ls-files -z sys/src | (cd "$VIEW" && xargs -0 -n 500 sh -c '
	for f; do mkdir -p "$(dirname "$f")"; ln -s "'"$ROOT"'/$f" "$f"; done' sh)
# generated files a 9front release tree has: libc/ucd's tables, which mk
# would make with a helper linked against a Plan 9 libc.a (the native
# /amd64/lib; the gcc build's archives are in /amd64/lib.gcc)
for f in "$ROOT"/sys/src/libc/ucd/rune*data; do
	ln -s "$f" "$VIEW/sys/src/libc/ucd/$(basename "$f")"
done

# the kernel where 9front installs it: amd64/9pc64 (386/9pc), the a.out
# the build's install step makes (scripts/build/elf2aout.py)
A=${objtype:-amd64}
case $A in amd64) K=9pc64 ;; 386) K=9pc ;; *) echo "stage-dist: objtype=$A unknown" >&2; exit 1 ;; esac
# the profile: the groups it keeps, the paths of the others (omit)
PROFILE=${PROFILE:-full}
PROFILES=$ROOT/scripts/guest/profiles
keep=$(awk -v p="$PROFILE" '$1 == "profile" && $2 == p { f = 1; for(i = 3; i <= NF; i++) print $i } END { exit !f }' "$PROFILES") ||
	{ echo "stage-dist: no profile $PROFILE (scripts/guest/profiles: $(awk '$1 == "profile" { printf " %s", $2 }' "$PROFILES"))" >&2; exit 1; }
has(){ case " "$keep" " in *" $1 "*) return 0;; esac; return 1; }
keep=$(echo $keep)
mapfile -t omit < <(awk -v keep=" $keep " '/^[a-z]/ && $1 != "profile" && index(keep, " " $1 " ") == 0 { print $2 }' "$PROFILES" | sed "s/\$objtype/$A/")
# the languages: DEV given, exactly those; else as before
if [ -n "${DEV+set}" ]; then
	dev=" $(echo "$DEV" | tr , ' ') "
	for l in $dev; do case $l in gcc|rust|go) ;; all) dev=" gcc rust go " ;; *) echo "stage-dist: DEV: no language $l (gcc, rust, go)" >&2; exit 1 ;; esac; done
	case $dev in *" rust "*) case $dev in *" gcc "*) ;; *) dev="$dev gcc "; echo "stage-dist: rust links with gcc, gcc comes along" >&2 ;; esac ;; esac
	case $dev in *" rust "*) RUST_OUT=${RUST_OUT:-$RUST_DIR/out/$A} ;; *) RUST_OUT= ;; esac
	case $dev in *" go "*) GO_OUT=${GO_OUT:-$GO_DIR/out/$A} ;; *) GO_OUT= ;; esac
fi
lang(){ [ -z "${DEV+set}" ] || case $dev in *" $1 "*) return 0;; *) return 1;; esac; }
langs="as built"
[ -z "${DEV+set}" ] || { langs=$(echo $dev); langs=${langs:-none}; }
echo "stage-dist: profile $PROFILE (groups: ${keep:-none}), languages: $langs" >&2
[ -s "$ROOT/$A/$K" ] || { echo "stage-dist: no $A/$K, run CC=gcc ARCH=$A sys/src/cmd/build/build.exe" >&2; exit 1; }
mkdir -p "$VIEW/.$A"
for f in "$ROOT"/$A/*; do ln -s "$f" "$VIEW/.$A/$(basename "$f")"; done
rm "$VIEW/$A"; mv "$VIEW/.$A" "$VIEW/$A"
# the native toolchain (../9front-gcc: build.sh native) as /$objtype/gnu
GCC_OUT=${GCC_OUT:-$GCC_DIR/out/$A}
# gccproto: /$objtype/lib.gcc, gcc's archives (with gcc); uiproto: the desktop's files
# of the main branch (TrueType fonts, themes, wbar's menu; the ui group)
PROTOS=()
lang gcc && PROTOS+=("$ROOT/scripts/guest/boot/gccproto") || omit+=("$A/lib.gcc")
has ui && PROTOS+=("$ROOT/scripts/guest/boot/uiproto")
if ! lang gcc; then
	:
elif [ -d "$GCC_OUT/$A/gnu" ]; then
	ln -s "$GCC_OUT/$A/gnu" "$VIEW/$A/gnu"
	PROTOS+=("$ROOT/scripts/guest/boot/gnuproto")
elif [ -n "${DEV+set}" ]; then
	echo "stage-dist: no $GCC_OUT/$A/gnu (DEV has gcc): build it, toolchain/gcc build.sh native" >&2; exit 1
else
	echo "stage-dist: no $GCC_OUT/$A/gnu, the distribution has no gcc" >&2
fi
# Rust only if asked (RUST_OUT=../9front-rust/out/$objtype, its scripts/install.sh): an
# optional install, else from its archives (rust-T.tgz, rust-tools-T.tgz) on the installed
# system; as /$objtype/rust, linking with the gcc above
if [ -n "$RUST_OUT" ]; then
	[ -d "$RUST_OUT/$A/rust" ] || { echo "stage-dist: no $RUST_OUT/$A/rust" >&2; exit 1; }
	ln -s "$RUST_OUT/$A/rust" "$VIEW/$A/rust"
	PROTOS+=("$ROOT/scripts/guest/boot/rustproto")
fi
# Go only if asked (GO_OUT=../9front-go/out/$objtype, its scripts/install.sh): an optional
# install, else from its archive (go-plan9-$objtype.tgz) on the installed system; as
# /$objtype/go. Its programs are a.out; the strip below leaves the tree alone
if [ -n "$GO_OUT" ]; then
	[ -d "$GO_OUT/$A/go" ] || { echo "stage-dist: no $GO_OUT/$A/go" >&2; exit 1; }
	ln -s "$GO_OUT/$A/go" "$VIEW/$A/go"
	PROTOS+=("$ROOT/scripts/guest/boot/goproto")
fi
# the Vulkan programs tests/gpu/build-vktest.sh built (vulkaninfo, vkcube,
# vktri; statically linked with Mesa's drivers) and drawspeed as /$objtype/vk
VK=$ROOT/tests/gpu/root/tests
if has gpu && [ -x "$VK/vulkaninfo" ]; then
	mkdir -p "$VIEW/$A/vk"
	for p in vulkaninfo vkcube vktri drawspeed; do
		[ -x "$VK/$p" ] && ln -s "$VK/$p" "$VIEW/$A/vk/$p"
	done
	PROTOS+=("$ROOT/scripts/guest/boot/vkproto")
fi
# tests/wayland's programs and scripts (its run.sh built them: wlsys,
# weston's examples, the Wayland tests) as /tests, for rc/bin/wlrun on a
# machine booted from the CD
WLT=$ROOT/tests/wayland/root/tests
if has ui && [ -x "$WLT/wlsys" ]; then
	rm -f "$VIEW/tests"
	mkdir "$VIEW/tests"
	for p in "$WLT"/*; do
		ln -s "$p" "$VIEW/tests/$(basename "$p")"
	done
	# the system's wsys, not the copy of the last tests/wayland run; uipeek
	# (tests/ui) for rc/bin/deskrun
	[ -x "$ROOT/$A/bin/wsys" ] && ln -sf "$ROOT/$A/bin/wsys" "$VIEW/tests/wsys"
	[ -x "$ROOT/tests/ui/root/tests/uipeek" ] && ln -s "$ROOT/tests/ui/root/tests/uipeek" "$VIEW/tests/uipeek"
	PROTOS+=("$ROOT/scripts/guest/boot/wlproto")
fi

rm -rf "$OUT"
mkdir -p "$OUT"
export objtype=$A
python3 "$ROOT/scripts/guest/proto.py" -s "$VIEW" -d "$OUT" -l "$OUT.list" \
	-e tmp -e lib/firmware -e n -e mnt \
	"$ROOT/sys/lib/sysconfig/proto/distproto" "$ROOT/sys/lib/sysconfig/proto/9bootproto" \
	"${PROTOS[@]}" \
	2> "$OUT.warnings" || { cat "$OUT.warnings"; exit 1; }
# the profile's omitted groups (scripts/guest/profiles), for check-install -x too
: > "$OUT.omitted"
for p in "${omit[@]}"; do
	echo "/$p" >> "$OUT.omitted"
	rm -rf "${OUT:?}/$p"
done
# and on the CD: what its profile left out (tests/iso checks a CD against
# it, whatever profile made it)
mkdir -p "$OUT/sys/lib/dist"
cp "$OUT.omitted" "$OUT/sys/lib/dist/omitted"
# the MCP server (MCP=1; MCP_DIR, tools/mcp): an agent on another machine works on the
# installed system over HTTP (tools/mcp/README.md). Built here with nfront's Go (GO_DIR)
# for objtype, with its example deny list as /sys/lib/mcp/deny (its README's -deny)
if [ "$MCP" = 1 ]; then
	go=${GO:-$GO_DIR/go/bin/go}
	[ -x "$go" ] || { echo "stage-dist: MCP=1: no Go ($go: toolchain/go scripts/build.sh)" >&2; exit 1; }
	[ -f "$MCP_DIR/main.go" ] || { echo "stage-dist: MCP=1: no MCP server in $MCP_DIR (dist/bin/repos mcp)" >&2; exit 1; }
	# -s -w: without Go's symbol table and DWARF (Go optimizes always; the
	# strip below takes ELF only, Go's plan9 programs are a.out)
	(cd "$MCP_DIR" && GOOS=plan9 GOARCH=$A CGO_ENABLED=0 "$go" build -trimpath "-ldflags=-s -w" -o "$OUT/$A/bin/9front-mcp" .)
	mkdir -p "$OUT/sys/lib/mcp"
	install -m 644 "$MCP_DIR/deny.example" "$OUT/sys/lib/mcp/deny"
	echo "stage-dist: the MCP server as /$A/bin/9front-mcp" >&2
fi
# the CA roots, with Rust or Go: /sys/lib/tls/ca.pem, where Go's crypto/x509 and our
# crates (rustls-native-certs, curl-sys) look; cargo and go reach crates.io and the
# module proxy over https with them. Mozilla's list as curl extracts it (MPL-2.0):
# like firmware a file of its own, not in the tree (DISTFILES, dist/distfiles.list)
if [ -n "$RUST_OUT" ] || [ -n "$GO_OUT" ]; then
	ca=$(ls "$DISTFILES"/cacert-*.pem 2>/dev/null | sort | tail -1)
	if [ -n "$ca" ]; then
		mkdir -p "$OUT/sys/lib/tls"
		install -m 644 "$ca" "$OUT/sys/lib/tls/ca.pem"
		echo "stage-dist: CA roots $(basename "$ca") as /sys/lib/tls/ca.pem" >&2
	else
		echo "stage-dist: no CA roots ($DISTFILES/cacert-*.pem, dist/bin/fetch): cargo and go cannot reach https" >&2
	fi
fi
# for the guest's disk/mkfs (scripts/guest/boot/mkdisk.rc); no proto lists it,
# so it stays off the disk
cat /dev/null "${PROTOS[@]}" > "$OUT/gccproto"
# not in /$objtype/go: its programs are a.out, its ELF files are test data of Go's std
# (debug/elf, runtime/pprof), which strip would change
find "$OUT" -path "$OUT/$A/go" -prune -o -type f -perm -u+x -exec sh -c 'file -b "$1" | grep -q ^ELF && strip "$1"' _ {} \;
# Mesa's libvulkan.a and the Linux code's archives (sys/src/linux:
# libamdgpu.a 340 MB, liblinux.a) without their debug information: with
# it disk/mk9660 fails, and the CD outgrows mkiso.sh's scratch disk
for f in "$OUT"/$A/lib.gcc/ape/libvulkan.a "$OUT"/$A/lib.gcc/libamdgpu.a "$OUT"/$A/lib.gcc/liblinux.a "$OUT"/$A/lib.gcc/libkpi*.a; do
	[ -f "$f" ] && strip -g "$f"
done
# the replica, last: what it publishes is the CD's tree as it is now
if [ -n "$REPLICA_DIR" ]; then
	[ -n "$REPLICA_KEY" ] && [ -n "$REPLICA_URL" ] || { echo "stage-dist: REPLICA_DIR needs REPLICA_KEY and REPLICA_URL" >&2; exit 1; }
	python3 "$ROOT/scripts/replica/publish.py" -k "$REPLICA_KEY" -c "$REPLICA_URL" "$OUT" "$OUT.list" "$REPLICA_DIR"
	cat "$ROOT/scripts/guest/boot/replicaproto" >> "$OUT/gccproto"
	echo "stage-dist: published in $REPLICA_DIR for $REPLICA_URL (pull(1))" >&2
fi
echo "$(grep -c . "$OUT.warnings") proto warnings ($OUT.warnings)"
# everything mk install installs made it into the distribution
python3 "$ROOT/scripts/build/check-install.py" -d "$OUT" -x "$OUT.omitted" | grep -v '^broken' | tail -20
[ "${PIPESTATUS[0]}" = 0 ] || { echo "stage-dist: distribution incomplete" >&2; exit 1; }
du -sh "$OUT"
