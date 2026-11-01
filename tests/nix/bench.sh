#!/bin/bash
# NIX benchmark (not a test): tests/nix/nixbench.c on a time-sharing core
# (TC) and on an application core (AC), on an idle system and with a busy
# loop on each TC. amd64 under KVM (boot-cli.sh needs /dev/kvm).
#	env: NCPU (4; the last one is the AC), REPS (5), MEM (1024)
# Prints the guest's lines: name core mean min max unit (core 0: a TC).
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
NCPU=${NCPU:-4} REPS=${REPS:-5}
ac=$((NCPU-1))
(cd "$HERE" && env PLAN9="$ROOT" ARCH=$objtype CC=gcc "$BUILDEXE" build.json) > "$TMPD/build" 2>&1 \
	|| { cat "$TMPD/build"; exit 1; }
load=""
for i in $(seq 1 $ac); do load="$load rc -c 'while(){}' & l$i=\$apid;"; done
kill=""
for i in $(seq 1 $ac); do kill="$kill echo kill > /proc/\$l$i/note;"; done
SMP=$NCPU MEM=${MEM:-1024} CMDWAIT=900 KARGS="${KARGS:+$KARGS }*ncpu=$NCPU *nixac=1" \
	"$ROOT/scripts/guest/boot-cli.sh" \
	"echo '##' idle; /tests/nixbench -r $REPS; execac -c $ac /tests/nixbench -r $REPS" \
	"echo '##' 2 MB pages; /tests/nixbench -b -r $REPS | grep memwalk; execac -c $ac /tests/nixbench -b -r $REPS | grep memwalk" \
	"$load echo '##' load: $ac busy loops on the $ac TCs; /tests/nixbench -r $REPS; execac -c $ac /tests/nixbench -r $REPS; $kill" \
	> "$TMPD/console" 2>&1
tr -d '\r' < "$TMPD/console" | grep -E '^(## |compute|memwalk|gaps|maxgap|syscall)'
