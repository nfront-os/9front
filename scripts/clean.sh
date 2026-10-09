#!/bin/bash
# Remove the build's artefacts from the tree, as `mk clean' in sys/src
# does: for every manifest scripts/build-cli.sh builds -- libc, the
# libraries, the programs (scripts/gencmds.py --list), the kernel
# libraries (klib.json) and the pc64 kernel -- the build tool's -clean
# removes the objects, links not yet installed, program-local archives,
# generated sources (the manifests' Clean: y.tab.[ch], enam.c, ...) and
# the kernel's copied and generated files. The manifests stay (they are
# checked in).
#
# -i also removes what the build installs (like a fresh checkout): every
# file scripts/check-install.py expects (programs, scripts, gcc archives
# in /$objtype/lib.gcc, amd64/9pc64), the ELF kernel amd64/bin/9pc64,
# amd64/init and the boot loaders of 9bootproto (scripts/build-boot.sh).
# Files the repository tracks are never removed.
#
# usage: scripts/clean.sh [-i]
#   env:  CLI_DIR   where build-cli.sh put the build tool
#                   (default ${XDG_CACHE_HOME:-~/.cache}/9front-cli)
set -e
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
die(){ echo "clean: $*" >&2; exit 1; }
installed=
case $* in
'')	;;
-i)	installed=1 ;;
*)	die "usage: scripts/clean.sh [-i]" ;;
esac

cd "$ROOT"
if [ ! -x "$CLI_DIR/build.exe" ] || [ sys/src/cmd/build/build.go -nt "$CLI_DIR/build.exe" ]; then
	mkdir -p "$CLI_DIR"
	(cd sys/src/cmd/build && GO111MODULE=off go build -o "$CLI_DIR/build.exe" .)
fi
mkdir -p "$CLI_DIR/log"
log=$CLI_DIR/log/clean.log
: > "$log"

clean(){
	env PLAN9="$ROOT" ARCH=amd64 CC=gcc TOOLPREFIX= TOOLPFX= \
		"$CLI_DIR/build.exe" -clean "$1" >> "$log" 2>&1 \
		|| { tail -5 "$log"; die "$1 failed (log: $log)"; }
}

clean sys/src/libc/libc.json
for l in $(python3 scripts/genlibs.py --list); do
	clean "sys/src/$l/$l.json"
done
for m in $(python3 scripts/gencmds.py --list); do
	clean "$m"
done
for k in sys/src/lib*/klib.json; do
	clean "$k"
done
objtype=amd64 CONF=pc64 clean sys/src/9/pc64/build.json
echo "$(grep -c '^rm ' "$log") files removed from the source directories"

if [ -n "$installed" ]; then
	n=0
	for f in $(python3 scripts/check-install.py -l) amd64/bin/9pc64 amd64/init \
		$(awk '/^386/ { s = 1; next } /^[^\t]/ { s = 0 } s && NF == 2 { print "386/" $1 }' \
			sys/lib/sysconfig/proto/9bootproto) \
		arm64/bootaa64.efi; do
		f=${f#/}
		[ -f "$f" ] || continue
		git ls-files --error-unmatch "$f" > /dev/null 2>&1 && continue
		rm -f "$f"
		n=$((n + 1))
	done
	# directories left empty below the install roots
	for d in amd64/bin amd64/lib.gcc acme/bin/amd64 rc/bin; do
		[ -d "$d" ] && find "$d" -mindepth 1 -type d -empty -delete
	done
	echo "$n installed files removed"
fi
