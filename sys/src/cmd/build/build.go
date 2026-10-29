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

// Build builds code as directed by json files.
// We slurp in the JSON, and recursively process includes.
// At the end, we issue a single cc command for all the files.
// Compilers are fast.
//
// ENVIRONMENT
//
// Needed: CC, PLAN9, ARCH
//
// PLAN9 should point to a 9front (Plan 9) root.
// A best-effort to autodetect the root is made if not explicitly set
// (HARVEY is honoured as a fallback for old habit, then the top level
// of the surrounding git repository).
//
// Optional: AR, LD, RANLIB, STRIP, SH, TOOLPREFIX
//
// These all control how the needed tools are found.
//
package main

import (
	"bytes"
	"crypto/sha1"
	"debug/elf"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"regexp"
	"runtime"
	"strconv"
	"strings"
	"time"
)

type kernconfig struct {
	// Every array element is one line of the corresponding section of a
	// 9front kernel CONF file, e.g. "pnp	pci" or "vgasoft	=cur	swcursor".
	// This preserves the mkdevc(8) / mkdevlist(8) semantics exactly.
	Code []string // the "port" section: arbitrary C
	Dev  []string // the "dev" section
	Link []string // the "link" section
	Ip   []string // the "ip" section
	Misc []string // the "misc" section
}

type kernel struct {
	// Systab is the header defining the syscall numbers.  It is used to
	// generate ../port/systab.h, errstr.h is derived from ../port/error.h.
	Systab string
	// ConfFile is the CONF file whose contents end up in the kernel as
	// configfile[] (shown by `cat /dev/config`).
	ConfFile string
	Config   kernconfig
	// Ramfiles are name, path pairs of binaries baked into the kernel and
	// made available at boot time under that name (addbootfile, bootlinks).
	Ramfiles map[string]string
}

type build struct {
	// jsons is unexported so can not be set in a .json file
	jsons []string
	path  string
	// -D: the commands of SourceFilesCmd to compile (the others are
	// linked again only)
	cmdstale map[string]bool
	Name  string
	// Projects name a whole subproject which is built independently of
	// this one. We'll need to be able to use environment variables at some point.
	Projects    []string
	Pre         []string
	Post        []string
	Cflags      []string
	Oflags      []string
	Include     []string
	SourceFiles []string
	ObjectFiles []string
	Libs        []string
	Env         []string
	SourceDeps  []string
	// cmd's
	SourceFilesCmd []string
	// CflagsShell: shell commands whose stdout (one flag per line) is
	// appended to Cflags at build time (rc `{...}` substitutions).
	CflagsShell []string
	// Targets.
	Program string
	Library string
	Install string // where to place the resulting binary/lib
	// Ape compiles like pcc: only /$ARCH/include/ape, /sys/include/ape and .
	Ape bool
	// IncludePath replaces the standard include path (/$ARCH/include,
	// /sys/include, . or Ape's): sources with headers of their own,
	// musl's in APE (sys/src/ape/lib/musl). Relative to the manifest's
	// directory, absolute ones re-rooted as the standard ones.
	IncludePath []string
	// Append adds to an existing Library archive instead of starting a
	// new one (libap: one archive from several directories, like mk's ar)
	Append bool
	// Shared: other manifests Append to this one's Library (libap's base
	// part); -D does not expect the archive to hold its objects only
	Shared bool
	// Clean names (globs, relative to the manifest's directory) the
	// files its Pre steps generate: yacc's y.tab.[ch], enam.c, ...
	// -clean removes them with what the build itself makes there.
	Clean []string
	// Objtag names the objects x.<Objtag>.o: the kernel's libraries
	// (klib.json) compile the userspace ones' sources in the same
	// directory with other flags; their objects stay apart (-D)
	Objtag string
	Kernel *kernel
}

type buildfile []build

func globby(s []string) []string {
	all := []string{}

	for _, n := range s {
		l, err := filepath.Glob(n)
		if err != nil || len(l) == 0 {
			all = append(all, n)
		} else {
			all = append(all, l...)
		}
	}

	debug("Glob of '%v' is '%v'", s, all)
	return all
}

// UnmarshalJSON works like the stdlib unmarshal would, except it adjusts all
// paths.
func (bf *buildfile) UnmarshalJSON(s []byte) error {
	r := make([]build, 0)
	if err := json.Unmarshal(s, &r); err != nil {
		return err
	}

	for k, b := range r {
		// we're getting a copy of the struct, remember.
		b.jsons = []string{}
		b.Projects = adjust(b.Projects)
		b.Libs = adjust(b.Libs)
		b.SourceDeps = adjust(b.SourceDeps)
		b.Cflags = adjust(b.Cflags)
		b.Oflags = adjust(b.Oflags)
		b.SourceFiles = globby(adjust(b.SourceFiles))
		b.SourceFilesCmd = globby(adjust(b.SourceFilesCmd))
		b.ObjectFiles = adjust(b.ObjectFiles)
		b.Include = adjust(b.Include)
		b.Install = fromRoot(b.Install)
		if b.Kernel != nil {
			b.Kernel.ConfFile = fromRoot(b.Kernel.ConfFile)
			b.Kernel.Systab = fromRoot(b.Kernel.Systab)
			for k, v := range b.Kernel.Ramfiles {
				b.Kernel.Ramfiles[k] = fromRoot(v)
			}
		}
		for i, e := range b.Env {
			b.Env[i] = os.ExpandEnv(e)
		}
		r[k] = b
	}
	*bf = r
	return nil
}

