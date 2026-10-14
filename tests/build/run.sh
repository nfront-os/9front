#!/bin/bash
# build: results of the build tool (CC=gcc ARCH=$objtype sys/src/cmd/build/build.exe)
. "$(dirname "$0")/../lib.sh"
cd "$ROOT"
L=$objtype/lib.gcc

dupsyms(){ nm "$1" 2>/dev/null | awk '$2=="T"{print $3}' | sort | uniq -d; }

libc(){
	[ -f $L/libc.a ] || { echo "$L/libc.a missing (run $BUILDHOW)"; return 1; }
	local want got d
	want=$(python3 -c 'import json, os; print(sum(len(e["SourceFiles"]) for f in ("sys/src/libc/libc%s.json" % os.environ["SUFFIX"], "sys/src/libc/%s/build.json" % os.environ["objtype"]) for e in json.load(open(f))))')
	got=$(ar t $L/libc.a | wc -l)
	[ "$want" -eq "$got" ] || { echo "libc.a has $got members, the manifests list $want sources"; return 1; }
	d=$(dupsyms $L/libc.a); [ -z "$d" ] || { echo "duplicate symbols: $d"; return 1; }
	# _notify: the raw stub behind the trampoline notify() (amd64, arm64)
	for s in _main _callmain _tos argv0 seek _seek notify $( [ $objtype != 386 ] && echo _notify ) nsec _nsec _exits \
		memmove memcpy strlen getfcr setjmp longjmp _tas $( [ $objtype != arm64 ] && echo cas64 ) toupperrune tmfmt calloc; do
		nm $L/libc.a 2>/dev/null | grep -qE " [TC] $s$" || { echo "libc.a lacks $s"; return 1; }
	done
}
check "libc.a: one member per manifest source, no duplicate symbols, entry points present" libc

libs(){
	local bad=0 d
	for l in $(python3 scripts/gen/genlibs.py --list); do
		[ -f $L/$l.a ] || { echo "$L/$l.a missing"; bad=1; continue; }
		d=$(dupsyms $L/$l.a); [ -z "$d" ] || { echo "$l.a: duplicate symbols $d"; bad=1; }
	done
	return $bad
}
check "all generated libraries built, no duplicate symbols" libs

cmds(){
	python3 - <<'PY'
import json, os, subprocess, sys
bad = n = 0
for dp, _, fs in [w for top in ('sys/src/cmd', 'sys/src/games', 'sys/src/ape') for w in os.walk(top)]:
	m = 'cmds%s.json' % os.environ['SUFFIX']
	if m not in fs:
		continue
	for e in json.load(open(os.path.join(dp, m))):
		if 'Library' in e or not e.get('Install'):	# archives; rc scripts (Post install)
			continue
		inst = e['Install'].replace('$ARCH', os.environ['objtype']).lstrip('/')
		names = [os.path.splitext(f)[0] for f in e.get('SourceFilesCmd', [])] or [e['Program']]
		for t in names:
			n += 1
			p = os.path.join(inst, t)
			if not os.path.isfile(p):
				print('missing', p); bad += 1; continue
			h = subprocess.run(['readelf', '-h', p], capture_output=True, text=True).stdout
			cls, mach = {'amd64': ('ELF64', 'X86-64'), '386': ('ELF32', '80386'), 'arm64': ('ELF64', 'AArch64')}[os.environ['objtype']]
			if cls not in h or 'EXEC' not in h or mach not in h:
				print('not an %s %s executable:' % (mach, cls), p); bad += 1
print(n, 'commands checked')
sys.exit(bad != 0)
PY
}
check "every command of the cmds.json manifests installed as $objtype ELF executable" cmds

# /$objtype/plan9.ld: like 8l, a code segment (R E) and a data segment (RW)
# with .rodata -- string literals writable, nothing writable+executable
segments(){
	python3 - <<'PY'
import os, subprocess, sys
bad = n = 0
A = os.environ['objtype']
paths = [A + '/init'] + [os.path.join(dp, f) for dp, _, fs in os.walk(A + '/bin') for f in fs]
for p in paths:
	if p == os.environ['KBIN'] or open(p, 'rb').read(4) != b'\x7fELF':
		continue
	n += 1
	out = subprocess.run(['readelf', '-lSW', p], capture_output=True, text=True).stdout
	# LOAD Offset VirtAddr PhysAddr FileSiz MemSiz Flg Align; Flg is "R E" or "RW"
	flags = [' '.join(l.split()[6:-1]) for l in out.splitlines() if l.split()[:1] == ['LOAD']]
	if flags != ['R E', 'RW']:
		print(p, 'segments', flags)
		bad += 1
		continue
	# sbrk starts the heap at end: it must be the end of the data segment
	ld = [l.split() for l in out.splitlines() if l.split()[:1] == ['LOAD']][1]
	top = int(ld[2], 16) + int(ld[5], 16)
	syms = subprocess.run(['nm', p], capture_output=True, text=True).stdout.split()
	if 'end' in syms and int(syms[syms.index('end') - 2], 16) != top:
		print(p, 'end %s, data segment ends %#x' % (syms[syms.index('end') - 2], top))
		bad += 1
	for l in out.splitlines():
		w = l.replace('[', ' ').replace(']', ' ').split()
		if '.rodata' in w and 'PROGBITS' in w and int(w[w.index('.rodata') + 2], 16) < int(ld[2], 16):
			print(p, '.rodata in the text segment')
			bad += 1
print(n, 'programs checked')
sys.exit(bad != 0)
PY
}
check "programs: a R E code and a RW data segment holding .rodata, end at its end (plan9.ld)" segments

kernel(){
	local k=$KBIN kz
	[ -f $k ] || { echo "$k missing"; return 1; }
	kz=$(sed -n 's/^#define[ \t]*KZERO[ \t]*(*\(0x[0-9a-fA-F]*\).*/\1/p' sys/src/9/$KDIR/mem.h 2>/dev/null | head -1)
	readelf -lW $k | awk -v kz=$kz '$1=="LOAD"{ v=strtonum($3); p=strtonum($4); if(v-p != strtonum(kz)){ print "LOAD vaddr " $3 " paddr " $4; bad=1 } } END{ exit bad }'
}
check "kernel: LOAD segments with paddr = vaddr - KZERO (multiboot)" kernel

# gcc may hoist the load of a kencc-style busy-wait (`while(l->key) ;')
# out of the loop: a branch to itself (scripts/check/find-spinloops.sh)
spinloops(){
	local k bad=0
	for k in amd64/bin/9pc64 386/bin/9pc arm64/bin/9qemu arm64/bin/9pi3 arm64/bin/9pi4; do
		[ -f $k ] || continue
		scripts/check/find-spinloops.sh $k || { echo "in $k"; bad=1; }
	done
	return $bad
}
check "kernels: no busy-wait compiled into a branch to itself" spinloops
done_tests
