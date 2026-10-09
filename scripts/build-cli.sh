#!/bin/bash
# Build everything the minimal pc64 CLI system needs, in dependency
# order (see MEMORY.md Teil 17):
#
#   1. the Go build tool (sys/src/cmd/build), into $CLI_DIR
#   2. manifests, regenerated from 9front's mkfiles: scripts/genlibc.py,
#      scripts/genlibs.py, scripts/gencmds.py
#   3. userspace libraries: libc, then those of genlibs.py, and the
#      kernel's (sys/src/lib*/klib.json: klib*.a)
#   4. programs: the manifests of gencmds.py (sys/src/mkfile's $SUBSYS
#      $CMDS: ape, /acme, cmd, games), with the files their install rules
#      put in place (scripts/mkinstall.py); scripts/check-install.py
#      checks that everything is there
#   5. what the kernel embeds: rebootcode.i, apbootstrap.i, etherm10g*.i,
#      sd53c8xx.i (scripts/gen-*-i.sh; aux/na and cpp as host tools), initcode (scripts/gen-initcode-i.sh),
#      /boot/boot and $CONF.bootfs.paq (scripts/gen-bootfs.sh)
#   6. the pc64 kernel: amd64/bin/9pc64 (ELF, multiboot: scripts/boot-cli.sh)
#      and amd64/9pc64, where 9front installs it, as the Plan 9 a.out that
#      9boot loads (scripts/elf2aout.py)
#   7. 9front's boot loaders, built in the system just made with its own
#      mkfiles and compilers (scripts/build-boot.sh; boots it once)
#
# usage: [objtype=386|amd64|arm64] scripts/build-cli.sh
#   objtype selects the architecture (default amd64, see scripts/arch.py);
#   its manifests are the cmds.<objtype>.json etc. besides amd64's
#   env:  CLI_DIR   work dir for the build tool and logs
#                   (default ${XDG_CACHE_HOME:-~/.cache}/9front-cli)
#         CPUS      parallel compiles (default: nproc)
#   needs: go, gcc, binutils, bison, bwrap, plan9port (host tools: yacc,
#          mpc, mkrunetype, mklatin, maketab, rc, iconv)
set -e

ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
CPUS=${CPUS:-$(nproc)}
eval "$(python3 "$ROOT/scripts/arch.py")" || exit 1
export objtype

die(){ echo "build-cli: $*" >&2; exit 1; }

mkdir -p "$CLI_DIR/log"

echo "build tool"
(cd "$ROOT/sys/src/cmd/build" && GO111MODULE=off go build -o "$CLI_DIR/build.exe" .)

build(){	# build manifest [entry regexp...]
	local log="$CLI_DIR/log/$(echo "$1" | tr / _).log"
	echo "$1"
	env PLAN9="$ROOT" ARCH=$objtype CPUS="$CPUS" CC=gcc TOOLPREFIX=$TOOLPREFIX TOOLPFX= \
		"$CLI_DIR/build.exe" "$@" > "$log" 2>&1 \
		|| { tail -20 "$log"; die "$1 failed (log: $log)"; }
}

cd "$ROOT"
mkdir -p $objtype/bin/aux $objtype/lib.gcc
# gcc's runtime (64-bit division on 386, ...: 8c had vlrt.c), linked
# after the libraries (/$objtype/cflags.json)
case $objtype in
386)	cp "$(gcc -m32 -print-libgcc-file-name)" 386/lib.gcc/libgcc.a ;;
arm64)	cp "$(${TOOLPREFIX}gcc -print-libgcc-file-name)" arm64/lib.gcc/libgcc.a ;;
esac

echo "manifests"
python3 scripts/genlibc.py > /dev/null
python3 scripts/genlibs.py > /dev/null
python3 scripts/gencmds.py > /dev/null

build sys/src/libc/libc$SUFFIX.json
for l in $(python3 scripts/genlibs.py --list); do
	build "sys/src/$l/$l$SUFFIX.json"
done
# the kernel's libraries (klib*.a: the same sources with the kernel's
# flags; root build.json lists them)
for k in sys/src/lib*/klib.json; do
	build "$k"
done
for m in $(python3 scripts/gencmds.py --list)
do
	build "$m"
done

gen(){	# gen <script> [args]: a generator of scripts/, logged
	local g=$1; shift
	bash scripts/$g "$@" > "$CLI_DIR/log/$g.log" 2>&1 \
		|| { tail -20 "$CLI_DIR/log/$g.log"; die "$g failed"; }
}

echo "initcode, boot, bootfs.paq"
case $objtype in
amd64)
	# what pc64/mkfile makes before the kernel objects: rebootcode.i,
	# apbootstrap.i, ../pc/etherm10g%.i, ../pc/sd53c8xx.i (main.$O, mp.$O,
	# etherm10g.$O, sd53c8xx.$O)
	for g in gen-rebootcode-i.sh gen-apbootstrap-i.sh gen-etherm10g-i.sh gen-sd53c8xx-i.sh; do
		gen $g
	done
	gen gen-initcode-i.sh
	gen gen-bootfs.sh pc64

	# bootdir paths: /$objtype/bin/paqfs, $CONF.bootfs.paq (as mk sets them)
	CONF=pc64 build sys/src/9/pc64/build.json
	python3 scripts/elf2aout.py amd64/bin/9pc64 amd64/9pc64 > /dev/null || die "elf2aout failed"
	echo "ok: amd64/bin/9pc64, amd64/9pc64"
	;;
386)
	# pc/mkfile: rebootcode.i, apbootstrap.i, sd53c8xx.i; the manifest
	# from the mkfile and CONF (scripts/genkernel.py)
	for g in gen-rebootcode-pc-i.sh "gen-apbootstrap-i.sh pc" gen-sd53c8xx-i.sh; do
		gen $g
	done
	gen gen-initcode-i.sh
	python3 scripts/genkernel.py pc > /dev/null || die "genkernel.py pc failed"
	gen gen-bootfs.sh pc

	CONF=pc build sys/src/9/pc/build.json '^pc$'
	python3 scripts/elf2aout.py 386/bin/9pc 386/9pc > /dev/null || die "elf2aout failed"
	echo "ok: 386/bin/9pc, 386/9pc"
	;;
*)
	die "no kernel for objtype=$objtype yet"
	;;
esac

# everything mk install would install (stale files of older builds,
# at places no rule installs to, are removed)
python3 scripts/check-install.py -r > "$CLI_DIR/log/check-install.log" 2>&1 \
	|| { grep -v '^removed' "$CLI_DIR/log/check-install.log"; die "check-install failed"; }
tail -1 "$CLI_DIR/log/check-install.log"

# 9front's boot loaders, built by the amd64 system's own compilers
if [ $objtype = amd64 ]; then
	echo "boot loaders"
	bash scripts/build-boot.sh > "$CLI_DIR/log/build-boot.log" 2>&1 \
		|| { tail -20 "$CLI_DIR/log/build-boot.log"; die "build-boot.sh failed"; }
	tail -1 "$CLI_DIR/log/build-boot.log"
fi
