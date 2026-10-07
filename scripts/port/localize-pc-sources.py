#!/usr/bin/env python3
"""
Make sys/src/9/pc64/build.json compile its "../pc/*.c" sources as local
copies instead of in place.

Why: those files do `#include "dat.h"` (a quoted include). C's quoted-include
search checks the *including file's own directory* before any -I path, so
when gcc compiles ../pc/apic.c for the pc64 build, "dat.h" resolves to
sys/src/9/pc/dat.h (the 386 one -- no RMACH/RUSER register-pinning, and
generally not guaranteed layout-compatible with pc64/dat.h's Mach/Proc)
instead of sys/src/9/pc64/dat.h. This silently miscompiles (or, for `m`/`up`,
fails to link) any such file that touches Mach/Proc state.

The classic mkfile build avoided this with 8c's `-.` flag (don't search the
source file's own directory for quoted includes, -I paths only) -- gcc has
no equivalent flag. The fix here matches what build.json's "Pre" step
already does for headers like usbehci.h: copy the file into pc64/ first, so
its own-directory quoted-include search finds pc64/dat.h.

This script:
  1. Copies each "../pc/X.c" entry in build.json's SourceFiles into
     sys/src/9/pc64/X.c.
  2. Adds a `cp ../pc/X.c X.c` entry to the "Pre" list for each, so future
     `./build.exe` runs keep the local copy in sync with sys/src/9/pc/X.c
     automatically (matching the existing header-copy entries).
  3. Rewrites the SourceFiles entries from "../pc/X.c" to "X.c".

Idempotent: safe to re-run (skips files already local/already in Pre).

Usage: python3 scripts/localize-pc-sources.py
"""
import json
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PC64 = ROOT / "sys/src/9/pc64"
BUILD_JSON = PC64 / "build.json"


def main():
    data = json.loads(BUILD_JSON.read_text())
    entry = data[0]
    sf = entry["SourceFiles"]
    pre = entry.setdefault("Pre", [])

    changed = []
    for i, f in enumerate(sf):
        if not (f.startswith("../pc/") and f.endswith(".c")):
            continue
        base = f[len("../pc/"):]
        cp_cmd = f"cp ../pc/{base} {base}"
        if cp_cmd not in pre:
            pre.append(cp_cmd)
        src = ROOT / "sys/src/9/pc" / base
        dst = PC64 / base
        shutil.copyfile(src, dst)
        sf[i] = base
        changed.append(base)

    BUILD_JSON.write_text(json.dumps(data, indent=4) + "\n")
    print(f"localized {len(changed)} files:")
    for b in changed:
        print(" ", b)


if __name__ == "__main__":
    main()