func (b *build) includedJson(filename string) bool {
	for _, includedJson := range b.jsons {
		if includedJson == filename {
			return true
		}
	}
	return false
}

var (
	plan9     string
	regexpAll = []*regexp.Regexp{regexp.MustCompile(".")}

	// findTools looks at all env vars and absolutizes these paths
	// also respects TOOLPREFIX
	tools = map[string]string{
		"cc":     "gcc",
		"ar":     "ar",
		"ld":     "ld",
		"ranlib": "ranlib",
		"strip":  "strip",
		"nm":      "nm",
		"objcopy": "objcopy",
	}
	arch = map[string]bool{
		"386":   true,
		"amd64": true,
		"arm64": true,
	}
	// per $ARCH defaults for the environment of the manifests and of
	// their Pre/Post steps (scripts/gen/arch.py has the same table):
	// objtype as 9front's mkfiles name it, O the object letter, SUFFIX
	// of the generated per-arch manifests (none for amd64), CONF the
	// kernel configuration, TOOLPREFIX of the gcc/binutils
	archEnv = map[string]map[string]string{
		"386":   {"objtype": "386", "O": "8", "SUFFIX": ".386", "CONF": "pc", "TOOLPREFIX": ""},
		"amd64": {"objtype": "amd64", "O": "6", "SUFFIX": "", "CONF": "pc64", "TOOLPREFIX": ""},
		"arm64": {"objtype": "arm64", "O": "7", "SUFFIX": ".arm64", "CONF": "qemu", "TOOLPREFIX": "aarch64-linux-musl-"},
	}
	debugPrint     = flag.Bool("debug", false, "enable debug prints")
	depends        = flag.Bool("D", false, "build only what is out of date, as mk: objects older than their sources or of another $ARCH, libraries and programs older than their objects or libraries")
	staticAnalysis = flag.Bool("analyze", false, "Do static analysis")
	shellhack      = flag.Bool("shellhack", false, "spawn every command in a shell (forced on if LD_PRELOAD is set)")
	cleanMode      = flag.Bool("clean", false, "remove what the build makes in the source directories instead of building (mk clean): objects, links, generated sources; installed targets stay")
)

func debug(fmt string, s ...interface{}) {
	if *debugPrint {
		log.Printf(fmt, s...)
	}
}

func fail(err error) {
	if err != nil {
		log.Fatalf("%s\n", err.Error())
	}
}

func adjust(s []string) []string {
	for i, v := range s {
		s[i] = fromRoot(v)
	}
	return s
}

// return the given absolute path as an absolute path rooted at the plan9 tree.
// idempotent: a path already rooted under plan9 (e.g. one that already went
// through fromRoot once during UnmarshalJSON's adjust(), and is now being
// passed through include()'s own fromRoot call) is left alone, since
// path.IsAbs is true either way and re-joining would double the prefix.
func fromRoot(p string) string {
	expandedPath := os.ExpandEnv(p)
	if path.IsAbs(expandedPath) && expandedPath != plan9 && !strings.HasPrefix(expandedPath, plan9+string(filepath.Separator)) {
		expandedPath = path.Join(plan9, expandedPath)
	}

	if strings.Contains(p, "$CC") {
		// Travis has versioned CCs of the form clang-X.Y.  We don't want to have
		// a file for each version of the compilers, so check if the versioned
		// file exists first.  If it doesn't, fall back to the unversioned file.
		expandedCc := os.Getenv("CC")
		expandedCcTokens := strings.Split(expandedCc, "-")
		fallbackCc := expandedCcTokens[0]
		//fmt.Printf(">>>CC=%v fallback=%v\n", expandedCc, fallbackCc)
		if len(expandedCcTokens) > 1 {
			if _, err := os.Stat(expandedPath); err != nil {
				if os.IsNotExist(err) {
					oldCc := os.Getenv("CC")
					os.Setenv("CC", fallbackCc)
					expandedPath = fromRoot(p)
					os.Setenv("CC", oldCc)
				}
			}
		}
	}

	return expandedPath
}

