"""
A 9front CD booted in QEMU the way a user does, for the tests that need
the running system (tests/rio, tests/usb): every boot prompt answered
with its default (Enter) until rio's grey screen shows, then an rc on
the serial line ('#t/eia0') as a way in that keeps working when rio
does not. Keys and mouse go through QMP like a user's.

	vm = VM(iso, work, objtype='amd64', extra=['-device', 'qemu-xhci,id=usb'])
	vm.boot()                      # up to rio, serial rc running
	vm.type("echo hi >'#t/eia0'\\n")
	vm.serial('ps -a')             # what the serial rc prints
	vm.close()

Environment: KVM=0 (no KVM), CPU, SMP, MEM (QEMU's -cpu, -smp, -m).
"""
import json, os, socket, subprocess, sys, tempfile, time

KEYS = {' ': 'spc', '-': 'minus', '/': 'slash', '.': 'dot', '\n': 'ret', "'": 'apostrophe',
	';': 'semicolon', '=': 'equal', ',': 'comma', '#': ('shift', '3'), '>': ('shift', 'dot'),
	'<': ('shift', 'comma'), '&': ('shift', '7'), '[': 'bracket_left', ']': 'bracket_right',
	'|': ('shift', 'backslash'), '*': ('shift', '8'), '{': ('shift', 'bracket_left'),
	'}': ('shift', 'bracket_right'), '!': ('shift', '1'), '$': ('shift', '4'), '_': ('shift', 'minus'),
	'(': ('shift', '9'), ')': ('shift', '0')}

class VM:
	def __init__(self, iso, work, objtype='amd64', extra=()):
		self.work = work
		os.makedirs(work, exist_ok=True)
		# unix socket paths are limited to 108 bytes
		self.sock = tempfile.mkdtemp(prefix='vm')
		qsock, ssock = os.path.join(self.sock, 'qmp'), os.path.join(self.sock, 'ser')
		q = ['qemu-system-i386' if objtype == '386' else 'qemu-system-x86_64',
			'-m', os.environ.get('MEM', '1024'), '-smp', os.environ.get('SMP', '1'),
			'-cpu', os.environ.get('CPU', 'qemu32' if objtype == '386' else 'qemu64'),
			'-display', 'none', '-no-reboot',
			'-qmp', 'unix:%s,server,nowait' % qsock,
			'-chardev', 'socket,id=ser,path=%s,server=on,wait=off' % ssock, '-serial', 'chardev:ser',
			'-netdev', 'user,id=n0', '-device', 'e1000,netdev=n0',
			'-drive', 'file=%s,format=raw,if=ide,index=2,media=cdrom,readonly=on' % iso]
		if os.access('/dev/kvm', os.W_OK) and os.environ.get('KVM', '1') != '0':
			q += ['-accel', 'kvm']
		self.vm = subprocess.Popen(q + list(extra), stdout=subprocess.DEVNULL,
			stderr=open(os.path.join(work, 'qemu.log'), 'w'))
		self.q = self._connect(qsock).makefile('rw')
		self.q.readline()
		self.cmd({'execute': 'qmp_capabilities'})
		self.ser = self._connect(ssock)
		self.ser.setblocking(False)
		self.log = open(os.path.join(work, 'serial.log'), 'ab')

	def _connect(self, path):
		for _ in range(200):
			try:
				s = socket.socket(socket.AF_UNIX)
				s.connect(path)
				return s
			except OSError:
				if self.vm.poll() is not None:
					sys.exit('qemu exited, see %s/qemu.log' % self.work)
				time.sleep(0.1)
		sys.exit('no socket ' + path)

	def cmd(self, c):
		self.q.write(json.dumps(c) + '\n')
		self.q.flush()
		while True:
			r = json.loads(self.q.readline())
			if 'return' in r or 'error' in r:
				return r

	def type(self, text):
		for ch in text:
			k = ('shift', ch.lower()) if ch.isupper() else KEYS.get(ch, ch)
			keys = k if isinstance(k, tuple) else (k,)
			r = self.cmd({'execute': 'send-key', 'arguments': {'keys': [{'type': 'qcode', 'data': x} for x in keys]}})
			if 'error' in r:
				sys.exit('send-key %r: %s' % (ch, r['error']['desc']))
			time.sleep(0.03)

	def _ev(self, *e):
		self.cmd({'execute': 'input-send-event', 'arguments': {'events': list(e)}})

	def move(self, dx, dy, step=3, pause=0.005):
		"""relative mouse motion, in pieces of at most step: the kernel
		accelerates steps of more than 3 (devmouse.c scale()), so small
		ones land where they are meant to"""
		while dx or dy:
			sx, sy = max(-step, min(step, dx)), max(-step, min(step, dy))
			self._ev({'type': 'rel', 'data': {'axis': 'x', 'value': sx}},
				{'type': 'rel', 'data': {'axis': 'y', 'value': sy}})
			dx -= sx
			dy -= sy
			time.sleep(pause)

	def button(self, b, down):
		"""b: left, middle, right"""
		self._ev({'type': 'btn', 'data': {'down': down, 'button': b}})
		time.sleep(0.15)

	def shot(self, name):
		f = os.path.join(self.work, name + '.ppm')
		self.cmd({'execute': 'screendump', 'arguments': {'filename': f}})
		time.sleep(0.5)
		return f

	def rio_up(self):
		"""rio's grey background on most of the screen"""
		try:
			d = open(self.shot('screen'), 'rb').read()
			_, w, h, _, px = d.split(maxsplit=4)
		except (OSError, ValueError):
			return False
		n = grey = 0
		for i in range(0, len(px) - 3, 3 * 997):
			n += 1
			grey += max(abs(px[i] - 0x76), abs(px[i+1] - 0x76), abs(px[i+2] - 0x76)) < 6
		return n > 0 and grey > n // 2

	def inkernel(self):
		r = self.cmd({'execute': 'human-monitor-command', 'arguments': {'command-line': 'info registers'}})
		reg = r.get('return', '')
		return 'RIP=ffffffff8' in reg or 'EIP=f0' in reg

	def boot(self, wait=300):
		"""to rio, taking every prompt's default; then the serial rc"""
		end = time.time() + wait
		while time.time() < end and not self.inkernel():
			time.sleep(1)
		while time.time() < end and not self.rio_up():
			self.type('\n')
			time.sleep(4)
		if not self.rio_up():
			return False
		time.sleep(3)	# riostart's windows
		# click into the first window: keyboard focus, as a user would
		self.move(-3000, -3000, step=100)	# corner: overshoot is clamped
		self.move(200, 250)
		self.button('left', True)
		self.button('left', False)
		time.sleep(0.5)
		self.type("rc -i <'#t/eia0' >'#t/eia0' >[2=1] &\n")
		time.sleep(2)
		return 'serial-ok' in self.serial('echo serial-ok', 3)

	def read(self, secs):
		out = b''
		end = time.time() + secs
		while time.time() < end:
			try:
				d = self.ser.recv(65536)
				if d:
					out += d
					self.log.write(d)
					self.log.flush()
					continue
			except BlockingIOError:
				pass
			time.sleep(0.1)
		return out.decode(errors='replace').replace('\r', '')

	def serial(self, line, secs=4):
		"""a line to the serial rc; what it prints within secs"""
		self.read(0.2)
		self.ser.send(line.encode() + b'\n')
		return self.read(secs)

	def close(self):
		if self.vm.poll() is None:
			self.vm.kill()
			self.vm.wait()
		for f in os.listdir(self.sock):
			os.remove(os.path.join(self.sock, f))
		os.rmdir(self.sock)
