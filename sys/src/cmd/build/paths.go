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
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)

// setup finds the 9front root.  PLAN9 wins; HARVEY is honoured as a
// fallback for old habit; as a last resort the top level of the
// surrounding git repository is used.  The current process env is
// patched so the rest of the code can rely on $PLAN9, $ARCH.
func setup() {
	plan9 = os.Getenv("PLAN9")
	if plan9 == "" {
		plan9 = os.Getenv("HARVEY")
	}
	if plan9 == "" {
		// Try figuring it out from git instead
		out, err := exec.Command("git", "rev-parse", "--show-toplevel").Output()
		if err == nil {
			plan9 = strings.TrimSpace(string(out))
		}
	}
	// If we still haven't found the path, bail out.
	if plan9 == "" {
		log.Fatal("Set the PLAN9 environment variable or run from a git checkout.")
	}

	// Normalise to an absolute, clean path.
	if abs, err := filepath.Abs(plan9); err == nil {
		plan9 = abs
	}
	plan9 = filepath.ToSlash(plan9)

	// util is where the plan9 tools (mk, rc, sam, ...) used by the
	// projects land; prepend it to $PATH.
	os.Setenv("PATH", strings.Join([]string{fromRoot("/util"), os.Getenv("PATH")}, string(os.PathListSeparator)))
}