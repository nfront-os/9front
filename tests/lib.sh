# shared helpers for tests/*/run.sh (sourced)
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
# the architecture under test (scripts/gen/arch.py): objtype, O, TOOLPREFIX,
# KERNEL (its configuration: pc64, pc, qemu), QEMU, SUFFIX (manifests);
# KBIN the ELF kernel the build leaves in /$objtype/bin
eval "$(python3 "$ROOT/scripts/gen/arch.py")" || exit 1
# riscv64 MACHINE=pf, um: that machine's kernel (scripts/guest/boot-cli.sh)
[ "$objtype" != riscv64 ] || [ -z "$MACHINE" ] || KERNEL=$MACHINE
KBIN=$objtype/bin/9$KERNEL
case $objtype in amd64) KDIR=pc64 ;; 386) KDIR=pc ;; *) KDIR=$objtype ;; esac	# sys/src/9/$KDIR
export objtype SUFFIX KERNEL KBIN KDIR
# guests with several processors: threads really run at once (races one hides)
export SMP=${SMP:-4}
# the build tool (go build in sys/src/cmd/build) and how to build everything
BUILDEXE=$ROOT/sys/src/cmd/build/build.exe
BUILDHOW="CC=gcc ARCH=$objtype sys/src/cmd/build/build.exe"
TMPD=$(mktemp -d)
trap 'rm -rf "$TMPD"' EXIT
npass=0
nfail=0

pass(){ echo "  PASS $*"; npass=$((npass+1)); }
fail(){ echo "  FAIL $*"; nfail=$((nfail+1)); }
# check name command...: PASS if the command succeeds
check(){ local n=$1; shift; if "$@" > "$TMPD/out" 2>&1; then pass "$n"; else fail "$n"; sed 's/^/       /' "$TMPD/out" | head -15; return 1; fi; }
# skip name reason
skip(){ echo "  SKIP $1 ($2)"; }
done_tests(){ echo "  -- $npass passed, $nfail failed"; [ $nfail -eq 0 ]; }
