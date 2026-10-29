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

/*
BUILDFILE FORMAT

A buildfile is a json object containing build objects. By convention, it's
broken out onto multiple lines indented by tabs, and the keys are TitleCased.

The environment variables PLAN9 (HARVEY is honoured as a fallback) and ARCH
are guaranteed to be set for the buildfile. PATH is modified so that
"$PLAN9/util" is included.

From ARCH the tool sets, unless they are in the environment already,
objtype (9front's name for it), O (the object letter), SUFFIX (of the
generated per-arch manifests, none for amd64), CONF (the kernel
configuration: pc, pc64, qemu), TOOLPREFIX (arm64: aarch64-linux-musl-)
and CPUS. PLAN9 defaults to the git checkout. Without an argument the
tool builds build.json of the current directory, else the tree's, which
builds everything for ARCH (build.$ARCH.json, scripts/gen/genbuild.py):

	CC=gcc ARCH=amd64 sys/src/cmd/build/build.exe

Any key that takes a path or array of paths as its value has absolute paths
re-rooted to the plan9 tree and variables in the string expanded once.

The shell commands in the "Pre" and "Post" steps must be in the subset of
syntax that's accepted by both POSIX sh and rc. Practically, this means
arguments with a "=" must be single-quoted, "test" must be called as
"test" (not "["), "if" statements may not have an "else" clause, "switch"
statements may not be used, "for" statements may only have one body command,
and redirection to a file descriptor cannont be used.

BUILD OBJECT

A build object has the following keys and types:

	Projects        []string
	Pre             []string
	Post            []string
	Cflags          []string
	Oflags          []string
	Include         []string
	SourceFiles     []string
	ObjectFiles     []string
	Libs            []string
	Env             []string
	SourceFilesCmd  []string
	Program         string
	Library         string
	Install         string
	Ape             bool
	IncludePath     []string
	Append          bool
	Shared          bool
	Objtag          string
	Clean           []string
	Kernel          kernel

These are the steps taken, in order:

	Env
	Include
	Projects
	Pre
	Kernel
	[compile] SourceFiles SourceFilesCmd Cflags Program
	[link] ObjectFiles Libs Oflags Library
	Install
	Post

"[compile]" and "[link]" are steps synthesized from the specified keys.

The meaning of the keys is as follows:

"Env" is an array of environment variables to be put in the environment of
every command run in a build. They are expanded once and only available for
use in other steps.

"Include" is an array of buildfiles to "merge" into the current one. This
is done before other projects are built.

"Projects" is an array of buildfiles to build in their entirety before
starting the current build.

"Pre" is an array of commands to run before starting the build.

"Kernel" is a kernel build object. See the kernel object section.

"Cflags" is an array of flags to pass to the C compiler. They are in addition
to the standard flags of

	-std=c11 -c -I /$ARCH/include -I /sys/include -I .

The standard include paths are re-rooted to the plan9 tree if not on a plan9
system.

"SourceFilesCmd" is an array of C files where each one should result in an
executable. If this key is provided, "SourceFiles" and "Program" are ignored.

"SourceFiles" is an array of C files that will ultimately produce a single
binary or library, named by "Program" or "Library" respectively.

"Ape" compiles the sources the way pcc does for the ANSI/POSIX
environment: the include path is /$ARCH/include/ape, /sys/include/ape
and . instead of /$ARCH/include, /sys/include and .

"IncludePath" replaces the standard include path (that of "Ape" too):
sources with headers of their own (musl's, in APE). The directories are
relative to the buildfile's directory; absolute ones are re-rooted as the
standard ones.

"Append" adds the objects to an existing "Library" archive, replacing
members of the same name, instead of creating the archive anew. It
builds one archive from several directories the way mk's ar does
(APE's libap).

"Shared" marks the manifest that starts such an archive (libap's gen):
other manifests "Append" to its "Library", so -D does not expect the
archive to hold this manifest's objects only.

"Objtag" names the objects x.<Objtag>.o instead of x.o. The kernel's
libraries (klib.json, "k") compile the userspace libraries' sources in
the same directory with other flags; with their own names both sets of
objects stay, and -D finds each up to date.

"Program" is the name of the final executable for the files specified by
"SourceFiles".

"Oflags" is an array of flags to pass to the linker. They are in addition
to the standard flags of

	-o $program $objfiles -L /$ARCH/lib.gcc $libs

The lib path is re-rooted to the plan9 tree if not on a plan9 system.

"ObjectFiles" is an array of strings specifying object files to be linked
into the final program specified by "Program". Any object files produced
by the preceeding "[compile]" step are automatically added to this before
beginning the "[link]" step.

"Libs" is a an array of library arguments to pass to the linker.

"Library" is the name of the archive resulting from bundling "SourceFiles"
into a library. The resulting archive has 'ranlib' run on it automatically.

"Install" is a directory where the result of the "[link]" step is moved
to. If it does not exist, it is created.

"Post" is an array of commands to run last during the build.

"Clean" is an array of files (globs, relative to the buildfile's
directory) that the "Pre" commands generate. With -clean the build tool
removes them together with what it makes there itself -- objects, links
not yet installed, archives installed into a source directory, a kernel's
generated C file and headers, the targets of "Pre" cp and mv commands --
and builds nothing, like mk clean. Installed targets stay.

DEPENDENCIES (-D)

Every compile writes, beside x.o, gcc's dependency file x.d (-MMD: the
source and the headers it read) with a signature of the compile command.
With -D the build tool does only what is out of date, as mk does: it
compiles a source whose object is missing, of another $ARCH, made by
another command, or older than the source or one of its headers;
archives a library again (from all its objects) when one of them is
newer than the archive; links a program again when one of its objects
or libraries is newer; and each command of "SourceFilesCmd" by its
object and installed program; an archive also when its members are
not its objects (an "Append" part: when its own are missing, after the
base part made the archive anew). "Pre" and "Post" commands always run;
a file named by "Clean" (or a "Pre" cp/mv target) that they write again
with the same content keeps its time, as do a kernel's generated C file
(but for its kerndate) and headers.

KERNEL OBJECT

A build object has the following keys and types:

	Systab   string
	ConfFile string
	Ramfiles map[string]string
	Config   kernconfig

kernconfig:

	Code []string  // the "port" section: arbitrary C
	Dev  []string  // the "dev" section
	Link []string  // the "link" section
	Ip   []string  // the "ip" section
	Misc []string  // the "misc" section

Every element of Code, Dev, Link, Ip and Misc is one line of the
corresponding section of a 9front kernel CONF file, e.g. "pnp	pci"
or "vgasoft	=cur	swcursor".  This preserves the mkdevc(8) and
mkdevlist(8) semantics exactly.  "Systab" is the header defining the
syscall numbers, used to generate ../port/systab.h; errstr.h is derived
from ../port/error.h relative to the kernel build directory.

"ConfFile" is the CONF file whose contents end up in the kernel as
configfile[] (shown by `cat /dev/config`).

"Ramfiles" is an object of name, path pairs of binaries at "path" that will
be baked into the kernel and made available at boot time as a boot file
named "name" (addbootfile(2), the bootdir section of the CONF).
*/
package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
)

