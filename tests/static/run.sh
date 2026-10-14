#!/bin/bash
# static: source-level checks of the gcc port, no build needed
. "$(dirname "$0")/../lib.sh"
cd "$ROOT"

# 1. manifests are what the generators produce from 9front's mkfiles
manifests(){
	local files="sys/src/libc/libc.json sys/src/libc/amd64/build.json
		$(ls sys/src/lib*/lib*.json) $(find sys/src/cmd -name cmds.json)"
	for f in $files; do cp "$f" "$TMPD/$(echo $f | tr / _)"; done
	python3 scripts/genlibc.py >/dev/null && python3 scripts/genlibs.py >/dev/null &&
		python3 scripts/gencmds.py >/dev/null || return 1
	local bad=0
	for f in $files; do
		if ! cmp -s "$f" "$TMPD/$(echo $f | tr / _)"; then
			echo "$f differs from its generator's output"; bad=1
			cp "$TMPD/$(echo $f | tr / _)" "$f"	# leave the tree as it was
		fi
	done
	return $bad
}
check "manifests match scripts/gen{libc,libs,cmds}.py" manifests

# 2. no 32-bit ulong compared against the 64-bit ~0UL (scripts/port/fixulongmax.py)
ulongmax(){
	! grep -rnP "[!=]=\s*~0UL\b(?!L)|~0UL\s*[!=]=" sys/src/9/port sys/src/9/pc sys/src/9/ip \
		sys/src/lib* sys/src/cmd --include=*.c | grep -vE "/cmd/(cc|[5678kqv][ac]|gs)/"
}
check "no ulong == / != ~0UL (kernel, libraries, commands)" ulongmax

# 3. explicit casts to an unnamed member's type: gcc does not adjust the
#    address like 8c; every remaining cast must be reviewed (allow list)
casts(){
	python3 scripts/port/find-embedded-casts.py sys/src/lib* sys/src/cmd sys/src/9/port \
		sys/src/9/pc sys/src/9/ip | grep -v '^	' | sed 's/:[0-9]*:/:/' | sort -u > "$TMPD/casts"
	comm -23 "$TMPD/casts" <(sort -u tests/static/embedded-casts.allow) | grep . && return 1
	return 0
}
check "no unreviewed casts to embedded member types" casts

# 3b. casts the port added relative to 9front: only pointer-to-pointer
#     ones (void** -> T**, which gcc requires) and documented ones
#     (/* GCC port: ... */, e.g. union members) may remain; member
#     conversions must stay implicit as in 9front (gcc's
#     -fplan9-extensions does them right, an explicit cast does not)
addedcasts(){
	python3 scripts/port/find-added-casts.py | grep -v '\*\*)' | grep -v 'GCC port' | grep . && return 1
	return 0
}
check "no casts added to 9front sources except T** conversions" addedcasts
check "mouse.h embeds Mouse like 9front" grep -qP '^\tMouse;' sys/include/mouse.h

# 3c. member paths must not lose a level against 9front (clunkq.q.head ->
#     clunkq.head changed the kernel's closeproc logic)
check "no member path shortened against 9front" python3 scripts/port/find-shortened-members.py

# 4. enumerators with a vlong value: kencc keeps the type, gcc makes them int
enums(){
	python3 - sys/src/lib* sys/src/cmd sys/src/9/port sys/src/9/pc sys/src/9/ip <<'PY'
import os, re, sys
bad = 0
for d in sys.argv[1:]:
	for dp, _, fs in os.walk(d):
		for f in fs:
			if not f.endswith(('.c', '.h')):
				continue
			p = os.path.join(dp, f)
			text = open(p, errors='replace').read()
			for m in re.finditer(r'\benum\s*\w*\s*\{(.*?)\}', text, re.S):
				for e in re.finditer(r'(\w+)\s*=\s*\((u?vlong|uintptr)\)', m.group(1)):
					print('%s: %s' % (p, e.group(0))); bad = 1
sys.exit(bad)
PY
}
check "no (u)vlong-valued enumerators" enums

# 4b. on-disk/on-wire structs laid out as kencc does: long 32 bit,
#     #pragma pack on honored (cwfs, scat, doom, snoopy, nusb/disk,
#     bootexec.h, libmach's ureg.h; amd64 and 386)
check "file and wire struct layouts match kencc's" scripts/port/cmp-struct-layout.sh

# 5. amd64 u.h: 8c intrinsics and the System V jmp_buf
# arguments through ... are promoted to int (double); va_arg of a
# narrower type is undefined -- gcc compiles it into a trap (ud2), kencc
# read the low bytes
check "no va_arg of a promoted type (char, short, float)" bash -c \
	"! git grep -n -E 'va_arg\([^,]+, *(uchar|ushort|char|short|unsigned char|unsigned short|signed char|u8int|u16int|s8int|s16int|float)\)' -- sys/src"
check "u.h: SET/USED are variadic no-ops" grep -q '^#define SET(...) ((void)0)' amd64/include/u.h
check "u.h: jmp_buf holds sp, pc and 6 callee-saved registers" grep -q 'jmp_buf\[8\]' amd64/include/u.h

# 6. the 6a sources stay 9front's (for 6a on Plan 9), each with its GAS
#    port beside it (foo.S, as the kernel's l.S beside l.s)
asmpairs(){
	local bad=0
	for d in sys/src/libc/amd64 sys/src/ape/lib/ap/amd64 sys/src/ape/lib/9/amd64; do
		git diff --quiet front -- "$d"/*.s || { echo "$d: .s differs from front"; bad=1; }
		for s in "$d"/*.s; do
			[ -f "${s%.s}.S" ] || { echo "$s: no GAS port ${s%.s}.S"; bad=1; }
		done
	done
	return $bad
}
check "amd64 assembly: 9front's .s unchanged, each with a GAS .S" asmpairs
check "GAS ports: no 6a assembly in them" \
	bash -c '! grep -nP "^(TEXT|GLOBL)\s|^\t[A-Z]{2,}[A-Z0-9]*\t" sys/src/libc/amd64/*.S sys/src/ape/lib/ap/amd64/*.S sys/src/ape/lib/9/amd64/*.S'

done_tests