func include(f string, targ string, b *build) {
	debug("include(%s, %s, %v)", f, targ, b)

	f = fromRoot(f)
	if b.includedJson(f) {
		return
	}
	b.jsons = append(b.jsons, f)

	log.Printf("Including %s", f)
	builds := unmarshalBuild(f)

	for _, build := range builds {
		t := target(&build)
		if t != "" {
			targ = t
			break
		}
	}

	for _, build := range builds {
		log.Printf("Merging %s", build.Name)
		b.Cflags = append(b.Cflags, build.Cflags...)
		b.Oflags = append(b.Oflags, build.Oflags...)
		b.Pre = append(b.Pre, build.Pre...)
		b.Post = append(b.Post, build.Post...)
		b.Libs = append(b.Libs, build.Libs...)
		b.Projects = append(b.Projects, build.Projects...)
		b.Env = append(b.Env, build.Env...)
		b.SourceFilesCmd = append(b.SourceFilesCmd, build.SourceFilesCmd...)
		if build.Install != "" {
			if b.Install != "" && build.Install != b.Install {
				log.Fatalf("In file %s (target %s) included by %s (target %s): redefined Install: was %s, redefined to %s.", f, b.Name, build.path, build.Name, b.Install, build.Install)
			}
			b.Install = build.Install
		}
		b.ObjectFiles = append(b.ObjectFiles, build.ObjectFiles...)
		// For each source file, assume we create an object file with the last char replaced
		// with 'o'. We can get smarter later.
		b.SourceDeps = append(b.SourceDeps, build.SourceDeps...)

		for _, v := range build.SourceFiles {
			b.SourceFiles = append(b.SourceFiles, v)
			b.ObjectFiles = append(b.ObjectFiles, objectOf(b, v))
		}

		for _, v := range build.Include {
			if !path.IsAbs(v) {
				wd := path.Dir(f)
				v = path.Join(wd, v)
			}
			include(v, targ, b)
		}
		b.Program += build.Program
		b.Library += build.Library
		b.Clean = append(b.Clean, build.Clean...)
		b.Ape = b.Ape || build.Ape
		b.IncludePath = append(b.IncludePath, build.IncludePath...)
		b.Append = b.Append || build.Append
		b.Shared = b.Shared || build.Shared
	}
}

func contains(a []string, s string) bool {
	for i := range a {
		if a[i] == s {
			return true
		}
	}
	return false
}

func unmarshalBuild(f string) buildfile {
	d, err := ioutil.ReadFile(f)
	fail(err)

	var builds buildfile
	fail(json.Unmarshal(d, &builds))
	return builds
}

func process(f string, r []*regexp.Regexp) []build {
	log.Printf("Processing %s", f)
	var results []build

	builds := unmarshalBuild(f)

	for _, build := range builds {
		build.jsons = []string{}
		skip := true
		for _, re := range r {
			if re.MatchString(build.Name) {
				skip = false
				break
			}
		}
		if skip {
			continue
		}
		log.Printf("Running %s", build.Name)

		build.jsons = append(build.jsons, f)
		build.path = path.Dir(f)

		// For each source file, assume we create an object file with the last char replaced
		// with 'o'. We can get smarter later.
		t := target(&build)
		debug("\ttarget is '%s'", t)
		for _, v := range build.SourceFiles {
			o := objectOf(&build, v)
			if !contains(build.ObjectFiles, o) {
				build.ObjectFiles = append(build.ObjectFiles, o)
			}
		}

		for _, v := range build.Include {
			include(v, t, &build)
		}
		results = append(results, build)
	}
	return results
}

// For a kernel build, buildkernel generates <Name>.c in the working
// directory (the analogue of what `rc ../port/mkdevc $CONF` does, plus
// the bootlinks()/ramfile data of mkrootc(8)) and the two shared headers
// errstr.h and ../port/systab.h.  The generated file is added to the
// source list so that the single-cc compile step picks it up.
func buildkernel(b *build) {
	if b.Kernel == nil {
		return
	}
	errstr(b)
	systab(b)
	codebuf := confcode(b)
	fail(writeconf(b.Name+".c", codebuf))
	o := b.Name + ".c"
	if !contains(b.SourceFiles, o) {
		b.SourceFiles = append(b.SourceFiles, o)
	}
	oo := b.Name + ".o"
	if !contains(b.ObjectFiles, oo) {
		b.ObjectFiles = append(b.ObjectFiles, oo)
	}
}

// stale: n is missing, or one of d is missing or not older than n
func stale(n string, d []string) bool {
	fi, err := os.Stat(n)
	if err != nil {
		debug("stale: %s: %v", n, err)
		return true
	}
	for _, f := range d {
		di, err := os.Stat(f)
		if err != nil || !di.ModTime().Before(fi.ModTime()) {
			debug("stale: %s: %s is newer", n, f)
			return true
		}
	}
	return false
}

// outofdate (-D) cuts b down to what is out of date, as mk decides from
// the files' times: a source is compiled when its object is missing,
// for another $ARCH, made by another command (flags), or older than the
// source or a header it read (gcc's x.d); a library is archived again,
// from all its objects, when one of them is newer than it; a program is
// linked again when one of its objects or libraries is; each command of
// SourceFilesCmd by its object and its installed program. Pre and Post
// steps always run (keeptimes: unchanged output keeps its time). It
// reports whether b's program is to be linked and its library archived.
func outofdate(b *build) (link, archive bool) {
	command, cArgs, asmArgs := compileArgs(b)
	csig, asig := signature(command, cArgs), signature(tools["cc"], asmArgs)
	objstale := func(f string) bool {
		o := objectOf(b, f)
		want := csig
		if isasm(f) && len(b.SourceFilesCmd) == 0 {
			want = asig
		}
		deps, sig, ok := readdeps(b, f)
		return !ok || sig != want || !thisarch(o) || stale(o, append(deps, f))
	}
	var libs []string
	for _, l := range b.Libs {
		if !strings.HasPrefix(l, "-") {
			libs = append(libs, l)
		}
	}
	var srcs []string
	for _, f := range b.SourceFiles {
		if objstale(f) {
			srcs = append(srcs, f)
		}
	}
	b.SourceFiles = srcs
	if b.Library != "" {
		lib := path.Join(b.Install, b.Library)
		archive = len(srcs) > 0 || stale(lib, b.ObjectFiles) || !members(lib, b.ObjectFiles, !b.Append && !b.Shared)
	}
	if b.Program != "" {
		link = len(srcs) > 0 || stale(target(b), append(append([]string{}, b.ObjectFiles...), libs...))
	}
	var cmds []string
	b.cmdstale = map[string]bool{}
	for _, f := range b.SourceFilesCmd {
		exe, dir := cmdTarget(b, f)
		if objstale(f) {
			b.cmdstale[f] = true
			cmds = append(cmds, f)
		} else if stale(path.Join(dir, path.Base(exe)), append([]string{objectOf(b, f)}, libs...)) {
			cmds = append(cmds, f)
		}
	}
	b.SourceFilesCmd = cmds
	return link, archive
}

