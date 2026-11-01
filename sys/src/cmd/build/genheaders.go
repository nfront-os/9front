/*
Copyright 2018 Harvey OS Team
Copyright 2026 The build tool port for 9front

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"path/filepath"
	"regexp"
	"strings"
)

// errstr generates errstr.h in the kernel build directory, the 9front
// equivalent of `rc ../port/mkerrstr`.  It converts the extern
// declarations of ../port/error.h into definitions, filling in the error
// strings from the trailing /* message */ comments.
func errstr(b *build) {
	src := filepath.Join(b.path, "../port/error.h")
	data, err := ioutil.ReadFile(src)
	if err != nil {
		log.Printf("kernel build %s: %v; errstr.h left stale", b.Name, err)
		return
	}

	commentRe := regexp.MustCompile(`;\s*/\*\s*(.*?)\s*\*/.*$`)
	var out []byte
	for _, line := range strings.Split(string(data), "\n") {
		body := line
		if strings.HasPrefix(body, "extern ") {
			body = body[len("extern "):]
			if m := commentRe.FindStringSubmatch(body); m != nil {
				body = body[:strings.Index(body, ";")] + ` = "` + m[1] + `";`
			}
		}
		out = append(out, body...)
		out = append(out, '\n')
	}

	log.Printf("Writing errstr.h in %s", b.path)
	fail(writefile(filepath.Join(b.path, "errstr.h"), out))
}

// systab generates ../port/systab.h from the syscall header, the 9front
// equivalent of `rc ../port/mksystab`.  The syscall table has holes where
// no number is assigned; those entries stay nil and are reported as
// system call errors by sysproc.c.
func systab(b *build) {
	src := b.Kernel.Systab
	if src == "" {
		src = fromRoot("/sys/src/libc/9syscall/sys.h")
	}
	data, err := ioutil.ReadFile(src)
	if err != nil {
		log.Printf("kernel build %s: %v; systab.h left stale", b.Name, err)
		return
	}

	var names []string
	for _, line := range strings.Split(string(data), "\n") {
		f := strings.Fields(line)
		if len(f) >= 2 && f[0] == "#define" {
			names = append(names, f[1])
		}
	}

	var out []byte
	write := func(format string, s ...interface{}) {
		out = append(out, fmt.Sprintf(format, s...)...)
	}

	write("#include \"%s\"\n\n", src)
	write("typedef uintptr Syscall(uchar*);\n\n")
	write("enum {\n")
	for _, n := range names {
		write("\tSYS_%s = %s,\n", strings.ToLower(n), strings.ToUpper(n))
	}
	write("};\n\n")
	for _, n := range names {
		write("Syscall %s;\n", sysname(n))
	}
	write("Syscall sysdeath;\n\n")

	write("Syscall *systab[]={\n")
	for _, n := range names {
		lower := strings.ToLower(n)
		fn := sysname(n)
		l := fmt.Sprintf("\t[SYS_%s]\t%s,\n", lower, fn)
		switch {
		case strings.Contains(l, "syssysr1"):
			l = "sysr1\n"
		case strings.Contains(l, "sys_x"):
			l = "sysdeath\n"
		}
		if strings.HasPrefix(l, "\t[") {
			// pad short designators to align columns, like mksystab does
			cl := strings.Index(l, "]")
			if cl-3 < 8 {
				l = l[:cl] + "\t" + l[cl:]
			}
		}
		write("%s", l)
	}
	write("};\n\n")

	write("char *sysctab[]={\n")
	for _, n := range names {
		l := fmt.Sprintf("\t[SYS_%s]\t\"%s\",\n", strings.ToLower(n), sysctabname(n))
		write("%s", l)
	}
	write("};\n\n")

	write("int nsyscall = (sizeof systab/sizeof systab[0]);\n")

	dst := filepath.Join(b.path, "../port/systab.h")
	log.Printf("Writing %s", dst)
	fail(writefile(dst, out))
}

// sysctabname is mksystab's name of a system call in sysctab (what
// ratrace, ps and /proc/n/status show): the first letter as it is, the
// rest lowercased ("RFORK" is "Rfork", "_ERRSTR" "_errstr"), "Sysr1"
// is "Running", "Rendezvous" "Rendez", a trailing "_" goes.
func sysctabname(n string) string {
	if n[0] >= 'A' && n[0] <= 'Z' {
		n = n[:1] + strings.ToLower(n[1:])
	} else {
		n = strings.ToLower(n)
	}
	switch n {
	case "Sysr1":
		n = "Running"
	case "Rendezvous":
		n = "Rendez"
	}
	return strings.TrimSuffix(n, "_")
}

// sysname is the mksystab transform of a syscall macro name to the
// corresponding kernel function (SYS<name>, lowercased, with the
// SYSSYSR1 special case).
func sysname(n string) string {
	n = "SYS" + n
	if n == "SYSSYSR1" {
		n = "SYSR1"
	}
	return strings.ToLower(n)
}