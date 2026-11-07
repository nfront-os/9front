// SPDX-License-Identifier: MIT

//go:build plan9

// gocount: a counter in a window of ui(4), from Go (package ui). Prints
// "ready srv" once its scene is there and an event a line; -n n: ends
// after n clicks, printing "count n" and the label's properties.
// -d tcp!host!port: no new window, the files of a ui exported there
// (exportfs: aux/listen1 ... exportfs -r /mnt/ui), the package's own 9P.
package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"strings"

	"ui"
)

func main() {
	max := flag.Int("n", 0, "end after n clicks")
	dial := flag.String("d", "", "a ui's files exported at tcp!host!port")
	flag.Parse()
	log.SetFlags(0)
	log.SetPrefix("gocount: ")
	var u *ui.Ui
	var err error
	if *dial != "" {
		f := strings.Split(*dial, "!")
		if len(f) != 3 {
			log.Fatalf("-d %s: not tcp!host!port", *dial)
		}
		c, err := net.Dial(f[0], net.JoinHostPort(f[1], f[2]))
		if err != nil {
			log.Fatal(err)
		}
		u, err = ui.NewUi(c)
		if err != nil {
			log.Fatal(err)
		}
		u.Srv = *dial
	} else if u, err = ui.New("gocount"); err != nil {
		log.Fatal(err)
	}
	if err := u.Ctl("clear\nnew l label\nset l text count 0\nnew b button\nset b text add one\n"); err != nil {
		log.Fatal(err)
	}
	fmt.Println("ready", u.Srv)
	n := 0
	for {
		e, err := u.Event()
		if err != nil || e.Kind == "close" {
			break
		}
		fmt.Println("event", e.Kind, e.ID, e.Arg)
		if e.Kind == "click" && e.ID == "b" {
			n++
			if err := u.Set("l", "text", fmt.Sprint("count ", n)); err != nil {
				log.Fatal(err)
			}
			if n == *max {
				break
			}
		}
	}
	t, _ := u.ReadFile("nodes/l/props")
	fmt.Printf("count %d\n%s", n, t)
	u.Close()
	os.Exit(0)
}
