#!/bin/bash
# Struct layouts as gcc builds them against what kencc builds from the
# same source. Two differences make on-disk and on-wire structs come out
# wrong under gcc:
#   - long: kencc's long is 32 bit on every arch, gcc's is 64 bit on
#     amd64 (LP64), so a long member widens and shifts what follows;
#   - #pragma pack on/off: kencc then aligns and pads to 1 byte
#     (6c/8c swt.c align(): packflg), gcc does not know that syntax and
#     only warns (-Wpragmas); its form is #pragma pack(push,1)/pack(pop).
# The reference ("kencc") is the same tree compiled by gcc with long
# replaced by int everywhere (program and include files) and every
# `#pragma pack on/off' turned into pack(push,1)/pack(pop). Sizes come
# from probe arrays `char __sz_X[sizeof(struct X)]' (nm -S).
#
# usage: scripts/port/cmp-struct-layout.sh
#            every known case (on-disk/on-wire structs), amd64 and 386
#        scripts/port/cmp-struct-layout.sh [-a arch] dir file 'cflags' name...
#            dir: program directory, file: what the probe includes
#            (a .c of dir, a header like <bootexec.h>, or pack:path for a
#            header included between #pragma pack on/off as libmach does
#            with other architectures' ureg.h, or defs:file.c for only the
#            definitions of the named structs in file.c, when the file
#            itself needs generated headers); name: a struct
#            tag, =name for a typedef, or `auto' for every struct in the
#            #pragma pack regions of dir and its parent
# exit 1 if any layout differs
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
cd "$ROOT"
TMPD=$(mktemp -d); trap 'rm -rf "$TMPD"' EXIT

# long -> int (not in `long long', `long double'), pragma pack -> gcc's
kencc(){
	python3 - "$@" <<'PY'
import re, sys
for f in sys.argv[1:]:
	t = open(f, errors='replace').read()
	u = re.sub(r'^([ \t]*#[ \t]*pragma[ \t]+pack)[ \t]+(on|yes)\b.*$', r'\1(push,1)', t, flags=re.M)
	u = re.sub(r'^([ \t]*#[ \t]*pragma[ \t]+pack)[ \t]+off\b.*$', r'\1(pop)', u, flags=re.M)
	u = re.sub(r'(?<!long )(?<!long\t)\blong\b(?![ \t]+long)(?![ \t]+double)', 'int', u)
	if u != t:
		open(f, 'w').write(u)
PY
}

# struct tags in #pragma pack on ... off regions
packnames(){
	find "$@" -maxdepth 1 -name '*.[ch]' -exec awk '
		/^[ \t]*#[ \t]*pragma[ \t]+pack[ \t]+(on|yes)/ { p = 1 }
		/^[ \t]*#[ \t]*pragma[ \t]+pack[ \t]+off/ { p = 0 }
		p && match($0, /^(typedef[ \t]+)?struct[ \t]+[A-Za-z_][A-Za-z_0-9]*/) {
			s = substr($0, RSTART, RLENGTH); sub(/.*[ \t]/, "", s); print s }
		p && match($0, /^\}[ \t]*[A-Za-z_][A-Za-z_0-9]*[ \t]*;/) {
			s = substr($0, RSTART+1, RLENGTH-2); gsub(/[ \t]/, "", s); print "=" s }
	' {} + | sort -u
}