// members: archive a holds the objects os -- exactly those unless it is
// one part of an archive (Append, Shared). A base part archived again
// starts the archive anew, and the other parts are then missing from it.
func members(a string, os []string, exact bool) bool {
	out, err := exec.Command(tools["ar"], "t", a).Output()
	if err != nil {
		return false
	}
	have := map[string]bool{}
	for _, m := range strings.Fields(string(out)) {
		have[m] = true
	}
	for _, o := range os {
		if !have[o] {
			return false
		}
		delete(have, o)
	}
	return !exact || len(have) == 0
}

// thisarch: object o is for $ARCH -- the objects of every architecture
// are named x.o, one built for another is not up to date
func thisarch(o string) bool {
	f, err := elf.Open(o)
	if err != nil {
		return false
	}
	defer f.Close()
	m := map[string]elf.Machine{"386": elf.EM_386, "amd64": elf.EM_X86_64, "arm64": elf.EM_AARCH64}
	return f.Machine == m[os.Getenv("ARCH")]
}

// objectOf: the object of b's source f, in the working directory
func objectOf(b *build, f string) string {
	f = path.Base(f)
	f = f[:len(f)-len(path.Ext(f))]
	if b.Objtag != "" {
		f += "." + b.Objtag
	}
	return f + ".o"
}

func target(b *build) string {
	if b.Program != "" {
		return path.Join(b.Install, b.Program)
	}
	if b.Library != "" {
		return path.Join(b.Install, b.Library)
	}
	return ""
}

func cmdTarget(b *build, n string) (string, string) {
	ext := filepath.Ext(n)
	exe := n[:len(n)-len(ext)]
	return exe, b.Install
}

// compileArgs: the compiler and its arguments for b's C sources and for
// its assembly (.s/.S), without the files. -MMD: gcc writes x.d beside
// x.o, the headers it read, for -D.
func compileArgs(b *build) (command string, cArgs, asmArgs []string) {
	// N.B. Plan 9 has a very well defined include structure, just three things:
	// /$ARCH/include, /sys/include, .
	incArgs := []string{
		"-I", fromRoot("/$ARCH/include"),
		"-I", fromRoot("/sys/include"),
		"-I", ".",
	}
	if b.Ape {
		// pcc: cpp -N -I/$objtype/include/ape -I/sys/include/ape
		incArgs = []string{
			"-I", fromRoot("/$ARCH/include/ape"),
			"-I", fromRoot("/sys/include/ape"),
			"-I", ".",
		}
	}
	if len(b.IncludePath) > 0 {
		incArgs = nil
		for _, d := range b.IncludePath {
			if path.IsAbs(d) {
				d = fromRoot(d)
			}
			incArgs = append(incArgs, "-I", d)
		}
	}
	cArgs = []string{
		"-std=c11",
		"-c",
		"-MMD",
	}
	cArgs = append(cArgs, incArgs...)

	command = tools["cc"]
	if *staticAnalysis {
		command = "scan-build"
		cArgs = append([]string{tools["cc"]}, cArgs...)
	}
	cArgs = append(cArgs, b.Cflags...)

	asmArgs = []string{"-c", "-MMD"}
	// Same include structure as C so #include works in assembly.
	asmArgs = append(asmArgs, incArgs...)
	// Pass architecture/model flags that the C compiler would use
	for _, flag := range b.Cflags {
		if strings.HasPrefix(flag, "-m") || strings.HasPrefix(flag, "-fzero") || strings.HasPrefix(flag, "-fcommon") || flag == "-fno-pie" {
			asmArgs = append(asmArgs, flag)
		}
	}
	return command, cArgs, asmArgs
}

// shellflags: the flags b.CflagsShell's commands print (one per line),
// evaluated when compiling, as mk runs a recipe's `{...}. They are not
// part of the command's signature (signdeps): a date, say, changes on
// every run, and -D compiles again only what is out of date, as mk.
func shellflags(b *build) []string {
	var flags []string
	for _, sh := range b.CflagsShell {
		out, err := exec.Command("sh", "-c", sh).Output()
		if err != nil {
			log.Panicf("CflagsShell %q: %v", sh, err)
		}
		for _, line := range strings.Split(strings.TrimSpace(string(out)), "\n") {
			if line != "" {
				flags = append(flags, line)
			}
		}
	}
	return flags
}

func isasm(f string) bool {
	return strings.HasSuffix(f, ".s") || strings.HasSuffix(f, ".S")
}

