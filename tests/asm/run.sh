#!/bin/bash
# asm: host unit tests of sys/src/libc/amd64/*.S (GAS/System V ports)
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
done_tests