var helptext = `
The buildfile is looked for at these positions, in this order:

  ./$arg
  ./$arg/build.json

If the buildfile argument is not provided, it defaults to "build.json".

-clean removes the build's artefacts instead of building (see "Clean").

-D builds only what is out of date (see DEPENDENCIES in the godoc).

After the buildfile, a number of regexps specifying targets may be provided.
If a target matches any supplied regexp, it is acted on. These regexps only
apply to the top-level buildfile.

BUILDFILE

See the build godoc for more information about the buildfile format.

ENVIRONMENT

ARCH is needed. Current acceptable values are: 386 amd64

CC is needed and should be a compiler able to cope with the kernel and
libraries (gcc, clang, ...).  TOOLPREFIX, AR, LD, RANLIB, STRIP and SH are
optional.

PLAN9 may be supplied to point at a 9front tree. The default is to attempt
to find the top level of a git repository.
`

func init() {
	flag.Usage = func() {
		fmt.Fprintf(os.Stderr, "Usage of %s:\n", os.Args[0])
		fmt.Fprintf(os.Stderr, "  %s [options] [buildfile] [target...]\n\n", os.Args[0])
		flag.PrintDefaults()
		fmt.Fprintln(os.Stderr, helptext)
		fmt.Fprintln(os.Stderr, "Tools to be used with current settings:")
		fmt.Fprintf(os.Stderr, "  prefix ($TOOLPREFIX): %q\n", os.Getenv("TOOLPREFIX"))
		for k, v := range tools {
			fmt.Fprintf(os.Stderr, "  %s ($%s): %s\n", k, strings.ToUpper(k), v)
		}
	}
}