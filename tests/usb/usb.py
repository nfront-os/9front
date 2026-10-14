#!/usr/bin/env python3
"""
A USB stick (QEMU usb-storage) on each USB host controller the kernels
have drivers for: xHCI, EHCI, UHCI, OHCI. Per controller the CD boots to
rio once; checked through the serial rc: the controller is found, its
root hub is in #u, the stick is enumerated and nusb/disk serves it as
/dev/sdU*, and its first sector reads back what the image holds.
Regression for: the long fields of nusb/disk's CBW (amd64), xHCI
registers read as single bytes (nports 0, no root hub), pc's CONF that
never registered xHCI (386).

usage: tests/usb/usb.py iso workdir objtype [controller...]
   prints one line per check: ok|fail <name>
"""
import os, re, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from vm import VM

CTLRS = {	# QEMU device: what #u/usb/ctl names the root hub's controller
	'xhci': ('qemu-xhci', 'xhci'),
	'ehci': ('usb-ehci', 'ehci'),
	'uhci': ('piix3-usb-uhci', 'uhci'),
	'ohci': ('pci-ohci', 'ohci'),
}
MARK = 'USB-STICK-MARKER-9front'

iso, work, objtype = sys.argv[1:4]
which = sys.argv[4:] or list(CTLRS)
os.makedirs(work, exist_ok=True)
stick = os.path.join(work, 'stick.img')
with open(stick, 'wb') as f:	# 64 MB, the marker in sector 0
	f.write((MARK + '\n').encode())
	f.truncate(64 << 20)

def result(ok, name):
	print('%s %s' % ('ok' if ok else 'fail', name), flush=True)
	return ok

for c in which:
	dev, name = CTLRS[c]
	vm = VM(iso, os.path.join(work, c), objtype, ['-device', '%s,id=usb' % dev,
		'-drive', 'if=none,id=stick,file=%s,format=raw' % stick,
		'-device', 'usb-storage,bus=usb.0,drive=stick'])
	try:
		if not result(vm.boot(), '%s: boot to rio, serial rc' % c):
			continue
		ctl = vm.serial("cat '#u/usb/ctl'", 4)
		result('roothub' in ctl and name in ctl, '%s: root hub' % c)
		result('storage' in ctl, '%s: stick enumerated' % c)
		sd = vm.serial('ls -d /dev/sdU*', 4)
		result(re.search(r'^/dev/sdU\w+$', sd, re.M) is not None, '%s: /dev/sdU* (nusb/disk)' % c)
		data = vm.serial('for(d in /dev/sdU*) dd -if $d/data -bs 512 -count 1 >[2]/dev/null | sed 1q', 6)
		result(MARK in data, '%s: read sector 0' % c)
	finally:
		vm.close()
