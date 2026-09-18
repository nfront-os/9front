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
	"regexp"
	"sort"
	"strings"
	"time"
)

// confcode generates <Name>.c in the working directory, the 9front
// equivalent of what
//
//	rc ../port/mkdevc $CONF
//
// and, for the boot files, what mkrootall(8) together with mkrootc(8)
// produce.  The output is one self-contained translation unit: devtab,
// links, bootlinks (with the ramfile data inlined), the knownarch,
// adifc/sdifc/physuart/vgadev/vgacur tables, the ip protocol table, the
// "port" section code, configfile[] and kerndate.
func confcode(b *build) []byte {
	kern := b.Kernel
	var out []byte
	write := func(format string, s ...interface{}) {
		out = append(out, fmt.Sprintf(format, s...)...)
	}

	write("#include \"u.h\"\n")
	write("#include \"../port/lib.h\"\n")
	write("#include \"mem.h\"\n")
	write("#include \"dat.h\"\n")
	write("#include \"fns.h\"\n")
	write("#include \"io.h\"\n")
	write("#include \"../port/error.h\"\n")
	write("\n")

	// Parse the CONF sections, following ../port/mkdevc(8) exactly.
	var dev, link, ip, arch, adifc, sdifc, physuart, vgadev, vgacur, dtracyprov []string
	var narch, nvgacur, ndtracyprov int
	devad, devsd, devuart, devvga, devdtracy, ramdisk := false, false, false, false, false, false
	i8250 := 0
	i8237dma := 0

	for _, line := range kern.Config.Dev {
		fields := strings.Fields(line)
		if len(fields) == 0 {
			continue
		}
		name := fields[0]
		dev = append(dev, name)
		if name == "ad" {
			devad = true
		} else if name == "sd" {
			devsd = true
		} else if name == "uart" {
			devuart = true
		} else if name == "vga" {
			devvga = true
		} else if name == "dtracy" {
			devdtracy = true
		}
		for _, f := range fields {
			if f == "dma" {
				i8237dma++
			}
		}
	}
	for _, line := range kern.Config.Ip {
		fields := strings.Fields(line)
		if len(fields) != 0 {
			ip = append(ip, fields[0])
		}
	}
	for _, line := range kern.Config.Link {
		fields := strings.Fields(line)
		if len(fields) != 0 {
			link = append(link, fields[0])
		}
	}
	for _, line := range kern.Config.Misc {
		fields := strings.Fields(line)
		if len(fields) == 0 {
			continue
		}
		name := fields[0]
		switch {
		case strings.HasPrefix(name, "arch"):
			arch = append(arch, name)
			narch++
		case strings.HasPrefix(name, "ad"):
			adifc = append(adifc, name)
		case strings.HasPrefix(name, "sd"):
			if name == "sdram" {
				ramdisk = true
			} else {
				sdifc = append(sdifc, name)
			}
		case strings.HasPrefix(name, "uart"):
			physuart = append(physuart, name[len("uart"):]+"physuart")
		case strings.HasPrefix(name, "vga"):
			// A line with only the name is a plain vga device.  A line with
			// flags adds it to the vgadev and/or vgacur tables: "+cur" means
			// both, "=cur" only vgacur.
			if len(fields) == 1 {
				vgadev = append(vgadev, name)
			} else {
				for _, f := range fields[1:] {
					if f == "+cur" {
						vgadev = append(vgadev, name)
					}
					if f == "+cur" || f == "=cur" {
						vgacur = append(vgacur, name)
						nvgacur++
					}
				}
			}
		case strings.HasPrefix(name, "dtracy"):
			dtracyprov = append(dtracyprov, name)
			ndtracyprov++
		}
	}

	// Dev *devtab[].
	for _, name := range dev {
		write("extern Dev %sdevtab;\n", name)
	}
	write("Dev* devtab[]={\n")
	for _, name := range dev {
		write("\t&%sdevtab,\n", name)
	}
	write("\tnil,\n};\n\n")

	// i8237 dma.
	alloc := "nil"
	if i8237dma > 0 {
		write("extern void _i8237alloc(void);\n")
		alloc = "_i8237alloc"
	}
	write("void (*i8237alloc)(void) = %s;\n", alloc)
	write("int i8237dma = %d;\n\n", i8237dma)

	// links, with bootlinks first.
	for _, name := range link {
		write("extern void %slink(void);\n", name)
	}
	write("void links(void){\n")
	write("\tbootlinks();\n")
	for _, name := range link {
		write("\t%slink();\n", name)
	}
	write("}\n\n")

	// bootlinks + ramfile data.
	write("%s", ramfiles(b))

	// knownarch.
	for _, name := range arch {
		write("extern PCArch %s;\n", name)
	}
	write("PCArch* knownarch[] = {\n")
	for _, name := range arch {
		write("\t&%s,\n", name)
	}
	write("\tnil,\n};\n\n")

	// ADifc* adifc[].
	if devad {
		write("#include \"../port/ad.h\"\n")
		for _, name := range adifc {
			write("extern ADifc %sifc;\n", name)
		}
		write("ADifc* adifc[] = {\n")
		for _, name := range adifc {
			write("\t&%sifc,\n", name)
		}
		write("\tnil,\n};\n\n")
	}

	// SDifc* sdifc[].
	if devsd {
		write("#include \"../port/sd.h\"\n")
		for _, name := range sdifc {
			write("extern SDifc %sifc;\n", name)
		}
		write("SDifc* sdifc[] = {\n")
		for _, name := range sdifc {
			write("\t&%sifc,\n", name)
		}
		write("\tnil,\n};\n\n")
	}
	if !ramdisk {
		write("void ramdiskinit(void)\n{\n}\n\n")
	}

	// PhysUart* physuart[].
	if devuart {
		for _, name := range physuart {
			write("extern PhysUart %s;\n", name)
			if strings.Contains(name, "i8250") {
				i8250++
			}
		}
		write("PhysUart* physuart[] = {\n")
		for _, name := range physuart {
			write("\t&%s,\n", name)
		}
		write("\tnil,\n};\n\n")
		if i8250 == 0 {
			write("void i8250console(void)\n{\n}\n\n")
		}
	}

	// VGAdev* vgadev[] and VGAcur* vgacur[].
	if devvga {
		write("#define\tImage\tIMAGE\n")
		write("#include <draw.h>\n")
		write("#include <memdraw.h>\n")
		write("#include <cursor.h>\n")
		write("#include \"screen.h\"\n")
		for _, name := range vgadev {
			write("extern VGAdev %sdev;\n", name)
		}
		write("VGAdev* vgadev[] = {\n")
		for _, name := range vgadev {
			write("\t&%sdev,\n", name)
		}
		write("\tnil,\n};\n\n")
		for _, name := range vgacur {
			write("extern VGAcur %scur;\n", name)
		}
		write("VGAcur* vgacur[] = {\n")
		for _, name := range vgacur {
			write("\t&%scur,\n", name)
		}
		write("\tnil,\n};\n\n")
	} else {
		write("void bootscreeninit(void)\n{\n}\n\n")
	}

	// void (*ipprotoinit[])(Fs*).
	if len(ip) > 0 {
		write("#include \"../ip/ip.h\"\n")
		for _, name := range ip {
			write("extern void %sinit(Fs*);\n", name)
		}
		write("void (*ipprotoinit[])(Fs*) = {\n")
		for _, name := range ip {
			write("\t%sinit,\n", name)
		}
		write("\tnil,\n};\n\n")
	}

	// "port" section: arbitrary C.
	for _, line := range kern.Config.Code {
		write("%s\n", line)
	}
	write("\n")

	// dtracy providers.
	if devdtracy {
		if ndtracyprov > 0 {
			write("#include <dtracy.h>\n")
			for _, name := range dtracyprov {
				write("extern DTProvider %sprov;\n", name)
			}
			write("DTProvider *dtproviders[] = {\n")
			for _, name := range dtracyprov {
				write("\t&%sprov,\n", name)
			}
			write("\tnil,\n};\n\n")
		}
	} else {
		write("void dtracytick(Ureg*)\n{\n}\n\n")
	}

	// conffile: the CONF file name shown by `cat /dev/config` (and the
	// raw contents as configfile[]).
	write("char* conffile = \"%s\";\n", kern.ConfFile)
	write("ulong kerndate = %d;\n\n", time.Now().Unix())

	var cf []byte
	if kern.ConfFile != "" {
		var err error
		cf, err = ioutil.ReadFile(kern.ConfFile)
		if err != nil {
			log.Printf("kernel build %s: cannot read CONF %s: %v; configfile left empty", b.Name, kern.ConfFile, err)
			cf = []byte{}
		}
	}
	write("uchar configfile[] = {\n")
	for i := 0; i < len(cf); i++ {
		if i%16 == 0 {
			write("\t")
		}
		write("0x%02x, ", cf[i])
		if i%16 == 15 {
			write("\n")
		}
	}
	write("0,\n};\n")

	return out
}

