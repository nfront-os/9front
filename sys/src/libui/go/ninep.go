// SPDX-License-Identifier: MIT

package ui

// A client of 9P2000 (intro(5)): what ui's files need, over any
// connection (a /srv file, a network one); several requests at once (a
// read of event waits while ctl is written).

import (
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"sync"
)

const (
	tversion = 100
	tattach  = 104
	rerror   = 107
	tflush   = 108
	twalk    = 110
	topen    = 112
	tread    = 116
	twrite   = 118
	tclunk   = 120

	notag = 0xFFFF
	nofid = 0xFFFFFFFF

	OREAD  = 0
	OWRITE = 1
	ORDWR  = 2

	msize = 8192 + 24
	iohdr = 24
)

type conn struct {
	rw    io.ReadWriteCloser
	wmu   sync.Mutex
	mu    sync.Mutex
	tags  map[uint16]chan []byte
	next  uint16
	fid   uint32
	msize uint32
	err   error
}

func newConn(rw io.ReadWriteCloser) (*conn, error) {
	c := &conn{rw: rw, tags: map[uint16]chan []byte{}, msize: msize}
	// Tversion before the reader: its reply is the first message
	t := msg(tversion, notag).u32(msize).str("9P2000")
	if err := c.send(t); err != nil {
		return nil, err
	}
	r, err := c.recv()
	if err != nil {
		return nil, err
	}
	if r[4] != tversion+1 {
		return nil, errors.New("9P: no version")
	}
	d := dec{b: r[7:]}
	c.msize = min(d.u32(), msize)
	if v := d.str(); v != "9P2000" {
		return nil, fmt.Errorf("9P: version %q", v)
	}
	go c.reader()
	return c, nil
}

// a message being made: size, type, tag, then its fields
type enc []byte

func msg(typ byte, tag uint16) enc {
	b := enc{0, 0, 0, 0, typ}
	return b.u16(tag)
}

func (b enc) u8(v byte) enc    { return append(b, v) }
func (b enc) u16(v uint16) enc { return binary.LittleEndian.AppendUint16(b, v) }
func (b enc) u32(v uint32) enc { return binary.LittleEndian.AppendUint32(b, v) }
func (b enc) u64(v uint64) enc { return binary.LittleEndian.AppendUint64(b, v) }
func (b enc) str(s string) enc { return append(b.u16(uint16(len(s))), s...) }

type dec struct{ b []byte }

func (d *dec) u16() uint16 { v := binary.LittleEndian.Uint16(d.b); d.b = d.b[2:]; return v }
func (d *dec) u32() uint32 { v := binary.LittleEndian.Uint32(d.b); d.b = d.b[4:]; return v }
func (d *dec) str() string {
	n := int(d.u16())
	s := string(d.b[:n])
	d.b = d.b[n:]
	return s
}

func (c *conn) send(t enc) error {
	binary.LittleEndian.PutUint32(t, uint32(len(t)))
	c.wmu.Lock()
	defer c.wmu.Unlock()
	_, err := c.rw.Write(t)
	return err
}

func (c *conn) recv() ([]byte, error) {
	var sz [4]byte
	if _, err := io.ReadFull(c.rw, sz[:]); err != nil {
		return nil, err
	}
	n := binary.LittleEndian.Uint32(sz[:])
	if n < 7 || n > msize {
		return nil, fmt.Errorf("9P: message of %d bytes", n)
	}
	b := make([]byte, n)
	copy(b, sz[:])
	if _, err := io.ReadFull(c.rw, b[4:]); err != nil {
		return nil, err
	}
	return b, nil
}

// the replies to whom asked: by tag
func (c *conn) reader() {
	for {
		r, err := c.recv()
		c.mu.Lock()
		if err != nil {
			c.err = err
			for t, ch := range c.tags {
				close(ch)
				delete(c.tags, t)
			}
			c.mu.Unlock()
			return
		}
		tag := binary.LittleEndian.Uint16(r[5:])
		ch := c.tags[tag]
		delete(c.tags, tag)
		c.mu.Unlock()
		if ch != nil {
			ch <- r
		}
	}
}

func (c *conn) newfid() uint32 {
	c.mu.Lock()
	defer c.mu.Unlock()
	c.fid++
	return c.fid
}

// a request and its reply (Rerror: the error)
func (c *conn) rpc(typ byte, fields func(enc) enc) (*dec, error) {
	ch := make(chan []byte, 1)
	c.mu.Lock()
	if c.err != nil {
		c.mu.Unlock()
		return nil, c.err
	}
	for c.tags[c.next] != nil || c.next == notag {
		c.next++
	}
	tag := c.next
	c.next++
	c.tags[tag] = ch
	c.mu.Unlock()
	if err := c.send(fields(msg(typ, tag))); err != nil {
		return nil, err
	}
	r, ok := <-ch
	if !ok {
		return nil, fmt.Errorf("9P: %v", c.err)
	}
	d := &dec{b: r[7:]}
	switch r[4] {
	case rerror:
		return nil, errors.New(d.str())
	case typ + 1:
		return d, nil
	}
	return nil, fmt.Errorf("9P: reply %d to %d", r[4], typ)
}

func (c *conn) attach(uname string) (uint32, error) {
	fid := c.newfid()
	_, err := c.rpc(tattach, func(b enc) enc { return b.u32(fid).u32(nofid).str(uname).str("") })
	return fid, err
}

// a file of the tree by its path's names, opened
type file struct {
	c   *conn
	fid uint32
	off uint64
}

func (c *conn) open(root uint32, names []string, mode byte) (*file, error) {
	fid := c.newfid()
	d, err := c.rpc(twalk, func(b enc) enc {
		b = b.u32(root).u32(fid).u16(uint16(len(names)))
		for _, n := range names {
			b = b.str(n)
		}
		return b
	})
	if err != nil {
		return nil, err
	}
	if int(d.u16()) != len(names) {
		return nil, fmt.Errorf("%v: file does not exist", names)
	}
	f := &file{c: c, fid: fid}
	if _, err := c.rpc(topen, func(b enc) enc { return b.u32(fid).u8(mode) }); err != nil {
		f.Close()
		return nil, err
	}
	return f, nil
}

func (f *file) Read(p []byte) (int, error) {
	n := min(len(p), int(f.c.msize-iohdr))
	d, err := f.c.rpc(tread, func(b enc) enc { return b.u32(f.fid).u64(f.off).u32(uint32(n)) })
	if err != nil {
		return 0, err
	}
	m := int(d.u32())
	if m == 0 {
		return 0, io.EOF
	}
	copy(p, d.b[:m])
	f.off += uint64(m)
	return m, nil
}

// all of p in one message: a write of ctl is a transaction
func (f *file) Write(p []byte) (int, error) {
	if len(p) > int(f.c.msize-iohdr) {
		return 0, errors.New("write too long for one message")
	}
	d, err := f.c.rpc(twrite, func(b enc) enc { return append(b.u32(f.fid).u64(f.off).u32(uint32(len(p))), p...) })
	if err != nil {
		return 0, err
	}
	n := int(d.u32())
	f.off += uint64(n)
	return n, nil
}

func (f *file) Close() error {
	_, err := f.c.rpc(tclunk, func(b enc) enc { return b.u32(f.fid) })
	return err
}
