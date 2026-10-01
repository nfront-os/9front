#!/bin/sh
# Regenerate sys/src/9/pc/etherm10g{2k,4k}.i from the matching .fw firmware
# blobs (etherm10g.c does `#include "../pc/etherm10gNk.i"` and expects each
# to define `static char fwNk[] = {...}`).
#
# The original mkfile rule shells out to Plan 9's `xd -1x` + sed to turn the
# raw firmware bytes into a C array; this is the Python equivalent (xd isn't
# available on a non-Plan9 host). Not tracked in git, like the other
# generated .i files -- regenerate on demand.
set -e

ROOT=${PLAN9:-$(git rev-parse --show-toplevel)}
PC=$ROOT/sys/src/9/pc

for stem in 2k 4k; do
	python3 - "$PC/etherm10g$stem.fw" "$PC/etherm10g$stem.i" "fw$stem" <<'PYEOF'
import sys
binpath, outpath, name = sys.argv[1], sys.argv[2], sys.argv[3]
data = open(binpath, 'rb').read()
lines = ["static char %s[] = {" % name]
for i in range(0, len(data), 12):
	lines.append("\t" + ", ".join("0x%02x" % b for b in data[i:i+12]) + ",")
lines.append("};")
open(outpath, "w").write("\n".join(lines) + "\n")
print("wrote %d bytes to %s" % (len(data), outpath))
PYEOF
done
