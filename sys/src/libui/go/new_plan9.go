// SPDX-License-Identifier: MIT

package ui

import (
	"fmt"
	"io"
	"os"
	"os/exec"
	"syscall"
	"time"
)

// the files mounted: the kernel's 9P, shared with whoever mounts the same
type mounted struct{ mtpt string }

func (m *mounted) open(name string, mode int) (io.ReadWriteCloser, error) {
	return os.OpenFile(m.mtpt+"/"+name, mode, 0)
}

func (m *mounted) close() error { return syscall.Unmount("", m.mtpt) }

// Open is the ui posted as /srv/srv, mounted on /mnt/srv.
func Open(srv string) (*Ui, error) {
	fd, err := syscall.Open("/srv/"+srv, syscall.O_RDWR)
	if err != nil {
		return nil, fmt.Errorf("/srv/%s: %v", srv, err)
	}
	// a mount that works closes fd (bind(2)): not an os.File, that would
	// close it again (and whatever has its number by then)
	mtpt := "/mnt/" + srv
	if err := syscall.Mount(fd, -1, mtpt, syscall.MREPL, ""); err != nil {
		syscall.Close(fd)
		return nil, fmt.Errorf("mount %s: %v", mtpt, err)
	}
	u, err := newUi(&mounted{mtpt})
	if err != nil {
		return nil, err
	}
	u.Srv = srv
	return u, nil
}

// New starts ui in a new window of rio or wsys (window(1)) and is its files.
func New(title string) (*Ui, error) {
	user := os.Getenv("user")
	if user == "" {
		user = "none"
	}
	srv := fmt.Sprintf("ui.%s.%d.0", user, os.Getpid())
	// window returns once the window is there; ui posts its files soon after
	if err := exec.Command("/bin/window", "/bin/ui -s "+srv).Run(); err != nil {
		return nil, fmt.Errorf("window: %v", err)
	}
	for t := time.Now(); ; time.Sleep(100 * time.Millisecond) {
		if _, err := os.Stat("/srv/" + srv); err == nil {
			break
		}
		if time.Since(t) > 10*time.Second {
			return nil, fmt.Errorf("no /srv/%s from a new ui", srv)
		}
	}
	u, err := Open(srv)
	if err != nil {
		return nil, err
	}
	if err := u.Ctl("title %s\n", Quote(title)); err != nil {
		u.Close()
		return nil, err
	}
	return u, nil
}
