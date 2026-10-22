# shared settings of scripts/mesa/*.sh (sourced)
ROOT=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)
MESA=$ROOT/sys/src/mesa
field(){ awk -v k="$1" '$1 == k { print $2 }' "$MESA/VERSION"; }
MESA_VERSION=$(field version)
MESA_URL=$(field url)
MESA_SHA256=$(field sha256)
MESA_KEY=$(field key)
# work directory: downloads, the source tree (a git repository, tag
# pristine = the tarball), the venv with meson and Mesa's python
# modules, the build directories
WORK=${MESA_WORK:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-gpu}
SRC=$WORK/src/mesa-$MESA_VERSION
VENV=$WORK/venv
objtype=${objtype:-amd64}
case $objtype in
amd64)	TRIPLE=x86_64-pc-plan9 CPU=x86_64 ;;
386)	TRIPLE=i686-pc-plan9 CPU=x86 ;;
*)	echo "mesa: objtype=$objtype not supported" >&2; exit 1 ;;
esac
# the cross compilers of ../9front-gcc
GCC_DIR=${GCC_DIR:-$ROOT/../9front-gcc}
CROSS=$GCC_DIR/build/$objtype/cross/bin
BUILD=$WORK/build-$objtype
