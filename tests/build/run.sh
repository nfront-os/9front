#!/bin/bash
# build: results of scripts/build-cli.sh
. "$(dirname "$0")/../lib.sh"
cd "$ROOT"
L=amd64/lib

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
for dp, _, fs in os.walk('sys/src/cmd'):
	if 'cmds.json' not in fs:
		continue
	for e in json.load(open(os.path.join(dp, 'cmds.json'))):
		inst = e['Install'].replace('/$ARCH', 'amd64').lstrip('/')
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

kernel(){
	local k=amd64/bin/9pc64
	[ -f $k ] || { echo "$k missing"; return 1; }
	readelf -lW $k | awk '$1=="LOAD"{ v=strtonum($3); p=strtonum($4); if(v-p != 0xffffffff80000000){ print "LOAD vaddr " $3 " paddr " $4; bad=1 } } END{ exit bad }'
}
check "kernel: LOAD segments with paddr = vaddr - KZERO (multiboot)" kernel
done_tests
