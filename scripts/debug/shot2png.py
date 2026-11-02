#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
# The picture tests/gpu/screenshot prints in a guest ("shot w h", then a
# line "s rrggbb..." per row), out of a console log, as a PNG.
#   usage: shot2png.py console.log out.png [n]   (n: the n-th shot, from 0)
import struct, sys, zlib

lines = open(sys.argv[1], errors="replace").read().split("\n")
n = int(sys.argv[3]) if len(sys.argv) > 3 else 0
starts = [i for i, l in enumerate(lines) if l.startswith("shot ")]
if n >= len(starts):
    sys.exit("shot2png: no shot %d in %s" % (n, sys.argv[1]))
i = starts[n]
w, h = map(int, lines[i].split()[1:3])
rows = [bytes.fromhex(l[2:].strip()[:w * 6]) for l in lines[i + 1:i + 1 + h] if l.startswith("s ")]
raw = b"".join(b"\0" + r.ljust(w * 3, b"\0") for r in rows)

def chunk(t, d):
    return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d) & 0xffffffff)

open(sys.argv[2], "wb").write(b"\x89PNG\r\n\x1a\n" +
    chunk(b"IHDR", struct.pack(">IIBBBBB", w, len(rows), 8, 2, 0, 0, 0)) +
    chunk(b"IDAT", zlib.compress(raw)) + chunk(b"IEND", b""))
print("%s: %dx%d" % (sys.argv[2], w, len(rows)))
