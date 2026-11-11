// SPDX-License-Identifier: MIT

package ui

import (
	"bufio"
	"encoding/binary"
	"io"
	"net"
	"strings"
	"testing"
)

func TestQuote(t *testing.T) {
	for in, want := range map[string]string{
		"hello world": "hello world",
		" pad ":       "' pad '",
		"it's":        "it's",
		"'q":          "'''q'",
		"":            "''",
		"a\nb":        "'a\\nb'",
		" a\\b":       "' a\\\\b'",
	} {
		if got := Quote(in); got != want {
			t.Errorf("Quote(%q) = %q, want %q", in, got, want)
		}
	}
}

// a ui of a test's: ctl and event, 9P on a pipe (the package's own 9P, as
// over a network)
func fake(t *testing.T, s net.Conn, wrote chan<- string) {
	r := bufio.NewReader(s)
	files := map[uint32]string{}
	var evq []string
	evq = append(evq, "click b\n", "text t hello world\n")
	for {
		var sz [4]byte
		if _, err := io.ReadFull(r, sz[:]); err != nil {
			return
		}
		b := make([]byte, binary.LittleEndian.Uint32(sz[:]))
		copy(b, sz[:])
		io.ReadFull(r, b[4:])
		typ, tag := b[4], binary.LittleEndian.Uint16(b[5:])
		d := dec{b: b[7:]}
		reply := msg(typ+1, tag)
		switch typ {
		case tversion:
			reply = reply.u32(d.u32()).str("9P2000")
		case tattach:
			files[d.u32()] = ""
			reply = append(reply, make([]byte, 13)...)
		case twalk:
			fid, nfid, n := d.u32(), d.u32(), d.u16()
			name := files[fid]
			for i := 0; i < int(n); i++ {
				name += "/" + d.str()
			}
			files[nfid] = name
			reply = reply.u16(n)
			for i := 0; i < int(n); i++ {
				reply = append(reply, make([]byte, 13)...)
			}
		case topen:
			reply = append(reply, make([]byte, 13+4)...)
		case tread:
			fid := d.u32()
			if files[fid] == "/event" && len(evq) > 0 {
				reply = reply.u32(uint32(len(evq[0]))).append(evq[0])
				evq = evq[1:]
			} else if files[fid] == "/nodes/l/props" {
				if binary.LittleEndian.Uint64(d.b) == 0 { // offset 0
					reply = reply.u32(12).append("text count 1")
				} else {
					reply = reply.u32(0)
				}
			} else {
				reply = reply.u32(0)
			}
		case twrite:
			d.u32()
			d.u32()
			d.u32()
			n := d.u32()
			wrote <- string(d.b[:n])
			reply = reply.u32(n)
		case tclunk:
		}
		binary.LittleEndian.PutUint32(reply, uint32(len(reply)))
		s.Write(reply)
	}
}

func (b enc) append(s string) enc { return append(b, s...) }

func TestNinep(t *testing.T) {
	c, s := net.Pipe()
	wrote := make(chan string, 10)
	go fake(t, s, wrote)
	u, err := NewUi(c)
	if err != nil {
		t.Fatal(err)
	}
	go func() {
		if err := u.Set("l", "text", " count 1 "); err != nil {
			t.Error(err)
		}
	}()
	if w := <-wrote; w != "set l text ' count 1 '\n" {
		t.Errorf("ctl got %q", w)
	}
	e, err := u.Event()
	if err != nil || e != (Event{"click", "b", ""}) {
		t.Errorf("event %v %v", e, err)
	}
	e, _ = u.Event()
	if e != (Event{"text", "t", "hello world"}) {
		t.Errorf("event %v", e)
	}
	p, err := u.ReadFile("nodes/l/props")
	if err != nil || strings.TrimSpace(string(p)) != "text count 1" {
		t.Errorf("props %q %v", p, err)
	}
	u.Close()
}
