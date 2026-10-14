#!/bin/bash
# A kernel header that only includes another directory's, as a kernel
# mkfile's recipe makes it (bcm64: screen.h: ../bcm/screen.h, echo
# '#include "../bcm/screen.h"' >screen.h). Pre step of manifests from
# scripts/gen/genkernel.py, which run without a shell.
#
# usage: scripts/build/kheader.sh target.h ../dir/source.h
echo '#include "'"$2"'"' > "$1"