bad=0
cmp1(){	# arch dir file cflags name...
	local arch=$1 dir=$2 file=$3 cflags=$4; shift 4
	local m= cc=gcc
	case $arch in
	386)	m=-m32 ;;
	arm64)	cc=${TOOLPREFIX:-aarch64-linux-musl-}gcc ;;
	esac
	local par=$(dirname "$dir") rel=$(basename "$dir") names= n
	if [ "$1" = auto ]; then
		names=$(packnames "$dir" "$par"); shift
	fi
	names="$names $*"
	local w=$TMPD/$(echo "$arch $dir $file" | tr ' /<>' '____')
	for v in gcc ref; do
		mkdir -p $w/$v/inc
		mkdir -p $w/$v/par
		find "$par" -maxdepth 1 -name '*.[ch]' -exec cp {} $w/$v/par/ \;
		cp -r "$dir" $w/$v/par/
		case $file in pack:*) cp "${file#pack:}" $w/$v/par/$rel/__packed.h ;; esac
		cp -r sys/include $w/$v/inc/sys
		cp -r $arch/include $w/$v/inc/arch
		case $file in
		\<*)	echo "#include <u.h>"; echo "#include <libc.h>"; echo "#include $file" ;;
		defs:*)	echo "#include <u.h>"; echo "#include <libc.h>"
			for n in $names; do
				awk -v n=$n '$0 ~ "^struct[ \t]+" n "[ \t]*\\{?$" { p = 1 } p { print } p && /^\};/ { p = 0 }' "$dir/${file#defs:}"
			done ;;
		pack:*)	echo "#include <u.h>"; echo "#include <libc.h>"; echo "#pragma pack on"
			echo '#include "__packed.h"'; echo "#pragma pack off" ;;
		*)	echo "#include \"$file\"" ;;
		esac > $w/$v/par/$rel/__probe.c
		for n in $names; do
			case $n in
			=*)	echo "char __sz_${n#=}[sizeof(${n#=})];" ;;
			*)	echo "char __sz_$n[sizeof(struct $n)];" ;;
			esac
		done >> $w/$v/par/$rel/__probe.c
		[ $v = ref ] && kencc $(find $w/$v -name '*.[ch]')
		(cd $w/$v/par/$rel && $cc $m -std=gnu17 -fno-asm -ffreestanding -nostdinc -fno-builtin \
			-fplan9-extensions -fno-common -w -Wno-error=implicit-int \
			-Wno-error=incompatible-pointer-types -Wno-error=int-conversion \
			-Wno-error=implicit-function-declaration -Wno-error=return-mismatch \
			-I$w/$v/inc/arch -I$w/$v/inc/sys -I. $cflags -c __probe.c -o $w/$v.o) 2>&1 |
			grep -m3 error && { echo "$arch $dir: probe does not compile ($v)"; bad=1; return; }
	done
	for n in $names; do
		n=${n#=}
		local g=$(nm -S $w/gcc.o | awk -v s=__sz_$n '$4 == s { print strtonum("0x" $2) }')
		local k=$(nm -S $w/ref.o | awk -v s=__sz_$n '$4 == s { print strtonum("0x" $2) }')
		if [ "$g" != "$k" ]; then
			echo "$arch $dir: $n: gcc $g bytes, kencc $k"; bad=1
		fi
	done
}

if [ $# -gt 0 ]; then
	arch=amd64
	[ "$1" = -a ] && { arch=$2; shift 2; }
	cmp1 $arch "$@"
	exit $bad
fi

for arch in amd64 386; do
	cmp1 $arch sys/src/cmd/cwfs/cwfs64x ../dentry.c -I.. auto
	cmp1 $arch sys/src/cmd/cwfs/cwfs ../dentry.c -I.. auto
	cmp1 $arch sys/src/cmd/scat scat.c -I$ROOT/sys/src/cmd/map auto
	cmp1 $arch sys/src/games/doom r_data.c '' auto
	cmp1 $arch sys/src/cmd/5e proc.c '' auto
	cmp1 $arch sys/src/cmd/ip/snoopy defs:main.c '' pcap_file_header pcap_pkthdr
	cmp1 $arch sys/src/cmd/nusb/disk disk.c -I$ROOT/sys/src/cmd/nusb/lib Cbw Csw
	cmp1 $arch sys/src/libmach '<bootexec.h>' '' coffsect mipsexec mips4kexec sparcexec nextexec i386exec
	for u in arm 386 sparc power mips; do
		cmp1 $arch sys/src/libmach pack:$u/include/ureg.h '' Ureg
	done
done
exit $bad