func compile(b *build) {
	log.Printf("Building %s\n", b.Name)
	command, args, asmArgs := compileArgs(b)
	if len(b.SourceFilesCmd) > 0 {
		for _, sourceFile := range b.SourceFilesCmd {
			if b.cmdstale != nil && !b.cmdstale[sourceFile] {
				continue // -D: only to be linked again
			}
			cmd := exec.Command(command, append(append(args[:len(args):len(args)], shellflags(b)...), sourceFile)...)
			run(b, *shellhack, cmd)
			signdeps(b, sourceFile, command, args)
		}
		return
	}

	// Separate .s/.S (assembly) and .c (C) source files
	var asmFiles []string
	var cFiles []string
	for _, f := range b.SourceFiles {
		if isasm(f) {
			asmFiles = append(asmFiles, f)
		} else {
			cFiles = append(cFiles, f)
		}
	}

	// Assemble .s/.S files with gcc (uses gas)
	if len(asmFiles) > 0 {
		cc(b, tools["cc"], asmArgs, asmFiles)
	}

	// Compile .c files
	if len(cFiles) > 0 {
		cc(b, command, args, cFiles)
	}
}

// cc compiles the files fs in one run of the compiler; with an Objtag
// each by itself into its x.<Objtag>.o (-o), which leaves the x.o of
// the same source, and gcc names the dependency file after it
func cc(b *build, command string, args, fs []string) {
	cargs := append(args[:len(args):len(args)], shellflags(b)...)
	if b.Objtag == "" {
		run(b, *shellhack, exec.Command(command, append(cargs, fs...)...))
	} else {
		for _, f := range fs {
			run(b, *shellhack, exec.Command(command, append(cargs, "-o", objectOf(b, f), f)...))
		}
	}
	for _, f := range fs {
		signdeps(b, f, command, args)
	}
}

// the dependency file gcc -MMD writes for source f's object
func depfile(b *build, f string) string {
	o := objectOf(b, f)
	return o[:len(o)-len(".o")] + ".d"
}

// signature of a compile command (without its files)
func signature(command string, args []string) string {
	return fmt.Sprintf("%x", sha1.Sum([]byte(strings.Join(append([]string{command}, args...), "\x00"))))
}

const sigTag = "# build.exe "

// signdeps notes in f's dependency file the command that compiled it:
// the same sources are built with other flags into the same x.o (klibc,
// libc), -D compiles again when the command differs
func signdeps(b *build, f, command string, args []string) {
	d, err := os.OpenFile(depfile(b, f), os.O_WRONLY|os.O_APPEND, 0)
	if err != nil {
		return // no dependency file: -D compiles f again
	}
	defer d.Close()
	fmt.Fprintf(d, "%s%s\n", sigTag, signature(command, args))
}

// readdeps: the files f's object depends on (gcc's x.d: the source and
// the headers it read) and the signature of the command that made it;
// ok is false without a dependency file
func readdeps(b *build, f string) (deps []string, sig string, ok bool) {
	data, err := os.ReadFile(depfile(b, f))
	if err != nil {
		return nil, "", false
	}
	text := string(data)
	if i := strings.Index(text, sigTag); i >= 0 {
		sig = strings.TrimSpace(strings.SplitN(text[i+len(sigTag):], "\n", 2)[0])
		text = text[:i]
	}
	text = strings.ReplaceAll(text, "\\\n", " ")
	if i := strings.Index(text, ":"); i >= 0 {
		text = text[i+1:]
	}
	return strings.Fields(text), sig, true
}

// presolve links a program's objects into one relocatable object with
// its common symbols (tentative definitions) defined but weak, to get
// 8l's semantics: GNU ld pulls an archive member to replace a common
// symbol (stats' Machine *mach got libmach's Mach *mach = &mi386),
// 8l searches libraries only for undefined symbols. A weak definition
// pulls nothing, and a member linked for other reasons still supplies
// its definition (control.h's char *alignnames[] in libcontrol).
func presolve(b *build, out string, objs []string) string {
	if b.Kernel != nil {
		return ""
	}
	var commons []string
	o, err := exec.Command(tools["nm"], append([]string{"-P"}, objs...)...).Output()
	fail(err)
	for _, l := range strings.Split(string(o), "\n") {
		f := strings.Fields(l)
		if len(f) >= 2 && f[1] == "C" {
			commons = append(commons, f[0])
		}
	}
	r := out + ".r.o"
	// the emulation the final link names (386: -m elf_i386), else ld's
	// default (x86-64) refuses 32-bit objects
	var emul []string
	for i := 0; i+1 < len(b.Oflags); i++ {
		if b.Oflags[i] == "-m" {
			emul = []string{"-m", b.Oflags[i+1]}
		}
	}
	run(b, *shellhack, exec.Command(tools["ld"], append(append(emul, "-r", "-d", "-o", r), objs...)...))
	if len(commons) > 0 {
		w := out + ".weak"
		fail(os.WriteFile(w, []byte(strings.Join(commons, "\n")+"\n"), 0644))
		run(b, *shellhack, exec.Command(tools["objcopy"], "--weaken-symbols="+w, r))
	}
	return r
}

