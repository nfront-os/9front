#!/usr/bin/env python3
"""
Generate gcc build-tool manifests (sys/src/cmd/.../cmds.json) for 9front
userspace commands.

Two kinds of entries, following 9front's mkfiles:
  - single: directories of one-file commands (sys/src/cmd/mkfile, mkmany
    without OFILES): one SourceFilesCmd entry; each x.c becomes x.
    The lists name the commands that build with gcc today (trial-built,
    see MEMORY.md Teil 18); the rest need libraries not ported yet
    (libdraw, libmemdraw, libmach, libttf, ...) or source fixes.
  - prog: mkone/mkmany directories with shared OFILES and an optional
    yacc grammar. Objects come from the mkfile; yacc runs through
    scripts/yacc.sh (Plan 9 yacc with 9front's yaccpar).

All commands link against the same library set; static linking only
pulls in what a command uses.

usage: scripts/gencmds.py        (rewrites every manifest listed below)
"""
import json
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
CMD = os.path.join(ROOT, 'sys', 'src', 'cmd')

# static link order; libsec (tlshand.c) calls back into libauth, so
# libauth is listed again after it
LIBS = ['libdisk', 'lib9p', 'libthread', 'libauth', 'libauthsrv', 'libndb', 'libip',
	'libsec', 'libauth', 'libmp', 'libflate', 'libregexp', 'libString', 'libbio', 'libc']

SINGLE = {
	'.': ('/$ARCH/bin/', '''
		aan alarm ascii basename bind cal calendar cat chgrp chmod
		cleanname cmp col comm cp crc32 date dc dd deroff derp du echo
		ecp ed factor fcp fmt fortune freq getmap html2ms idiff io
		iostats kbremap lnfs lock look ls md5sum mkdir mntgen mount
		ms2html mtime mv news ns os patch pbd p pcc pipefile pr primes
		ps pump pwd qr ramfs ratrace rdbfs read riow rm seconds sed seg
		seq sha1sum skelfs sleep sort split srv srvfs sshnet strings sum
		swap tail tar tee test time timepic touch touchfs tput tr uhtml
		unicode uniq unlnfs unmount urlencode walk wc xargs xd'''),
	'aux': ('/$ARCH/bin/aux/', '''
		9pcon acidleak astarld bflz cddb cdsh clog cpuid consolefs
		data2s dial disksim esd icanhasmsi icanhasvmx lines listen
		listen1 msexceltables mouse olefs pcmcia rdwr reboot searchfs
		seek stub tablet timesync trampoline txt2uimage unbflz usage
		usbsdmux write wacom wikifmt zerotrunc'''),
	'ip': ('/$ARCH/bin/ip/', '''
		6in4 ayiya dhcpclient hogports hproxy httpfile linklocal ping
		pppoe pptp pptpd rarpd rip rlogind telnet tftpd tftpfs
		traceroute torrent udpecho socksd'''),
	'ndb': ('/$ARCH/bin/ndb/', '''
		mkdb query mkhash mkhosts cs csquery dnsquery ipquery inform'''),
}

# dir -> (install dir, [(target, extra objects from the mkfile var)]),
# Pre steps; None objects = the mkfile's OFILES.
PROGS = {
	'grep':		('/$ARCH/bin/', None),
	'awk':		('/$ARCH/bin/', ['scripts/gen-awk-proctab.sh']),
	'ip/ipconfig':	('/$ARCH/bin/ip/', None),
	'disk/prep':	('/$ARCH/bin/disk/', None),
	# ndb's mkmany links dns/dnsdebug/dnsgetip from explicit object lists
	'ndb:dns':	('/$ARCH/bin/ndb/', 'DNSOBJ'),
	'ndb:dnsdebug':	('/$ARCH/bin/ndb/', 'DNSDEBUGOBJ'),
	'ndb:dnsgetip':	('/$ARCH/bin/ndb/', 'DNSGETIPOBJ'),
}

def mkvars(d):
	text = open(os.path.join(CMD, d, 'mkfile')).read().replace('\\\n', ' ')
	v = {}
	for m in re.finditer(r'^([A-Za-z_]+)\s*=(.*)$', text, re.M):
		v[m.group(1)] = m.group(2).split()
	return v, text

def objs2c(objs):
	return [o.replace('$O', 'c').replace('.$c', '.c') if o.endswith('.$O') else o for o in objs]

def libs():
	return ['/$ARCH/lib/%s.a' % l for l in LIBS]

def rel_scripts(d):
	depth = len(os.path.normpath(os.path.join('sys/src/cmd', d)).split('/'))
	return '../' * depth + 'scripts/'

def base(name, install):
	return {'Name': name, 'Include': ['/$ARCH/cflags.json'], 'Install': install,
		'Oflags': ['--entry=_main'], 'Libs': libs()}

def write(d, entries):
	out = os.path.join(CMD, d, 'cmds.json')
	open(out, 'w').write(json.dumps(entries, indent='\t') + '\n')
	print(os.path.relpath(out, ROOT))

def main():
	byfile = {}
	for d, (install, names) in SINGLE.items():
		e = base('cmds-' + d.replace('/', '-').replace('.', 'cmd'), install)
		e['SourceFilesCmd'] = [n + '.c' for n in names.split()]
		for n in names.split():
			if not os.path.exists(os.path.join(CMD, d, n + '.c')):
				sys.exit('%s/%s.c missing' % (d, n))
		byfile.setdefault(d, []).append(e)
	for key, (install, extra) in PROGS.items():
		d, _, targ = key.partition(':')
		v, text = mkvars(d)
		pre = []
		y = v.get('YFILES', [])
		if y:
			m = re.search(r'^[^\n:]*:\s*\$YFILES\s*\n\t\$YACC (.*)$', text, re.M)
			yflags = m.group(1).replace('$YFLAGS', '-d').replace('$prereq', ' '.join(y)) if m else '-d ' + ' '.join(y)
			pre.append('bash %syacc.sh %s' % (rel_scripts(d), yflags))
		if isinstance(extra, list):
			pre += ['bash %s%s' % (rel_scripts(d), os.path.basename(s)) for s in extra]
		if targ:
			targets = [(targ, objs2c(v[extra]))]
		else:
			ofiles = objs2c(v.get('OFILES', []))
			tg = v['TARG']
			if len(tg) == 1:	# mkone
				targets = [(tg[0], ofiles)]
			else:			# mkmany: <targ>.c + OFILES
				targets = [(t, [t + '.c'] + ofiles) for t in tg]
		for t, src in targets:
			e = base(t, install)
			if pre:
				e['Pre'] = pre
				pre = []	# grammar/tables once per directory run
			e['Program'] = t
			e['SourceFiles'] = src
			byfile.setdefault(d, []).append(e)
	for d, entries in byfile.items():
		write(d, entries)

main()
