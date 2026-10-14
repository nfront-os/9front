#!/usr/bin/env python3
"""
Drive a running 9front guest in QEMU from the host, for debugging what
hangs in the guest (rio, a file server) while the rest still runs:
type keys, take screenshots, ask QEMU's monitor, and run commands in a
second rc on the guest's serial line.

The guest's QEMU needs a QMP socket and the serial line as a socket,
both in one directory (unix socket paths are limited to 108 bytes, so
keep it short):

	qemu-system-x86_64 ... \\
		-qmp unix:DIR/qmp.sock,server,nowait \\
		-chardev socket,id=ser,path=DIR/serial.sock,server=on,wait=off \\
		-serial chardev:ser

and, once rio is up, an rc on the serial line (typed into a rio window,
or with `type' below), which then keeps working when rio does not:

	rc -i <'#t/eia0' >'#t/eia0' >[2=1] &

usage: scripts/debug/qemu-ctl.py DIR type 'text'     keys via QMP send-key (\\n for Enter)
       scripts/debug/qemu-ctl.py DIR ser 'cmd' [secs] a line to the serial rc, prints
                                                     what comes back within secs (3)
       scripts/debug/qemu-ctl.py DIR shot NAME        screendump to DIR/NAME.ppm
       scripts/debug/qemu-ctl.py DIR hmp 'cmd'        a QEMU monitor command, e.g.
                                                     'info registers -a', 'gdbserver tcp::1234'
scripts/debug/thread-dump.py uses `ser'.
"""
import json, os, socket, sys, time

KEYS = {' ': 'spc', '-': 'minus', '/': 'slash', '.': 'dot', '\n': 'ret', "'": 'apostrophe',
	';': 'semicolon', '=': 'equal', ',': 'comma', '#': ('shift', '3'), '>': ('shift', 'dot'),
	'<': ('shift', 'comma'), '&': ('shift', '7'), '[': 'bracket_left', ']': 'bracket_right',
	'|': ('shift', 'backslash'), '*': ('shift', '8'), '{': ('shift', 'bracket_left'),
	'}': ('shift', 'bracket_right'), '!': ('shift', '1'), '$': ('shift', '4'), '_': ('shift', 'minus'),
	'(': ('shift', '9'), ')': ('shift', '0'), '"': ('shift', 'apostrophe'), ':': ('shift', 'semicolon'),
	'\t': 'tab', '\\': 'backslash', '`': 'grave_accent', '~': ('shift', 'grave_accent'),
	'@': ('shift', '2'), '%': ('shift', '5'), '^': ('shift', '6'), '+': ('shift', 'equal'),
	'?': ('shift', 'slash')}

def qmp(d):
	s = socket.socket(socket.AF_UNIX)
	s.connect(os.path.join(d, 'qmp.sock'))
	f = s.makefile('rw')
	f.readline()
	def cmd(c):
		f.write(json.dumps(c) + '\n')
		f.flush()
		while True:
			r = json.loads(f.readline())
			if 'return' in r or 'error' in r:
				return r
	cmd({'execute': 'qmp_capabilities'})
	return cmd

def keys(ch):
	if ch.isupper():
		return ('shift', ch.lower())
	k = KEYS.get(ch, ch)
	return k if isinstance(k, tuple) else (k,)

def serial(d, line, secs=3.0):
	"""send line to the serial rc, return what arrives within secs"""
	s = socket.socket(socket.AF_UNIX)
	s.connect(os.path.join(d, 'serial.sock'))
	s.setblocking(False)
	try:	# what is left from before
		while s.recv(65536):
			pass
	except BlockingIOError:
		pass
	s.send(line.encode() + b'\n')
	out = b''
	end = time.time() + secs
	while time.time() < end:
		try:
			b = s.recv(65536)
			if b:
				out += b
				continue
		except BlockingIOError:
			pass
		time.sleep(0.1)
	s.close()
	return out.decode(errors='replace').replace('\r', '')

def main():
	if len(sys.argv) < 4:
		sys.exit(__doc__)
	d, what, arg = sys.argv[1:4]
	if what == 'type':
		cmd = qmp(d)
		for ch in arg.replace('\\n', '\n'):
			r = cmd({'execute': 'send-key', 'arguments': {'keys': [{'type': 'qcode', 'data': x} for x in keys(ch)]}})
			if 'error' in r:
				sys.exit('send-key %r: %s' % (ch, r['error']['desc']))
			time.sleep(0.03)
	elif what == 'ser':
		sys.stdout.write(serial(d, arg, float(sys.argv[4]) if len(sys.argv) > 4 else 3))
	elif what == 'shot':
		r = qmp(d)({'execute': 'screendump', 'arguments': {'filename': os.path.join(os.path.abspath(d), arg + '.ppm')}})
		if 'error' in r:
			sys.exit(r['error']['desc'])
	elif what == 'hmp':
		print(qmp(d)({'execute': 'human-monitor-command', 'arguments': {'command-line': arg}}).get('return', ''))
	else:
		sys.exit('unknown command %s' % what)

if __name__ == '__main__':
	main()