// clean removes what building b makes in its directory, as mk clean
// does: the objects and their dependency files (.d), presolve's
// .r.o/.weak, links not yet installed, archives installed into a
// source directory (program-local libraries,
// cmd/map/libmap/libmap.a6), a kernel's <Name>.c, errstr.h and
// ../port/systab.h, the targets of the Pre steps' cp and mv, and the
// Clean globs. Installed programs and libraries (/$ARCH/...) stay.
func clean(b *build) {
	var files []string
	for _, o := range b.ObjectFiles {
		files = append(files, o, o[:len(o)-len(path.Ext(o))]+".d")
	}
	for _, n := range b.SourceFilesCmd {
		n = n[:len(n)-len(filepath.Ext(n))]
		files = append(files, path.Base(n)+".o", path.Base(n)+".d", n+".out", n+".r.o", n+".weak")
	}
	if b.Program != "" {
		files = append(files, b.Program, b.Program+".r.o", b.Program+".weak")
	}
	if b.Library != "" && !strings.HasPrefix(b.Install, fromRoot("/$ARCH")+"/") && !strings.HasPrefix(b.Install, fromRoot("/tests")+"/") {
		files = append(files, path.Join(b.Install, b.Library))
	}
	if b.Kernel != nil {
		files = append(files, b.Name+".c", b.Name+".o", "errstr.h", "../port/systab.h")
	}
	files = append(files, generated(b)...)
	for _, f := range files {
		if d, err := isDir(f); err != nil || d {
			continue
		}
		log.Printf("rm %s\n", f)
		fail(os.Remove(f))
	}
}

// generated: what b's Pre steps make in its directory -- the targets of
// their cp and mv, and the Clean globs
func generated(b *build) []string {
	var files []string
	for _, c := range b.Pre {
		w := strings.Fields(c)
		if len(w) != 3 || (w[0] != "cp" && w[0] != "mv") {
			continue
		}
		t := w[2]
		if d, err := isDir(t); err == nil && d || strings.HasSuffix(t, "/") {
			t = path.Join(t, path.Base(w[1]))
		}
		files = append(files, t)
	}
	return append(files, globby(b.Clean)...)
}

type filestate struct {
	data  []byte
	mtime time.Time
}

// snapshot and keeptimes: a file the Pre steps write again with the
// same content gets its old time back, so that what depends on it is
// not out of date (mk makes generated files only when their sources
// change; the steps here make them every time)
func snapshot(files []string) map[string]filestate {
	m := map[string]filestate{}
	for _, f := range files {
		fi, err := os.Stat(f)
		if err != nil || fi.IsDir() {
			continue
		}
		data, err := os.ReadFile(f)
		if err != nil {
			continue
		}
		m[f] = filestate{data, fi.ModTime()}
	}
	return m
}

func keeptimes(m map[string]filestate) {
	for f, st := range m {
		data, err := os.ReadFile(f)
		if err != nil || !bytes.Equal(data, st.data) {
			continue
		}
		fail(os.Chtimes(f, time.Now(), st.mtime))
	}
}

var kerndateRE = regexp.MustCompile(`(?m)^ulong kerndate = [0-9]+;$`)

// writeconf writes a kernel's configuration C file: unless it differs
// from f in more than kerndate (its build time, as mkdevc writes it),
// f stays as it is -- with its date and time
func writeconf(f string, data []byte) error {
	if old, err := os.ReadFile(f); err == nil {
		if m := kerndateRE.Find(old); m != nil && bytes.Equal(kerndateRE.ReplaceAll(data, m), old) {
			return nil
		}
	}
	return os.WriteFile(f, data, 0666)
}

// writefile writes data to f, unless f has it already (keeps its time)
func writefile(f string, data []byte) error {
	if old, err := os.ReadFile(f); err == nil && bytes.Equal(old, data) {
		return nil
	}
	return os.WriteFile(f, data, 0666)
}

func link(b *build) {
	log.Printf("Linking %s\n", b.Name)
	if len(b.SourceFilesCmd) > 0 {
		for _, n := range b.SourceFilesCmd {
			// Split off the last element of the file
			var ext = filepath.Ext(n)
			if len(ext) == 0 {
				log.Fatalf("refusing to overwrite extension-less source file %v", n)
				continue
			}
			n = n[:len(n)-len(ext)]
			f := path.Base(n)
			o := f[:len(f)] + ".o"
			// link to <name>.out like mk's $O.<name>: a directory
			// named like the command (sys/src/cmd/test) would
			// otherwise be in the way; install renames it
			args := []string{"-o", n + ".out", o}
			if r := presolve(b, n, []string{o}); r != "" {
				args = []string{"-o", n + ".out", r}
			}
			args = append(args, "-L", fromRoot("/$ARCH/lib.gcc"))
			args = append(args, b.Libs...)
			args = append(args, b.Oflags...)
			run(b, *shellhack, exec.Command(tools["ld"], args...))
		}
		return
	}
	args := []string{"-o", b.Program}
	if r := presolve(b, b.Program, b.ObjectFiles); r != "" {
		args = append(args, r)
	} else {
		args = append(args, b.ObjectFiles...)
	}
	args = append(args, "-L", fromRoot("/$ARCH/lib.gcc"))
	args = append(args, b.Libs...)
	args = append(args, b.Oflags...)
	run(b, *shellhack, exec.Command(tools["ld"], args...))
}

