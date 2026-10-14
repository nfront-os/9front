#!/usr/bin/env python3
"""
rio with mouse and keyboard as a user works it: select text in the first
window with the left button, type; New from the button 3 menu, sweep
the window with button 3, type into it; select there and type again.
Each typed line writes a marker to the serial line ('#t/eia0'), where
it is checked. A click that rio's frselect never saw end (readmouse()
received into the wrong place) left rio hung here: no keyboard, New
stuck in the sweep cursor.

usage: tests/rio/rio.py iso workdir objtype
   prints one line per check: ok|fail <name>
"""
import os, sys, time
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from vm import VM

iso, work, objtype = sys.argv[1:4]
vm = VM(iso, work, objtype)
def result(ok, name):
	print('%s %s' % ('ok' if ok else 'fail', name), flush=True)
	return ok

def typed(marker):
	vm.type("echo %s >'#t/eia0'\n" % marker)
	return marker in vm.read(8)

try:
	if not result(vm.boot(), 'boot to rio, serial rc'):
		sys.exit(0)
	vm.move(-3000, -3000, step=100)	# top left corner (clamped)
	vm.move(300, 250)		# into the first window
	vm.button('left', True)
	vm.move(120, 0, step=2, pause=0.01)	# select by dragging
	vm.button('left', False)
	time.sleep(0.5)
	result(typed('rio-after-select'), 'type after selecting in a window')

	vm.move(100, 300)		# below the window: rio's background
	vm.button('right', True)
	time.sleep(0.5)
	vm.shot('menu')
	vm.button('right', False)	# New
	time.sleep(0.8)
	vm.button('right', True)
	vm.move(200, 200, step=1, pause=0.005)	# sweep
	vm.button('right', False)
	time.sleep(3)
	vm.shot('new')
	result(typed('rio-new-window'), 'New: sweep a window, type into it')

	vm.button('left', True)
	vm.move(-40, 0, step=1, pause=0.01)
	vm.button('left', False)
	time.sleep(0.5)
	result(typed('rio-select-new'), 'type after selecting in the new window')

	ps = vm.serial('ps | grep -c rio', 4)
	result(any(l.strip().isdigit() and int(l.strip()) > 0 for l in ps.split('\n')), 'rio still running')
	vm.shot('final')
finally:
	vm.close()
