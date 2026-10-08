#!/bin/bash
# build: results of scripts/build-cli.sh
. "$(dirname "$0")/../lib.sh"
cd "$ROOT"
L=amd64/lib.gcc

dupsyms(){ nm "$1" 2>/dev/null | awk '$2=="T"{print $3}' | sort | uniq -d; }

libc(){
	[ -f $L/libc.a ] || { echo "$L/libc.a missing (run scripts/build-cli.sh)"; return 1; }
	local want got d
	want=$(python3 -c 'import json; print(sum(len(e["SourceFiles"]) for f in ("sys/src/libc/libc.json","sys/src/libc/amd64/build.json") for e in json.load(open(f))))')
	got=$(ar t $L/libc.a | wc -l)
	[ "$want" -eq "$got" ] || { echo "libc.a has $got members, the manifests list $want sources"; return 1; }
	d=$(dupsyms $L/libc.a); [ -z "$d" ] || { echo "duplicate symbols: $d"; return 1; }
	for s in _main _callmain _tos argv0 seek _seek notify _notify nsec _nsec _exits \
		memmove memcpy strlen getfcr setjmp longjmp _tas cas64 toupperrune tmfmt calloc; do
		nm $L/libc.a 2>/dev/null | grep -qE " [TC] $s$" || { echo "libc.a lacks $s"; return 1; }
	done
}
check "libc.a: one member per manifest source, no duplicate symbols, entry points present" libc

libs(){
	local bad=0 d
	for l in $(python3 scripts/genlibs.py --list); do
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
	if 'cmds.json' not in fs:
		continue
	for e in json.load(open(os.path.join(dp, 'cmds.json'))):
		if 'Library' in e or not e.get('Install'):	# archives; rc scripts (Post install)
			continue
		inst = e['Install'].replace('$ARCH', 'amd64').lstrip('/')
		names = [os.path.splitext(f)[0] for f in e.get('SourceFilesCmd', [])] or [e['Program']]
		for t in names:
			n += 1
			p = os.path.join(inst, t)
			if not os.path.isfile(p):
				print('missing', p); bad += 1; continue
			h = subprocess.run(['readelf', '-h', p], capture_output=True, text=True).stdout
			if 'ELF64' not in h or 'EXEC' not in h or 'X86-64' not in h:
				print('not an x86-64 ELF64 executable:', p); bad += 1
print(n, 'commands checked')
sys.exit(bad != 0)
PY
}
check "every command of the cmds.json manifests installed as ELF64 executable" cmds

# amd64/plan9.ld: like 8l, a code segment (R E) and a data segment (RW)
# with .rodata -- string literals writable, nothing writable+executable
segments(){
	python3 - <<'PY'
import os, subprocess, sys
bad = n = 0
paths = ['amd64/init'] + [os.path.join(dp, f) for dp, _, fs in os.walk('amd64/bin') for f in fs]
for p in paths:
	if p.endswith('9pc64') or open(p, 'rb').read(4) != b'\x7fELF':
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
		if '.rodata' in w and 'PROGBITS' in w and int(w[w.index('.rodata') + 2], 16) < 0x600000:
			print(p, '.rodata in the text segment')
			bad += 1
print(n, 'programs checked')
sys.exit(bad != 0)
PY
}
check "programs: a R E code and a RW data segment holding .rodata, end at its end (plan9.ld)" segments

kernel(){
	local k=amd64/bin/9pc64
	[ -f $k ] || { echo "$k missing"; return 1; }
	readelf -lW $k | awk '$1=="LOAD"{ v=strtonum($3); p=strtonum($4); if(v-p != 0xffffffff80000000){ print "LOAD vaddr " $3 " paddr " $4; bad=1 } } END{ exit bad }'
}
check "kernel: LOAD segments with paddr = vaddr - KZERO (multiboot)" kernel
done_tests
