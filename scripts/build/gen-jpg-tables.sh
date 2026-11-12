#!/bin/bash
# Generate sys/src/cmd/jpg/rgbv.h and ycbcr.h the way jpg's mkfile does:
# build the host tools rgbrgbv and rgbycc and run them. They run on the
# build host, so they are built with plan9port's 9c/9l.
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# PLAN9PORT (the distribution's paths.env)
P9P=$PLAN9PORT
JPG=$ROOT/sys/src/cmd/jpg
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
cd "$TMP"
for t in rgbrgbv:rgbv.h rgbycc:ycbcr.h; do
	tool=${t%%:*}; out=${t#*:}
	PLAN9=$P9P "$P9P/bin/9c" -o $tool.o "$JPG/$tool.c"
	PLAN9=$P9P "$P9P/bin/9l" -o $tool $tool.o
	./$tool > "$JPG/$out"
	echo "wrote sys/src/cmd/jpg/$out"
done