// ramfiles renders the bootlinks() function plus the C byte arrays with
// the raw contents of every Kernel.Ramfiles entry, the equivalent of what
// mkrootc(8) (for bootlinks) and mkrootall(8) (for the data) produce.
// A missing file produces an empty array and a warning so that a first
// bring-up can proceed without the whole boot image being present yet.
func ramfiles(b *build) string {
	kern := b.Kernel
	var out []byte
	write := func(format string, s ...interface{}) {
		out = append(out, fmt.Sprintf(format, s...)...)
	}

	names := make([]string, 0, len(kern.Ramfiles))
	for name := range kern.Ramfiles {
		names = append(names, name)
	}
	sort.Strings(names)

	// Sanitised C identifier per name.
	re := regexp.MustCompile("[^a-zA-Z0-9_]")
	cname := map[string]string{}
	uniq := map[string]bool{}
	for _, name := range names {
		c := re.ReplaceAllString(name, "_")
		for c != "ramfile" && uniq[c] {
			c = "_" + c
		}
		uniq[c] = true
		cname[name] = c
	}

	for _, name := range names {
		c := cname[name]
		var d []byte
		p := kern.Ramfiles[name]
		if p != "" {
			var err error
			d, err = ioutil.ReadFile(p)
			if err != nil {
				log.Printf("kernel build %s: cannot read ramfile %s (%s): %v; using empty array", b.Name, name, p, err)
				d = []byte{}
			}
		}
		write("static uchar %s_code[] = {\n", c)
		for i := 0; i < len(d); i++ {
			if i%8 == 0 {
				write("\t")
			}
			write("0x%02x, ", d[i])
			if i%8 == 7 {
				write("\n")
			}
		}
		write("\n};\n")
		write("static ulong %s_len = sizeof(%s_code);\n\n", c, c)
	}

	write("void bootlinks(void){\n")
	for _, name := range names {
		c := cname[name]
		write("\taddbootfile(\"%s\", %s_code, %s_len);\n", name, c, c)
	}
	write("}\n\n")

	return string(out)
}