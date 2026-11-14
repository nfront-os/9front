#!/usr/bin/env python3
# Boot the install/live CD in QEMU the way a user does and report whether
# it came up: press Enter (the default) at each of 9front's boot prompts
# -- bootargs, user, vgasize, monitor, mouseport -- until rio's grey
# screen shows, then type into rio's window a command that writes a
# marker and the boot device to the serial line ('#t/eia0'; the CD's
# plan9.ini has no console=0, so the serial line is otherwise quiet).
#
# usage: drive.py iso cd|disk bios|uefi workdir
#   exit 0 when the marker arrived; the serial log is workdir/serial.log
#   env: QEMU_EXTRA  more QEMU options (e.g. "-vga none -device virtio-vga")
#        SHOW        files whose contents go to the serial line with the
#                    marker (e.g. /dev/vgactl: whose screen it is)
import json, os, shutil, socket, subprocess, sys, time

iso, media, fw, work = sys.argv[1:5]
MARK = 'iso-boot-ok'
os.makedirs(work, exist_ok=True)
sock = os.path.join(work, 'qmp.sock')
serial = os.path.join(work, 'serial.log')
for f in (sock, serial):
	if os.path.exists(f):
		os.remove(f)

q = ['qemu-system-x86_64', '-m', '1024', '-display', 'none', '-no-reboot',
	'-cpu', 'qemu64,+rdrand', '-qmp', 'unix:%s,server,nowait' % sock,
	'-serial', 'file:' + serial, '-netdev', 'user,id=n0', '-device', 'e1000,netdev=n0']
q += os.environ.get('QEMU_EXTRA', '').split()
if os.environ.get('KVM', '1') != '0':	# KVM always (KVM=0: emulation)
	if not os.access('/dev/kvm', os.W_OK):
		sys.exit('drive: /dev/kvm not writable (KVM=0 to emulate)')
	q += ['-accel', 'kvm']
if fw == 'uefi':
	code = os.environ.get('OVMF_CODE', '/usr/share/edk2/x64/OVMF_CODE.4m.fd')
	vars = os.path.join(work, 'vars.fd')
	shutil.copy(code.replace('CODE', 'VARS'), vars)
	q += ['-drive', 'if=pflash,format=raw,readonly=on,file=' + code,
		'-drive', 'if=pflash,format=raw,file=' + vars]
if media == 'cd':
	q += ['-drive', 'file=%s,format=raw,if=ide,index=2,media=cdrom,readonly=on' % iso]
else:	# the hybrid image as a disk (USB stick); QEMU writes to it: into a qcow2 overlay,
	# the image stays as it is (a copy of a CD past 2 GB per run filled the tmpfs of
	# tests/run.sh's work directory: the full profile's, disk with uefi after bios)
	disk = os.path.join(work, 'disk.qcow2')
	subprocess.run(['qemu-img', 'create', '-q', '-f', 'qcow2', '-F', 'raw',
		'-b', os.path.abspath(iso), disk], check=True)
	q += ['-drive', 'file=%s,format=qcow2,if=ide,index=0' % disk]
vm = subprocess.Popen(q, stdout=subprocess.DEVNULL, stderr=open(os.path.join(work, 'qemu.log'), 'w'))

def qmp():
	for _ in range(100):
		try:
			s = socket.socket(socket.AF_UNIX)
			s.connect(sock)
			f = s.makefile('rw')
			f.readline()
			return f
		except OSError:
			time.sleep(0.1)
	sys.exit('no QMP socket')
f = qmp()
def cmd(c):
	f.write(json.dumps(c) + '\n')
	f.flush()
	while True:
		r = json.loads(f.readline())
		if 'return' in r or 'error' in r:
			return r
cmd({'execute': 'qmp_capabilities'})

KEYS = {' ': 'spc', '-': 'minus', '/': 'slash', '.': 'dot', '\n': 'ret', "'": 'apostrophe',
	';': 'semicolon', '=': 'equal', ',': 'comma', '#': ('shift', '3'), '>': ('shift', 'dot'),
	'|': ('shift', 'backslash'), '*': ('shift', '8'), '{': ('shift', 'bracket_left'),
	'}': ('shift', 'bracket_right'), '!': ('shift', '1'), '$': ('shift', '4'),
	'(': ('shift', '9'), ')': ('shift', '0')}
def type_(text):
	for ch in text:
		k = KEYS.get(ch, ch)
		keys = k if isinstance(k, tuple) else (k,)
		r = cmd({'execute': 'send-key', 'arguments': {'keys': [{'type': 'qcode', 'data': x} for x in keys]}})
		if 'error' in r:
			sys.exit('send-key %r: %s' % (ch, r['error']))
		time.sleep(0.03)

def rio_up():
	# rio's background is grey 0x777777 (0x737573 at 16 bits per pixel):
	# most of the screen, unlike the text console (black), the EFI
	# console (teal, white) or the BIOS
	shot = os.path.join(work, 'screen.ppm')
	cmd({'execute': 'screendump', 'arguments': {'filename': shot}})
	try:
		d = open(shot, 'rb').read()
		_, w, h, _, px = d.split(maxsplit=4)
	except (OSError, ValueError):
		return False
	n = grey = 0
	for i in range(0, len(px) - 3, 3 * 997):
		n += 1
		r, g, b = px[i], px[i+1], px[i+2]
		grey += max(abs(r - 0x76), abs(g - 0x76), abs(b - 0x76)) < 6
	return n > 0 and grey > n // 2

def inkernel():
	# 9boot (or bootx64.efi) has jumped to the kernel; a key pressed
	# while the loader still runs stops its automatic boot at its > prompt
	# (KZERO: pc64 0xffffffff80000000, pc 0xf0000000)
	r = cmd({'execute': 'human-monitor-command', 'arguments': {'command-line': 'info registers'}})
	reg = r.get('return', '')
	return 'RIP=ffffffff8' in reg or 'EIP=f0' in reg	# 32-bit mode: EIP

def marked():
	try:
		return MARK in open(serial, errors='replace').read()
	except OSError:
		return False

ok = False
end = time.time() + float(os.environ.get('WAITMAX', '240'))
while time.time() < end and vm.poll() is None and not inkernel():
	time.sleep(1)
while time.time() < end and vm.poll() is None:
	if rio_up():
		time.sleep(3)	# riostart's windows
		show = os.environ.get('SHOW', '')
		show = '; cat ' + show if show else ''
		# and which window system runs: the desktop wsys or rio (tests/iso/run.sh)
		type_("{echo %s; ls -d /dev/sd*/data /dev/sd*/esp; if(ps | grep -s ' wsys$') echo desktop wsys; if not echo desktop rio%s} >'#t/eia0'\n" % (MARK, show))
		for _ in range(20):
			time.sleep(1)
			if marked():
				ok = True
				break
		break
	type_('\n')	# a boot prompt's default
	time.sleep(4)
if vm.poll() is None:	# the screen at the end, for the log
	cmd({'execute': 'screendump', 'arguments': {'filename': os.path.join(work, 'final.ppm')}})
	time.sleep(0.5)
vm.kill()
vm.wait()
print(open(serial, errors='replace').read().replace('\r', '').strip())
sys.exit(0 if ok else 1)
