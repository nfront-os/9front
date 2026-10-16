#!/bin/bash
# asm: host unit tests of the GAS ports beside 9front's assembly:
# sys/src/libc/amd64/*.S (System V), libmp and libsec ($objtype amd64
# and 386) and lib387 (sys/src/libc/386/387), each against glibc or the
# portable C 9front uses without them; 386 as -m32 programs.
. "$(dirname "$0")/../lib.sh"
HERE=$(cd "$(dirname "$0")" && pwd)
A=$ROOT/sys/src/libc/amd64

# assemble each port for the host with a p9_ prefix on its symbols
build(){
	for f in memmove memcpy memset memchr memccpy memcmp strlen strcpy strcat \
		strchr atom atomic atomic64 muldiv getcallerpc getfcr cycles tas; do
		sed 's/^\([a-z_][a-z0-9_]*\):/p9_\1:/; s/\.globl[ \t]*/.globl p9_/; s/, \([a-z_]\)/, p9_\1/g; s/\(call\|jmp\)\t\([a-z_]\)/\1\tp9_\2/' \
			"$A/$f.S" > "$TMPD/$f.s" || return 1
	done
	gcc -O1 -fno-omit-frame-pointer -o "$TMPD/asm_test" "$HERE/asm_test.c" \
		"$HERE/callcheck.s" "$TMPD"/*.s
}
if check "assemble libc/amd64 ports for the host" build; then
	check "libc/amd64 routines vs. glibc, callee-saved registers, DF" "$TMPD/asm_test"
fi

# port: assemble a port (prefix p9_) or compile a portable C file with
# the tree's headers (prefix c_) into dir; objcopy prefixes every symbol,
# the local labels and the references too
#	port m32|m64 dir p9|c file [include dir]
port(){
	local m=$1 d=$2 p=$3 f=$4 o
	o=$d/${p}_$(basename "${f%.*}").o
	if [ $p = p9 ]; then
		gcc -$m -c -o "$o" "$f" || return 1
	else
		local arch=amd64; [ $m = m32 ] && arch=386
		gcc -$m -c -O1 -ffreestanding -nostdinc -fno-builtin -fno-stack-protector -fno-pie \
			-fplan9-extensions -I"$ROOT/$arch/include" -I"$ROOT/sys/include" ${5:+-I"$5"} \
			-o "$o" "$f" || return 1
	fi
	objcopy --prefix-symbols=${p}_ "$o"
}

# libmp, libsec: the ports against sys/src/lib*/port's C
mpsec(){
	local arch=$1 m=$2 d=$TMPD/$1 cc=callcheck.s f
	[ $arch = 386 ] && cc=callcheck386.s
	mkdir -p "$d/mp" "$d/sec"
	for f in "$ROOT"/sys/src/libmp/$arch/*.S; do port $m "$d/mp" p9 "$f" || return 1; done
	for f in mpdigdiv mpvecadd mpvecsub mpvecdigmuladd; do
		port $m "$d/mp" c "$ROOT/sys/src/libmp/port/$f.c" "$ROOT/sys/src/libmp/port" || return 1
	done
	for f in "$ROOT"/sys/src/libsec/$arch/*.S; do port $m "$d/sec" p9 "$f" || return 1; done
	for f in md5block sha1block $( [ $arch = amd64 ] && echo chachablock aes ); do
		port $m "$d/sec" c "$ROOT/sys/src/libsec/port/$f.c" "$ROOT/sys/src/libsec/port" || return 1
	done
	gcc -$m -O1 -no-pie -o "$d/mp_test" "$HERE/mp_test.c" "$HERE/$cc" "$d"/mp/*.o &&
	gcc -$m -O1 -no-pie -o "$d/sec_test" "$HERE/sec_test.c" "$HERE/$cc" "$d"/sec/*.o
}
if check "assemble libmp/libsec amd64 ports, compile the port C" mpsec amd64 m64; then
	check "libmp amd64 vs. port C, callee-saved registers, DF" "$TMPD/amd64/mp_test"
	check "libsec amd64 (md5, sha1, chacha, AES-NI) vs. port C, callee-saved, DF" "$TMPD/amd64/sec_test"
fi

# 386: needs gcc -m32 (multilib)
if echo 'int main(void){return 0;}' | gcc -m32 -x c -o "$TMPD/m32" - 2>/dev/null; then
	if check "assemble libmp/libsec 386 ports, compile the port C" mpsec 386 m32; then
		check "libmp 386 vs. port C, callee-saved registers, DF" "$TMPD/386/mp_test"
		check "libsec 386 (md5, sha1) vs. port C, callee-saved registers, DF" "$TMPD/386/sec_test"
	fi
	fp387(){
		mkdir -p "$TMPD/387"
		for f in "$ROOT"/sys/src/libc/386/387/*.S; do port m32 "$TMPD/387" p9 "$f" || return 1; done
		gcc -m32 -O1 -no-pie -o "$TMPD/387/fp_test" "$HERE/fp387_test.c" "$HERE/callcheck386.s" \
			"$TMPD"/387/*.o -lm
	}
	if check "assemble lib387 ports" fp387; then
		check "lib387 vs. the host's x87 libm, x87 stack balance, callee-saved" "$TMPD/387/fp_test"
	fi
else
	skip "386 ports" "no gcc -m32"
fi
done_tests
