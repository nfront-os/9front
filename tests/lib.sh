# shared helpers for tests/*/run.sh (sourced)
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
# the architecture under test (scripts/arch.py): objtype, O, TOOLPREFIX,
# KERNEL (its configuration: pc64, pc, qemu), QEMU, SUFFIX (manifests);
# KBIN the ELF kernel the build leaves in /$objtype/bin
eval "$(python3 "$ROOT/scripts/arch.py")" || exit 1
KBIN=$objtype/bin/9$KERNEL
case $objtype in amd64) KDIR=pc64 ;; 386) KDIR=pc ;; *) KDIR=$objtype ;; esac	# sys/src/9/$KDIR
export objtype SUFFIX KERNEL KBIN KDIR
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
