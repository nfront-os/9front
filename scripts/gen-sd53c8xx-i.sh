#!/bin/bash
# sys/src/9/pc/sd53c8xx.i, the NCR 53c8xx SCSI microcode, as
# sys/src/9/pc/mkfile makes it (pc64/mkfile: cd ../pc && mk sd53c8xx.i):
#
#	sd53c8xx.i:	sd53c8xx.n
#		aux/na $prereq > $target
#
# aux/na (sys/src/cmd/aux/na/na.y) is the script assembler; it runs
# /bin/cpp -+ -N on its input. Both run on the build host: 9front's na
# and cpp built with plan9port (../plan9port; cpp needs two shims: glibc
# declares a getopt of its own, include.c assigns nil to a uchar), and
# na runs in a bwrap namespace where /bin/cpp is 9front's cpp.
#
# usage: scripts/gen-sd53c8xx-i.sh
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
P9P=${PLAN9PORT:-$ROOT/../plan9port}
HOST=$CLI_DIR/host
NA=$ROOT/sys/src/cmd/aux/na
CPP=$ROOT/sys/src/cmd/cpp
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
p9(){ PLAN9=$P9P "$@"; }

mkdir -p "$HOST"
if [ ! -x "$HOST/na" ] || [ "$NA/na.y" -nt "$HOST/na" ]; then
	(cd "$TMP" && cp "$NA/na.y" "$NA/na.h" . &&
	 p9 "$P9P/bin/yacc" -d na.y && p9 "$P9P/bin/9c" y.tab.c &&
	 p9 "$P9P/bin/9l" -o "$HOST/na" y.tab.o) > "$TMP/na.log" 2>&1 \
		|| { cat "$TMP/na.log"; exit 1; }
fi
if [ ! -x "$HOST/cpp" ] || [ -n "$(find "$CPP" -name '*.[ch]' -newer "$HOST/cpp")" ]; then
	printf '#include <unistd.h>\n#define getopt cpp_getopt\n' > "$TMP/shim.h"
	(cd "$TMP" &&
	 for f in cpp lex nlist tokens macro eval include hideset; do
		p9 "$P9P/bin/9c" -include "$TMP/shim.h" -Wno-error=int-conversion \
			-I"$CPP" -o $f.o "$CPP/$f.c" || exit 1
	 done &&
	 p9 "$P9P/bin/9l" -o "$HOST/cpp" cpp.o lex.o nlist.o tokens.o macro.o eval.o include.o hideset.o) \
		> "$TMP/cpp.log" 2>&1 || { cat "$TMP/cpp.log"; exit 1; }
fi

cd "$ROOT/sys/src/9/pc"
bincpp=(--ro-bind "$HOST/cpp" /usr/bin/cpp)	# /bin is /usr/bin, or:
[ -L /bin ] || bincpp+=(--ro-bind "$HOST/cpp" /bin/cpp)
objtype=amd64 bwrap --ro-bind / / --tmpfs /tmp --dev /dev \
	"${bincpp[@]}" --ro-bind "$HOST/na" /tmp/na \
	--bind "$PWD" "$PWD" --chdir "$PWD" \
	/tmp/na sd53c8xx.n > "$TMP/sd53c8xx.i"
mv "$TMP/sd53c8xx.i" sd53c8xx.i
grep -E '^#define NA_(SCRIPT_SIZE|PATCHES)' sd53c8xx.i