// install puts the targets in place: with -D only a program linked anew
// (dolink) and a library archived anew (doarchive)
func install(b *build, dolink, doarchive bool) {
	if b.Install == "" {
		return
	}
	if len(b.SourceFilesCmd) == 0 && (b.Program != "" && !dolink || b.Program == "" && b.Library != "" && !doarchive) {
		return
	}

	log.Printf("Installing %s\n", b.Name)
	fail(os.MkdirAll(b.Install, 0755))

	switch {
	case len(b.SourceFilesCmd) > 0:
		for _, n := range b.SourceFilesCmd {
			exe, dir := cmdTarget(b, n)
			final := path.Join(dir, path.Base(exe))
			log.Printf("move %s %s\n", exe+".out", final)
			_ = os.Remove(final)
			fail(mv(exe+".out", final))
		}
	case len(b.Program) > 0:
		move(b.Program, b.Install)
	case len(b.Library) > 0:
		libpath := path.Join(b.Install, b.Library)
		// start from an empty archive: ar -r only adds/replaces members,
		// so objects of sources dropped from a manifest would linger
		// (-D: the Append parts see their members missing, members)
		if !b.Append {
			_ = os.Remove(libpath)
		}
		args := append([]string{"-rs", libpath}, b.ObjectFiles...)
		run(b, *shellhack, exec.Command(tools["ar"], args...))
		run(b, *shellhack, exec.Command(tools["ranlib"], libpath))
	}
}

func move(from, to string) {
	final := path.Join(to, from)
	log.Printf("move %s %s\n", from, final)
	_ = os.Remove(final)
	fail(mv(from, final))
}

// mv renames from to final, or copies it there (with its mode) and
// removes it: Plan 9 has no hard links and renames only within a
// directory
func mv(from, final string) error {
	if runtime.GOOS != "plan9" || path.Dir(from) == path.Dir(final) {
		return os.Rename(from, final)
	}
	fi, err := os.Stat(from)
	if err != nil {
		return err
	}
	in, err := os.Open(from)
	if err != nil {
		return err
	}
	defer in.Close()
	out, err := os.OpenFile(final, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, fi.Mode().Perm())
	if err != nil {
		return err
	}
	if _, err := io.Copy(out, in); err != nil {
		out.Close()
		return err
	}
	if err := out.Close(); err != nil {
		return err
	}
	return os.Remove(from)
}

func run(b *build, pipe bool, cmd *exec.Cmd) {
	sh := os.Getenv("SHELL")
	if sh == "" {
		sh = "sh"
	}

	if b != nil {
		cmd.Env = append(environ(), b.Env...)
	}
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if pipe {
		// Sh sends cmd to a shell. It's needed to enable $LD_PRELOAD tricks,
		// see https://github.com/Harvey-OS/harvey/issues/8#issuecomment-131235178
		shell := exec.Command(sh)
		shell.Env = cmd.Env
		shell.Stderr = os.Stderr
		shell.Stdout = os.Stdout

		commandString := cmd.Args[0]
		for _, a := range cmd.Args[1:] {
			if strings.Contains(a, "=") {
				commandString += " '" + a + "'"
			} else {
				commandString += " " + a
			}
		}
		shStdin, err := shell.StdinPipe()
		if err != nil {
			log.Fatalf("cannot pipe [%v] to %s: %v", commandString, sh, err)
		}
		go func() {
			defer shStdin.Close()
			io.WriteString(shStdin, commandString)
		}()

		log.Printf("%q | %s\n", commandString, sh)
		fail(shell.Run())
		return
	}
	log.Println(strings.Join(cmd.Args, " "))
	fail(cmd.Run())
}

func projects(b *build, r []*regexp.Regexp) {
	for _, v := range b.Projects {
		f, err := findBuildfile(v)
		log.Printf("Doing %s\n", f)
		if err != nil {
			log.Println(err)
		}
		project(f, r)
	}
}

func dirPop(s string) {
	fmt.Printf("Leaving directory `%v'\n", s)
	fail(os.Chdir(s))
}

func dirPush(s string) {
	fmt.Printf("Entering directory `%v'\n", s)
	fail(os.Chdir(s))
}

func runCmds(b *build, s []string) {
	for _, c := range s {
		args := adjust(strings.Split(c, " "))
		var exp []string
		for _, v := range args {
			e, err := filepath.Glob(v)
			debug("glob %v to %v err %v", v, e, err)
			if len(e) == 0 || err != nil {
				exp = append(exp, v)
			} else {
				exp = append(exp, e...)
			}
		}
		if runtime.GOOS == "plan9" {
			exp = plan9step(exp)
		}
		run(b, *shellhack, exec.Command(exp[0], exp[1:]...))
	}
}

// environ: os.Environ without rc's functions (fn#name) on Plan 9 --
// Go hands them on as plain variables, which a child rc cannot parse
func environ() []string {
	if runtime.GOOS != "plan9" {
		return os.Environ()
	}
	var env []string
	for _, e := range os.Environ() {
		if !strings.HasPrefix(e, "fn#") {
			env = append(env, e)
		}
	}
	return env
}

// plan9step: on 9front the build steps of the manifests are the rc
// scripts beside the Linux ones -- bash .../scripts/build/X.sh and
// python3 .../scripts/build/X.py run as rc .../scripts/build/rc/X.rc,
// which do the same with 9front's own tools (mk, yacc, awk, ...);
// install(1), which 9front has not, as rc .../scripts/build/rc/install.rc.
func plan9step(exp []string) []string {
	if (exp[0] == "bash" || exp[0] == "python3") && len(exp) > 1 && strings.Contains(exp[1], "scripts/build/") {
		dir, f := path.Split(exp[1])
		name := strings.TrimSuffix(strings.TrimSuffix(f, ".sh"), ".py")
		return append([]string{"rc", dir + "rc/" + name + ".rc"}, exp[2:]...)
	}
	if exp[0] == "install" {
		return append([]string{"rc", fromRoot("/scripts/build/rc/install.rc")}, exp[1:]...)
	}
	return exp
}

