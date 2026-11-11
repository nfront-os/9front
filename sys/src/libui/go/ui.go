// SPDX-License-Identifier: MIT

// Package ui is a client of ui(4) (a window of widgets described through
// files) in Go, without cgo. On Plan 9 ui's /srv file is mounted (on
// /mnt/srvname: mntgen's, nobody else's), as any program of Plan 9's does
// it: others mount the same and share the session. Over a connection without
// a kernel's mount (a network one to exportfs, another system) the package
// speaks 9P itself (NewUi).
//
//	u, err := ui.New("count")
//	u.Ctl("new b button\nset b text add one\nnew l label\nset l text 0\n")
//	for n := 1; ; n++ {
//		e, err := u.Event()
//		if err != nil || e.Kind == "close" {
//			break
//		}
//		if e.Kind == "click" && e.ID == "b" {
//			u.Set("l", "text", fmt.Sprint(n))
//		}
//	}
package ui

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strings"
)

// Ui is a ui's files: ctl (transactions), event (what the user did).
type Ui struct {
	Srv   string
	fs    fsys
	ctl   io.WriteCloser
	ev    io.ReadCloser
	event *bufio.Reader
}

// where ui's files are: mounted, or a 9P conversation of the package's
type fsys interface {
	open(name string, mode int) (io.ReadWriteCloser, error)
	close() error
}

type ninep struct {
	c    *conn
	root uint32
}

func (n *ninep) open(name string, mode int) (io.ReadWriteCloser, error) {
	return n.c.open(n.root, strings.Split(name, "/"), byte(mode))
}

func (n *ninep) close() error { return n.c.rw.Close() }

func newUi(fs fsys) (*Ui, error) {
	u := &Ui{fs: fs}
	ctl, err := fs.open("ctl", OWRITE)
	if err != nil {
		fs.close()
		return nil, err
	}
	ev, err := fs.open("event", OREAD)
	if err != nil {
		ctl.Close()
		fs.close()
		return nil, err
	}
	u.ctl, u.ev, u.event = ctl, ev, bufio.NewReader(ev)
	return u, nil
}

// Event is a line of event: click id, text id text, enter id text,
// check id 0|1, slide id value, close.
type Event struct {
	Kind, ID, Arg string
}

// NewUi is ui's files over rw, a connection of the program's own (9P of
// the package's: not a /srv file others mount as well).
func NewUi(rw io.ReadWriteCloser) (*Ui, error) {
	c, err := newConn(rw)
	if err != nil {
		rw.Close()
		return nil, err
	}
	user := os.Getenv("user")
	if user == "" {
		user = "none"
	}
	root, err := c.attach(user)
	if err != nil {
		rw.Close()
		return nil, err
	}
	return newUi(&ninep{c, root})
}

// Ctl writes one transaction: its lines all or none (an error names the line).
func (u *Ui) Ctl(format string, a ...any) error {
	s := format
	if len(a) > 0 {
		s = fmt.Sprintf(format, a...)
	}
	_, err := u.ctl.Write([]byte(s))
	return err
}

// Set sets a property, its value quoted as rc does where it needs it.
func (u *Ui) Set(id, key, value string) error {
	return u.Ctl("set %s %s %s\n", id, key, Quote(value))
}

// Quote quotes as rc (and ui) where a value needs it: blanks at its ends, a
// quote at its start, a newline. In the quotes \n is a new line, \\ a
// backslash (a textarea's lines stay on one line).
func Quote(v string) string {
	if v != "" && strings.TrimSpace(v[:1]) != "" && strings.TrimSpace(v[len(v)-1:]) != "" &&
		v[0] != '\'' && !strings.Contains(v, "\n") {
		return v
	}
	r := strings.NewReplacer("'", "''", "\\", "\\\\", "\n", "\\n")
	return "'" + r.Replace(v) + "'"
}

// Event waits for the next event; an error when ui is gone.
func (u *Ui) Event() (Event, error) {
	l, err := u.event.ReadString('\n')
	if err != nil {
		return Event{}, err
	}
	f := strings.SplitN(strings.TrimSuffix(l, "\n"), " ", 3)
	e := Event{Kind: f[0]}
	if len(f) > 1 {
		e.ID = f[1]
	}
	if len(f) > 2 {
		e.Arg = f[2]
	}
	return e, nil
}

// ReadFile is a file of ui's: tree, ctl, nodes/id/props, ...
func (u *Ui) ReadFile(name string) ([]byte, error) {
	f, err := u.fs.open(name, OREAD)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	return io.ReadAll(f)
}

// Close lets ui's files go; the window stays until ui ends.
func (u *Ui) Close() error {
	u.ctl.Close()
	u.ev.Close()
	return u.fs.close()
}
