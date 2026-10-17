#!/bin/bash
# The kernel's boot files (bootdir in sys/src/9/pc64/pc64, ...), as
# sys/src/9/boot/bootmkfile makes them:
#
#   boot              ../boot/boot.c, the first program initcode execs
#   $CONF.bootfs.paq  paqfs image of the boot protos:
#                       $CONF.bootfs.proto, bootfs.proto or ../boot/bootfs.proto,
#                       ../boot/disk.proto if the kernel has devsd/devfs/devusb,
#                       ../boot/X.proto for every X.$O in the kernel
#
# disk/mkfs and mkpaqfs run on Plan 9 only: scripts/guest/proto.py expands the
# protos, mkpaqfs is 9front's sys/src/cmd/paqfs/mkpaqfs.c built for the
# host with plan9port (../plan9port). Our binaries carry DWARF (8l
# writes none), so the image is stripped -- bootmkfile has the strip
# commented out.
#
# usage: scripts/build/gen-bootfs.sh [conf [dir]]	(default pc64; dir below
#	sys/src/9, default conf; needs the programs of the build)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
P9P=${PLAN9PORT:-$ROOT/../plan9port}
CONF=${1:-pc64}
KDIR=$ROOT/sys/src/9/${2:-$CONF}
# objtype as the kernel's mkfile sets it
export objtype=$(sed -n 's/^objtype=//p' "$KDIR/mkfile")
eval "$(python3 "$ROOT/scripts/gen/arch.py")"
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
case $objtype in
amd64)	ARCHFLAGS=-mcmodel=small LDEMUL= ;;
386)	ARCHFLAGS="-m32 -march=pentium -mno-sse" LDEMUL="-m elf_i386" ;;
arm64)	ARCHFLAGS="-fsigned-char -mno-outline-atomics" LDEMUL= ;;	# as arm64/cflags.json
riscv64)	ARCHFLAGS="-march=rv64gc -mabi=lp64d -mcmodel=medany -mno-relax -fsigned-char" LDEMUL=--no-relax ;;	# as riscv64/cflags.json
*)	ARCHFLAGS= LDEMUL= ;;
esac
CFLAGS="-std=gnu17 -fno-asm -ffreestanding -nostdinc -fno-builtin -fplan9-extensions
	-fno-omit-frame-pointer -fno-stack-protector -fno-pie $ARCHFLAGS -fcommon
	-I$ROOT/$objtype/include -I$ROOT/sys/include -I$ROOT/sys/src/9/boot"
${TOOLPREFIX}gcc $CFLAGS -c -o "$TMP/boot.o" "$ROOT/sys/src/9/boot/boot.c"
${TOOLPREFIX}ld $LDEMUL -o "$KDIR/boot" --entry=_main -z noseparate-code -z max-page-size=0x1000 \
	-T "$ROOT/$objtype/plan9.ld" "$TMP/boot.o" "$ROOT/$objtype/lib.gcc/libc.a" \
	$( [ -f "$ROOT/$objtype/lib.gcc/libgcc.a" ] && echo "$ROOT/$objtype/lib.gcc/libgcc.a")

# the kernel's objects ($OBJ): source files and configuration lines
OBJ=$(python3 - "$KDIR/build.json" $CONF <<'PY'
import json, os, sys
ms = json.load(open(sys.argv[1]))
m = ([x for x in ms if x['Name'] == sys.argv[2]] or ms)[0]
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

python3 "$ROOT/scripts/guest/proto.py" -s "$ROOT" -d "$TMP/$CONF.bootfs" -e tmp -e lib/firmware $PROTOS
find "$TMP/$CONF.bootfs" -type f -exec sh -c 'file -b "$1" | grep -q ^ELF && '"${TOOLPREFIX}"'strip "$1"' _ {} \;
# a new image only when what goes into it changes (names, modes, contents):
# mkpaqfs records the time and the files' times, so every image differs, and
# the kernel embedding it would always be out of date (build.exe -D)
sum=$(cd "$TMP/$CONF.bootfs" && find . | LC_ALL=C sort | while read -r f; do
	echo "$f $(stat -c %a "$f")"; [ -f "$f" ] && sha1sum < "$f"; done | sha1sum)
if [ -f "$KDIR/$CONF.bootfs.paq" ] && [ "$(cat "$KDIR/$CONF.bootfs.sum" 2>/dev/null)" = "$sum" ]; then
	echo "$CONF.bootfs.paq: unchanged"
else
	"$HOST/mkpaqfs" -9 -b 65536 -o "$KDIR/$CONF.bootfs.paq" "$TMP/$CONF.bootfs"
	echo "$sum" > "$KDIR/$CONF.bootfs.sum"
fi
ls -l "$KDIR/boot" "$KDIR/$CONF.bootfs.paq"
