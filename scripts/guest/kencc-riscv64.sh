#!/bin/bash
# A gcc-built amd64 guest in which 9front's own tools build riscv64 with
# kencc (jc/ja/jl, mk): the tree is served as src9 and bound over the
# guest's /sys/src, /sys/include and /riscv64, so mk installs land in
# the tree (riscv64/lib, riscv64/bin, the kernel in sys/src/9/riscv64).
# Host programs that mkfiles build on the way (libc/ucd's mkrunetype)
# need kencc amd64 libraries: they go to .guest/amd64/lib in the tree,
# bound on /amd64/lib.
#
# usage: scripts/guest/kencc-riscv64.sh [cmd ...]
#   no cmd: the guest stays up (HOLD=1); type lines with
#           CLI_DIR=$CLI_DIR scripts/guest/guest-cmd.sh 'cmd'
#   cmd:    typed at the prompt after the setup, then the guest exits
#   env:  CLI_DIR  work dir (default ~/.cache/9front-riscv-cli)
#         SMP MEM  guest CPUs (4) and memory (2048)
#   needs the amd64 build first: CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
export CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-riscv-cli}
mkdir -p "$CLI_DIR" "$ROOT/.guest/amd64/lib/ape" "$ROOT/riscv64/lib/ape" "$ROOT/riscv64/bin"
grep -qx '/.guest/' "$(git -C "$ROOT" rev-parse --git-path info/exclude)" 2>/dev/null \
	|| echo '/.guest/' >> "$(git -C "$ROOT" rev-parse --git-path info/exclude)"

T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
# mount points the guest's root lacks
mkdir -p "$T/extra/riscv64" "$T/extra/amd64/lib"
# every architecture's include directory (libmach includes every ureg.h)
archs=$(git -C "$ROOT" ls-files '*/include/*' | cut -d/ -f1 | sort -u | grep -v '^sys$')
for a in $archs; do mkdir -p "$T/extra/$a/include"; done

setup='srv tcp!10.0.2.101!5641 src9; mount -c /srv/src9 /n/src9;'
setup="$setup bind /n/src9/sys/src /sys/src; bind /n/src9/sys/include /sys/include;"
setup="$setup bind -c /n/src9/riscv64 /riscv64; bind -c /n/src9/.guest/amd64/lib /amd64/lib;"
setup="$setup bind -a /n/src9/amd64 /amd64;"
for a in $archs; do setup="$setup bind -a /n/src9/$a/include /$a/include;"; done

if [ $# -eq 0 ]; then
	HOLD=1
	export HOLD
fi
SRC9=$ROOT UFS_EXTRA=$T/extra MEM=${MEM:-2048} QEMU_EXTRA="-smp ${SMP:-4} $QEMU_EXTRA" \
	PLAN9=$ROOT objtype=amd64 bash "$ROOT/scripts/guest/boot-cli.sh" "$setup" "$@"
