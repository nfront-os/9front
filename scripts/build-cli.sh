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
#   5. what the kernel embeds: rebootcode.i, apbootstrap.i, etherm10g*.i
#      (scripts/gen-*-i.sh), initcode (scripts/gen-initcode-i.sh),
#      /boot/boot and $CONF.bootfs.paq (scripts/gen-bootfs.sh)
#   6. the pc64 kernel: amd64/bin/9pc64 (ELF, multiboot: scripts/boot-cli.sh)
#      and amd64/9pc64, where 9front installs it, as the Plan 9 a.out that
#      9boot loads (scripts/elf2aout.py)
#   7. 9front's boot loaders, built in the system just made with its own
#      mkfiles and compilers (scripts/build-boot.sh; boots it once)
#
# usage: scripts/build-cli.sh
#   env:  CLI_DIR   work dir for the build tool and logs
#                   (default ${XDG_CACHE_HOME:-~/.cache}/9front-cli)
#         CPUS      parallel compiles (default: nproc)
#   needs: go, gcc, binutils, bison, bwrap, plan9port (host tools: yacc,
#          mpc, mkrunetype, mklatin, maketab, rc, iconv)
set -e

ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
CPUS=${CPUS:-$(nproc)}

die(){ echo "build-cli: $*" >&2; exit 1; }

mkdir -p "$CLI_DIR/log"

echo "build tool"
(cd "$ROOT/sys/src/cmd/build" && GO111MODULE=off go build -o "$CLI_DIR/build.exe" .)

build(){
	local log="$CLI_DIR/log/$(echo "$1" | tr / _).log"
	echo "$1"
	env PLAN9="$ROOT" ARCH=amd64 CPUS="$CPUS" CC=gcc TOOLPREFIX= TOOLPFX= \
		"$CLI_DIR/build.exe" "$1" > "$log" 2>&1 \
		|| { tail -20 "$log"; die "$1 failed (log: $log)"; }
}

cd "$ROOT"
mkdir -p amd64/bin/aux

echo "manifests"
python3 scripts/genlibc.py > /dev/null
python3 scripts/genlibs.py > /dev/null
python3 scripts/gencmds.py > /dev/null

build sys/src/libc/libc.json
for l in $(python3 scripts/genlibs.py --list); do
	build "sys/src/$l/$l.json"
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

echo "initcode, boot, bootfs.paq"
# what pc64/mkfile makes before the kernel objects: rebootcode.i,
# apbootstrap.i, ../pc/etherm10g%.i (main.$O, mp.$O, etherm10g.$O)
for g in gen-rebootcode-i.sh gen-apbootstrap-i.sh gen-etherm10g-i.sh; do
	sh scripts/$g > "$CLI_DIR/log/$g.log" 2>&1 || { cat "$CLI_DIR/log/$g.log"; die "$g failed"; }
done
bash scripts/gen-initcode-i.sh > "$CLI_DIR/log/gen-initcode-i.log" 2>&1 \
	|| { cat "$CLI_DIR/log/gen-initcode-i.log"; die "gen-initcode-i.sh failed"; }
bash scripts/gen-bootfs.sh pc64 > "$CLI_DIR/log/gen-bootfs.log" 2>&1 \
	|| { tail "$CLI_DIR/log/gen-bootfs.log"; die "gen-bootfs.sh failed"; }

# bootdir paths: /$objtype/bin/paqfs, $CONF.bootfs.paq (as mk sets them)
objtype=amd64 CONF=pc64 build sys/src/9/pc64/build.json
python3 scripts/elf2aout.py amd64/bin/9pc64 amd64/9pc64 > /dev/null || die "elf2aout failed"
echo "ok: amd64/bin/9pc64, amd64/9pc64"

# everything mk install would install (stale files of older builds,
# at places no rule installs to, are removed)
python3 scripts/check-install.py -r > "$CLI_DIR/log/check-install.log" 2>&1 \
	|| { grep -v '^removed' "$CLI_DIR/log/check-install.log"; die "check-install failed"; }
tail -1 "$CLI_DIR/log/check-install.log"

echo "boot loaders"
bash scripts/build-boot.sh > "$CLI_DIR/log/build-boot.log" 2>&1 \
	|| { tail -20 "$CLI_DIR/log/build-boot.log"; die "build-boot.sh failed"; }
tail -1 "$CLI_DIR/log/build-boot.log"
