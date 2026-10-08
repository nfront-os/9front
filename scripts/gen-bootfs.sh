#!/bin/bash
# The kernel's boot files (bootdir in sys/src/9/pc64/pc64), as
# sys/src/9/boot/bootmkfile makes them:
#
#   boot              ../boot/boot.c, the first program initcode execs
#   $CONF.bootfs.paq  paqfs image of the boot protos:
#                       $CONF.bootfs.proto, bootfs.proto or ../boot/bootfs.proto,
#                       ../boot/disk.proto if the kernel has devsd/devfs/devusb,
#                       ../boot/X.proto for every X.$O in the kernel
#
# disk/mkfs and mkpaqfs run on Plan 9 only: scripts/proto.py expands the
# protos, mkpaqfs is 9front's sys/src/cmd/paqfs/mkpaqfs.c built for the
# host with plan9port (../plan9port). Our binaries carry DWARF (8l
# writes none), so the image is stripped -- bootmkfile has the strip
# commented out.
#
# usage: scripts/gen-bootfs.sh [conf]	(default pc64; needs scripts/build-cli.sh's programs)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
P9P=${PLAN9PORT:-$ROOT/../plan9port}
CONF=${1:-pc64}
KDIR=$ROOT/sys/src/9/pc64
export objtype=amd64
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

# host mkpaqfs
HOST=$CLI_DIR/host
if [ ! -x "$HOST/mkpaqfs" ] || [ "$ROOT/sys/src/cmd/paqfs/mkpaqfs.c" -nt "$HOST/mkpaqfs" ]; then
	mkdir -p "$HOST"
	(cd "$TMP" &&
	 PLAN9=$P9P "$P9P/bin/9c" -I"$ROOT/sys/src/cmd/paqfs" "$ROOT/sys/src/cmd/paqfs/mkpaqfs.c" &&
	 PLAN9=$P9P "$P9P/bin/9l" -o "$HOST/mkpaqfs" mkpaqfs.o) > "$TMP/mkpaqfs.log" 2>&1 \
		|| { cat "$TMP/mkpaqfs.log"; exit 1; }
fi

# boot: boot.$O: ../boot/boot.c	$CC -I../boot $CFLAGS ../boot/boot.c
CFLAGS="-std=gnu17 -fno-asm -ffreestanding -nostdinc -fno-builtin -fplan9-extensions
	-fno-omit-frame-pointer -fno-stack-protector -fno-pie -mcmodel=small -fcommon
	-I$ROOT/amd64/include -I$ROOT/sys/include -I$ROOT/sys/src/9/boot"
gcc $CFLAGS -c -o "$TMP/boot.o" "$ROOT/sys/src/9/boot/boot.c"
ld -o "$KDIR/boot" --entry=_main -z noseparate-code -z max-page-size=0x1000 \
	-T "$ROOT/amd64/plan9.ld" "$TMP/boot.o" "$ROOT/amd64/lib/libc.a"

# the kernel's objects ($OBJ): source files and configuration lines
OBJ=$(python3 - "$KDIR/build.json" <<'PY'
import json, os, sys
m = json.load(open(sys.argv[1]))[0]
objs = set(os.path.splitext(os.path.basename(f))[0] for f in m.get('SourceFiles', []))
c = m['Kernel']['Config']
objs |= set('dev' + l.split()[0] for l in c.get('Dev', []))
for k in ('Link', 'Misc', 'Ip'):
	objs |= set(l.split()[0] for l in c.get(k, []))
print(' '.join(sorted(objs)))
PY
)
has(){ case " $OBJ " in *" $1 "*) return 0 ;; esac; return 1; }
cd "$KDIR"
PROTOS=
for i in $CONF.bootfs.proto bootfs.proto ../boot/bootfs.proto; do
	[ -r $i ] && { PROTOS=$i; break; }
done
if has devfs || has devsd || has devusb; then PROTOS="$PROTOS ../boot/disk.proto"; fi
for j in ../boot/*.proto; do
	has "$(basename $j .proto)" && PROTOS="$PROTOS $j"
done
echo "protos: $PROTOS"

python3 "$ROOT/scripts/proto.py" -s "$ROOT" -d "$TMP/$CONF.bootfs" -e tmp -e lib/firmware $PROTOS
find "$TMP/$CONF.bootfs" -type f -exec sh -c 'file -b "$1" | grep -q ^ELF && strip "$1"' _ {} \;
"$HOST/mkpaqfs" -9 -b 65536 -o "$KDIR/$CONF.bootfs.paq" "$TMP/$CONF.bootfs"
ls -l "$KDIR/boot" "$KDIR/$CONF.bootfs.paq"
