#!/usr/bin/env python3
"""
Dump libthread's procs and threads of a process in a running guest,
each blocked thread with its stack: what `acid -l thread' would show,
for gcc-built binaries (ELF/DWARF, which acid cannot read).

It reads the process's memory through /proc/PID/mem in the guest, over
the serial rc of scripts/port/qemu-ctl.py (see there for the QEMU
setup): from _threadpq the Proc list, per Proc its Thread list
(Thread.nextt). A thread that does not run has its context in
Thread.sched (setjmp: sp, pc, bp in the first three words, amd64 and
386, see libc/$objtype/setjmp.S); its stack follows from the bp chain
(everything is built with -fno-omit-frame-pointer). Struct offsets come
from the binary's DWARF (gdb ptype /o), addresses are resolved with
addr2line. Give the unstripped binary of the build (/$objtype/bin/...),
not the stripped one from the CD.

usage: scripts/port/thread-dump.py DIR PID BINARY
   e.g. scripts/port/thread-dump.py $W 307 amd64/bin/rio

Found the rio hang of readmouse() (Teil 36 in MEMORY.md): every thread
of rio's main proc waiting in alt, mousethread in wcurrent() sending to
a winctl that sat in frselect -> readmouse.
"""
import importlib.util, os, re, subprocess, sys

here = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location('qemuctl', os.path.join(here, 'qemu-ctl.py'))
qemuctl = importlib.util.module_from_spec(spec)
spec.loader.exec_module(qemuctl)

STATE = ['Dead', 'Running', 'Ready', 'Rendezvous']	# libthread/threadimpl.h
CHAN = ['Channone', 'Chanalt', 'Chansend', 'Chanrecv']

def die(s):
	sys.exit('thread-dump: ' + s)

def layout(binary, structs):
	"""{struct: {member: offset}} from gdb's ptype /o"""
	args = ['gdb', '-batch', '-nx']
	for s in structs:
		args += ['-ex', 'ptype /o struct ' + s]
	out = subprocess.run(args + [binary], capture_output=True, text=True).stdout
	res, cur, depth = {}, None, 0
	for l in out.split('\n'):
		m = re.search(r'type = struct (\w+) \{', l)
		if m:
			cur, depth = m.group(1), 1
			res[cur] = {}
			continue
		if cur is None:
			continue
		m = re.match(r'/\*\s+(\d+)\s+\|\s+\d+\s+\*/\s+(.*);$', l)
		if m and depth == 1:
			name = re.split(r'[\s*]+', re.sub(r'\[.*\]', '', m.group(2)).strip())[-1]
			res[cur][name] = int(m.group(1))
		depth += l.count('{') - l.count('}')
		if depth <= 0:
			cur = None
	for s in structs:
		if s not in res:
			die('no struct %s in %s (unstripped binary with DWARF?)' % (s, binary))
	return res

def main():
	if len(sys.argv) != 4:
		sys.exit(__doc__)
	d, pid, binary = sys.argv[1:4]
	elf = open(binary, 'rb').read(20)
	if elf[:4] != b'\x7fELF':
		die('%s: not ELF' % binary)
	machine = int.from_bytes(elf[18:20], 'little')
	if machine not in (3, 62):	# EM_386, EM_X86_64
		die('%s: only 386 and amd64 (jmp_buf, frame layout)' % binary)
	W = 8 if elf[4] == 2 else 4

	L = layout(binary, ['Pqueue', 'Proc', 'Thread', 'Tqueue'])
	P, T, Q = L['Proc'], L['Thread'], L['Tqueue']
	nm = subprocess.run(['nm', binary], capture_output=True, text=True).stdout
	m = re.search(r'^([0-9a-f]+) [BbDd] _threadpq$', nm, re.M)
	if not m:
		die('%s: no _threadpq (not a libthread program?)' % binary)
	pq = int(m.group(1), 16)

	def mem(addr, n):
		a0 = addr - addr % W
		cnt = (addr + n - a0 + W - 1) // W
		out = qemuctl.serial(d, 'dd -if /proc/%s/mem -bs %d -count %d -iseek %d >[2]/dev/null | xd -1x'
			% (pid, W, cnt, a0 // W), 2.5)
		b = b''
		for l in out.split('\n'):
			f = l.split()
			if len(f) > 1 and re.fullmatch(r'[0-9a-f]{7,}', f[0]):
				b += bytes(int(x, 16) for x in f[1:] if re.fullmatch(r'[0-9a-f]{2}', x))
		return b[addr - a0: addr - a0 + n]
	def word(b, o):
		return int.from_bytes(b[o:o+W], 'little')
	def i32(b, o):
		return int.from_bytes(b[o:o+4], 'little')
	def cstr(a):
		return mem(a, 32).split(b'\0')[0].decode(errors='replace') if a else ''
	def sym(a):
		o = subprocess.run(['addr2line', '-f', '-e', binary, '%x' % a], capture_output=True, text=True).stdout.split('\n')
		return '%-22s %s' % (o[0], o[1].split('/sys/src/')[-1] if len(o) > 1 else '')

	if not qemuctl.serial(d, 'echo thread-dump-ok', 2).count('thread-dump-ok'):
		die('no answer from the serial rc in %s (see scripts/port/qemu-ctl.py)' % d)
	psize = max(P.values()) + W
	tsize = max(T.values()) + W
	p = word(mem(pq + L['Pqueue']['head'], W), 0)
	if not p:
		die('pid %s: _threadpq is empty (wrong binary or pid?)' % pid)
	while p:
		pr = mem(p, psize)
		print('proc %d  ready.asleep=%d ready.head=%x nthreads=%d running=%x' % (i32(pr, P['pid']),
			i32(pr, P['ready'] + Q['asleep']), word(pr, P['ready'] + Q['head']),
			i32(pr, P['nthreads']), word(pr, P['thread'])))
		t = word(pr, P['threads'] + Q['head'])
		while t:
			th = mem(t, tsize)
			st, ch = i32(th, T['state']), i32(th, T['chan'])
			print('  thread %d %-10s %-9s %s  rendtag=%x inrendez=%d alt=%x' % (i32(th, T['id']),
				STATE[st] if st < len(STATE) else st, CHAN[ch] if ch < len(CHAN) else ch,
				cstr(word(th, T['cmdname'])), word(th, T['rendtag']), i32(th, T['inrendez']), word(th, T['alt'])))
			if st != 1:	# not running: context in sched
				pc, bp = word(th, T['sched'] + W), word(th, T['sched'] + 2*W)
				print('      %x  %s' % (pc, sym(pc)))
				for _ in range(20):
					if not bp or bp % W:
						break
					fr = mem(bp, 2*W)
					if len(fr) < 2*W:
						break
					nbp, ret = word(fr, 0), word(fr, W)
					print('      %x  %s' % (ret, sym(ret - 1)))
					if nbp <= bp:
						break
					bp = nbp
			t = word(th, T['nextt'])
		p = word(pr, P['next'])

if __name__ == '__main__':
	main()