// assumes we are in the wd of the project.
func project(bf string, which []*regexp.Regexp) {
	cwd, err := os.Getwd()
	fail(err)
	debug("Start new project cwd is %v", cwd)
	defer dirPop(cwd)
	dir := path.Dir(bf)
	root := path.Base(bf)
	debug("CD to %v and build using %v", dir, root)
	dirPush(dir)
	builds := process(root, which)
	debug("Processing %v: %d target", root, len(builds))
	for _, b := range builds {
		debug("Processing %v: %v", b.Name, b)
		if *cleanMode {
			projects(&b, regexpAll)
			clean(&b)
			continue
		}
		projects(&b, regexpAll)
		// also without -D: a full build leaves the times for the next -D
		snap := snapshot(generated(&b))
		runCmds(&b, b.Pre)
		keeptimes(snap)
		buildkernel(&b)
		dolink, doarchive := true, true
		if *depends {
			dolink, doarchive = outofdate(&b)
		}
		if len(b.SourceFiles) > 0 || len(b.SourceFilesCmd) > 0 {
			compile(&b)
		}
		if b.Program != "" && dolink || len(b.SourceFilesCmd) > 0 {
			link(&b)
		}
		install(&b, dolink, doarchive)
		runCmds(&b, b.Post)
	}
}

func main() {
	log.SetFlags(0)

	// a build step for 9front, which has no python (elf2aout.go)
	if len(os.Args) == 4 && os.Args[1] == "-elf2aout" {
		if err := elf2aout(os.Args[2], os.Args[3]); err != nil {
			log.Fatalf("elf2aout: %v", err)
		}
		return
	}
	flag.Parse()
	setup()

	if os.Getenv("CC") == "" {
		log.Fatalf("You need to set the CC environment variable (e.g. gcc, clang, clang-3.6, ...)")
	}

	a := os.Getenv("ARCH")
	if a == "" || !arch[a] {
		s := []string{}
		for i := range arch {
			s = append(s, i)
		}
		log.Fatalf("You need to set the ARCH environment variable from: %v", s)
	}
	// the environment of the manifests and their steps: an unset variable gets
	// $ARCH's default, one set in the environment (even empty) stays
	for k, v := range archEnv[a] {
		if _, ok := os.LookupEnv(k); !ok {
			os.Setenv(k, v)
		}
	}
	if _, ok := os.LookupEnv("CPUS"); !ok {
		os.Setenv("CPUS", strconv.Itoa(runtime.NumCPU()))
	}
	findTools(os.Getenv("TOOLPREFIX"))

	// ensure this is exported, in case we used a default value
	os.Setenv("PLAN9", plan9)

	if os.Getenv("LD_PRELOAD") != "" {
		log.Println("Using shellhack")
		*shellhack = true
	}

	// If there is'n args, we search for a 'build.json' file
	// Otherwise the first argument could be
	// A path to a json file
	// or a directory containing the 'build.json' file
	// or a regular expression to apply assuming 'build.json'
	// Further arguments are considered regex.
	consumedArgs := 0
	var bf string
	if flag.NArg() > 0 {
		f, err := findBuildfile(flag.Arg(0))
		fail(err)

		if f == "" {
			fb, err := findBuildfile("build.json")
			fail(err)
			bf = fb
		} else {
			consumedArgs = 1
			bf = f
		}
	} else {
		// build.json here, else the tree's: everything for $ARCH
		f, err := findBuildfile("build.json")
		if err != nil {
			f, err = findBuildfile(path.Join(plan9, "build.json"))
		}
		fail(err)
		bf = f
	}

	re := []*regexp.Regexp{regexp.MustCompile(".")}
	if len(flag.Args()) > consumedArgs {
		re = re[:0]
		for _, r := range flag.Args()[consumedArgs:] {
			rx, err := regexp.Compile(r)
			fail(err)
			re = append(re, rx)
		}
	}
	project(bf, re)
}

func findTools(toolprefix string) {
	var err error
	for k, v := range tools {
		if x := os.Getenv(strings.ToUpper(k)); x != "" {
			v = x
		}
		v, err = exec.LookPath(toolprefix + v)
		fail(err)
		tools[k] = v
	}
}

func isDir(f string) (bool, error) {
	fi, err := os.Stat(f)
	if err != nil {
		return false, err
	}
	return fi.IsDir(), nil
}

// disambiguate the buildfile argument
func findBuildfile(f string) (string, error) {
	if !strings.HasSuffix(f, ".json") {
		b, err := isDir(f)
		if err != nil {
			// the path didn't exist
			return "", errors.New("unable to find buildfile " + f)
		}
		if b {
			f = path.Join(f, "build.json")
		} else {
			// this is a file without .json suffix
			return "", errors.New("buildfile must be a .json file, " + f + " is not a valid name")
		}
	}
	if b, err := isDir(f); err != nil || b {
		return "", errors.New("unable to find buildfile " + f)
	}
	return f, nil
}