#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
#
# vncshot.py SOCKET OUT.png [x,y ...]: one frame from QEMU's VNC server
# (-vnc unix:SOCKET) as PNG, and the colours at the points given as
# "x,y rrggbb" lines. For a guest whose display is GL (virtio-gpu-gl with
# egl-headless): QMP's screendump has no surface there, VNC gets what
# egl-headless reads back, i.e. what the host really shows. Raw RFB 3.8,
# no authentication, raw encoding only.

import socket
import struct
import sys
import zlib


def recvn(s, n):
	b = b''
	while len(b) < n:
		c = s.recv(n - len(b))
		if not c:
			sys.exit('vncshot: connection closed')
		b += c
	return b


def shot(path):
	s = socket.socket(socket.AF_UNIX)
	s.settimeout(30)
	s.connect(path)
	ver = recvn(s, 12)
	if not ver.startswith(b'RFB '):
		sys.exit('vncshot: not an RFB server')
	s.sendall(b'RFB 003.008\n')
	n = recvn(s, 1)[0]
	if n == 0:
		sys.exit('vncshot: refused: %r' % recvn(s, struct.unpack('>I', recvn(s, 4))[0]))
	if 1 not in recvn(s, n):
		sys.exit('vncshot: needs a password')
	s.sendall(b'\x01')
	if struct.unpack('>I', recvn(s, 4))[0] != 0:
		sys.exit('vncshot: security failed')
	s.sendall(b'\x01')		# shared
	w, h = struct.unpack('>HH', recvn(s, 4))
	recvn(s, 16)			# the server's pixel format; ours below
	recvn(s, struct.unpack('>I', recvn(s, 4))[0])
	# 32 bits, little endian, true colour, r<<16 g<<8 b
	pf = struct.pack('>BBBBHHHBBB3x', 32, 24, 0, 1, 255, 255, 255, 16, 8, 0)
	s.sendall(b'\x00\x00\x00\x00' + pf)
	s.sendall(struct.pack('>BxHi', 2, 1, 0))		# raw only
	s.sendall(struct.pack('>BBHHHH', 3, 0, 0, 0, w, h))
	fb = bytearray(w * h * 4)
	got = 0
	while got < w * h:
		t = recvn(s, 1)[0]
		if t == 2:		# bell
			continue
		if t == 3:		# cut text
			recvn(s, 3)
			recvn(s, struct.unpack('>I', recvn(s, 4))[0])
			continue
		if t == 1:		# colour map
			recvn(s, 3)
			recvn(s, 6 * struct.unpack('>H', recvn(s, 2))[0])
			continue
		if t != 0:
			sys.exit('vncshot: message %d' % t)
		recvn(s, 1)
		for _ in range(struct.unpack('>H', recvn(s, 2))[0]):
			x, y, rw, rh, enc = struct.unpack('>HHHHi', recvn(s, 12))
			if enc == -223:		# desktop size: new w, h
				w, h = rw, rh
				fb = bytearray(w * h * 4)
				s.sendall(struct.pack('>BBHHHH', 3, 0, 0, 0, w, h))
				continue
			if enc != 0:
				sys.exit('vncshot: encoding %d' % enc)
			px = recvn(s, rw * rh * 4)
			for r in range(rh):
				o = ((y + r) * w + x) * 4
				fb[o:o + rw * 4] = px[r * rw * 4:(r + 1) * rw * 4]
			got += rw * rh
	s.close()
	return w, h, fb


def png(out, w, h, fb):
	raw = bytearray()
	for y in range(h):
		raw.append(0)
		row = fb[y * w * 4:(y + 1) * w * 4]
		for x in range(w):
			b, g, r = row[x * 4], row[x * 4 + 1], row[x * 4 + 2]
			raw += bytes((r, g, b))

	def chunk(t, b):
		return struct.pack('>I', len(b)) + t + b + struct.pack('>I', zlib.crc32(t + b))
	open(out, 'wb').write(b'\x89PNG\r\n\x1a\n'
		+ chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0))
		+ chunk(b'IDAT', zlib.compress(bytes(raw))) + chunk(b'IEND', b''))


def main():
	if len(sys.argv) < 3:
		sys.exit('usage: vncshot.py SOCKET OUT.png [x,y ...]')
	w, h, fb = shot(sys.argv[1])
	png(sys.argv[2], w, h, fb)
	print('screen %dx%d' % (w, h))
	for p in sys.argv[3:]:
		x, y = map(int, p.split(','))
		o = (y * w + x) * 4
		print('%d,%d %02x%02x%02x' % (x, y, fb[o + 2], fb[o + 1], fb[o]))


if __name__ == '__main__':
	main()
