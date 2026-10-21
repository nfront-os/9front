#!/bin/bash
# A gcc-built amd64 guest in which 9front's own tools build riscv64 with
# kencc (jc/ja/jl, mk), on a copy of the tree's sources ($CLI_DIR/src:
# git ls-files, no gcc objects -- the gcc build leaves files under mk's
# names in the source directories, cc.aj, usb.aj, ..., and the kernel's
# virt.c with the root files as C arrays, which mk would take as up to
# date). The copy is served as src9 and bound over the guest's
# /sys/src, /sys/include and /riscv64, so mk installs land in the copy
# (riscv64/lib, riscv64/bin, the kernel in sys/src/9/riscv64).
# Host programs that mkfiles build on the way (libc/ucd's mkrunetype)
# need kencc amd64 libraries, as 9front's /amd64/lib has them: the setup
# builds them (mk libs, 6c) into .guest/amd64/lib of the copy, bound on
# /amd64/lib.
#
# usage: scripts/guest/kencc-riscv64.sh [cmd ...]
#   no cmd: the guest stays up (HOLD=1); type lines with
#           CLI_DIR=$CLI_DIR scripts/guest/guest-cmd.sh 'cmd'
#   cmd:    typed at the prompt after the setup, then the guest exits
#   env:  CLI_DIR  work dir (default ~/.cache/9front-riscv-cli)
#         KEEP=1   reuse the copy of an earlier run (mk goes on from there)
#         SMP MEM  guest CPUs (4) and memory (2048)
#   needs the amd64 build first: CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
export CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-riscv-cli}
mkdir -p "$CLI_DIR"

S=$CLI_DIR/src
if [ "$KEEP" != 1 ] || [ ! -d "$S" ]; then
	rm -rf "$S"
	mkdir -p "$S"
	git -C "$ROOT" ls-files -z sys/src sys/include sys/lib acme rc/bin '*/include/*' '*/mkfile' \
		| (cd "$ROOT" && tar -cf - --null -T -) | tar -xf - -C "$S"
fi
mkdir -p "$S/.guest/amd64/lib/ape" "$S/.glog"
# the install directories, as 9front's root has them
grep -E '^mkdir -p (acme/bin/)?riscv64(/|$)' "$ROOT/sys/lib/rootstub" | (cd "$S" && sh)

T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
# mount points the guest's root lacks
mkdir -p "$T/extra/riscv64" "$T/extra/amd64/lib" "$T/extra/acme" "$T/extra/lib/firmware"
# every architecture's include directory (libmach includes every ureg.h)
archs=$(git -C "$ROOT" ls-files '*/include/*' | cut -d/ -f1 | sort -u | grep -v '^sys$')
for a in $archs; do mkdir -p "$T/extra/$a/include"; done

setup='srv tcp!10.0.2.101!5641 src9; mount -c /srv/src9 /n/src9;'
setup="$setup bind /n/src9/sys/src /sys/src; bind /n/src9/sys/include /sys/include;"
setup="$setup bind -c /n/src9/riscv64 /riscv64; bind -c /n/src9/.guest/amd64/lib /amd64/lib;"
setup="$setup bind -a /n/src9/amd64 /amd64; bind -c /n/src9/acme /acme;"
for a in $archs; do setup="$setup bind -a /n/src9/$a/include /$a/include;"; done
setup="$setup test -e /amd64/lib/.libs || @{cd /sys/src && objtype=amd64 mk libs >/n/src9/.glog/amd64-libs.log >[2=1] && touch /amd64/lib/.libs};"

if [ $# -eq 0 ]; then
	HOLD=1
	export HOLD
fi
SRC9=$S UFS_EXTRA=$T/extra MEM=${MEM:-2048} QEMU_EXTRA="-smp ${SMP:-4} $QEMU_EXTRA" \
	PLAN9=$ROOT objtype=amd64 bash "$ROOT/scripts/guest/boot-cli.sh" "$setup" "$@"
