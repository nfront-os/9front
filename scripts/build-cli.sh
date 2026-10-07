#!/bin/bash
# Build everything the minimal pc64 CLI system needs, in dependency
# order (see MEMORY.md Teil 17):
#
#   1. the Go build tool (sys/src/cmd/build), into $CLI_DIR
#   2. userspace libraries: libc libbio libauth libthread lib9p
#   3. programs: init, rc, aux/kbdfs, boot/netmount
#   4. the boot payloads compiled into the kernel
#      (scripts/gen-testinit-i.sh, scripts/gen-exectest-payloads.sh)
#   5. the pc64 kernel (amd64/bin/9pc64)
#
# Kernel libraries (klib*.a) are NOT rebuilt; after changing them run
# their klib.json manifests first.
#
# usage: scripts/build-cli.sh
#   env:  CLI_DIR   work dir for the build tool and logs
#                   (default ${XDG_CACHE_HOME:-~/.cache}/9front-cli)
#         MKSYS_DIR dir holding Harvey's mksys (syscall stub generator,
#                   default ../harvey/util/src/harvey/cmd/mksys)
#         CPUS      parallel compiles (default: nproc)
#   needs: go, gcc, binutils, mksys, plan9port (for kbdfs' latin1.h)
set -e

ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
MKSYS_DIR=${MKSYS_DIR:-$ROOT/../harvey/util/src/harvey/cmd/mksys}
CPUS=${CPUS:-$(nproc)}

die(){ echo "build-cli: $*" >&2; exit 1; }

[ -x "$MKSYS_DIR/mksys" ] || die "mksys not found in $MKSYS_DIR (set MKSYS_DIR)"
export PATH=$MKSYS_DIR:$PATH
mkdir -p "$CLI_DIR/log"

echo "build tool"
(cd "$ROOT/sys/src/cmd/build" && GO111MODULE=off go build -o "$CLI_DIR/build.exe" .)

build(){
	local log="$CLI_DIR/log/$(echo "$1" | tr / _).log"
	echo "$1"
	env PLAN9="$ROOT" ARCH=amd64 CPUS="$CPUS" CC=gcc TOOLPREFIX= TOOLPFX= \
		"$CLI_DIR/build.exe" "$1" > "$log" 2>&1 \
		|| { tail -20 "$log"; die "$1 failed (log: $log)"; }
}

cd "$ROOT"
mkdir -p amd64/bin/aux

build sys/src/libc/libc.json
# The 9front kernel zeroes %rbx/%r12/%r13 on syscall return (forkret);
# gcc code needs the stubs to preserve them. That fix lives in mksys'
# template, outside this repo -- refuse to go on with an unpatched one.
grep -q '%rbx,56(%rsp)' sys/src/libc/9syscall/pread.s \
	|| die "mksys generates syscall stubs that don't save %rbx/%r12/%r13 (see MEMORY.md Teil 17)"

for m in \
	sys/src/libbio/libbio.json \
	sys/src/libauth/libauth.json \
	sys/src/libthread/libthread.json \
	sys/src/lib9p/lib9p.json \
	sys/src/cmd/init/build.json \
	sys/src/cmd/rc/build.json \
	sys/src/cmd/aux/kbdfs/build.json \
	scripts/boot/netmount.json
do
	build "$m"
done

echo "boot payloads"
bash scripts/gen-testinit-i.sh > "$CLI_DIR/log/gen-testinit-i.log" 2>&1
bash scripts/gen-exectest-payloads.sh > "$CLI_DIR/log/gen-exectest-payloads.log" 2>&1

build sys/src/9/pc64/build.json
echo "ok: amd64/bin/9pc64"
