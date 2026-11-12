#!/bin/bash
# weston's example clients for 9front (../9front-ui PLAN §12): weston-simple-shm
# (wl_shm) and weston-simple-egl (GLES on EGL's Wayland platform), built with
# ../9front-gcc against the tree (libwayland, libvulkan.a's EGL and GLES), from
# the release in sys/src/weston/VERSION (the sha256 is the check; the port's
# changes in sys/src/weston/patches). The protocols' code is made by the host's
# wayland-scanner from weston's and wayland-protocols' XML.
#
# usage: weston.sh [prepare|patches|build] [outdir]   (default build)
#   prepare  download, unpack to $WORK/src/weston-<version> as a git repository
#            (tag pristine), apply the patches (git am)
#   patches  write sys/src/weston/patches from the commits there
#   build    weston-simple-shm, weston-simple-egl into outdir ($BUILD/weston)
set -e
. "$(dirname "$0")/../mesa/common.sh"
WT=$ROOT/sys/src/weston
vfield(){ awk -v k="$1" '$1 == k { print $2 }' "$WT/VERSION"; }
WV=$(vfield version)
WSRC=$WORK/src/weston-$WV
WL=$ROOT/sys/src/wayland
wlv(){ awk -v k="$1" '$1 == k { print $2 }' "$WL/VERSION"; }

prepare(){
	mkdir -p "$WORK/dl"
	f=$WORK/dl/weston-$WV.tar.xz
	[ -f "$f" ] || curl -fsSL -o "$f" "$(vfield url)"
	echo "$(vfield sha256)  $f" | sha256sum -c - >/dev/null
	rm -rf "$WSRC"
	mkdir -p "$WORK/src"
	tar -C "$WORK/src" -xJf "$f"
	cd "$WSRC"
	git init -q
	git add -A
	git -c user.name=pristine -c user.email=pristine@localhost commit -q -m "weston $WV"
	git tag pristine
	for p in "$WT"/patches/*.patch; do
		[ -f "$p" ] || continue
		git -c user.name=port -c user.email=port@localhost am -q -3 "$p"
	done
	echo "weston: $WSRC ($(git rev-list --count pristine..HEAD) patches)"
}

patches(){
	cd "$WSRC"
	[ -z "$(git status --porcelain)" ] || { echo "weston: commit the changes in $WSRC first" >&2; exit 1; }
	rm -f "$WT"/patches/*.patch
	git format-patch -q --no-signature --zero-commit -N -o "$WT/patches" pristine..HEAD
	ls "$WT/patches"
}

build(){
	[ -d "$WSRC/.git" ] || prepare
	O=${1:-$BUILD/weston}
	G=$BUILD/weston-gen
	mkdir -p "$O" "$G"
	# wayland-protocols' XML (the release of sys/src/wayland/VERSION, checked)
	WP=$BUILD/wayland-protocols-$(wlv protocols)
	if [ ! -d "$WP" ]; then
		t=$(wlv protocols-tar)
		t=${t/#\$DISTFILES/$DISTFILES}	# the distribution's archives (scripts/lib/paths.sh)
		echo "$(wlv protocols-sha256)  $t" | sha256sum -c --quiet
		tar xJf "$t" -C "$BUILD"
	fi
	for x in "$WP/stable/xdg-shell/xdg-shell.xml" "$WP/stable/viewporter/viewporter.xml" \
		"$WP/staging/fractional-scale/fractional-scale-v1.xml" \
		"$WP/staging/tearing-control/tearing-control-v1.xml" "$WSRC/protocol/ivi-application.xml"; do
		n=$(basename "$x" .xml)
		wayland-scanner client-header "$x" "$G/$n-client-protocol.h"
		wayland-scanner private-code "$x" "$G/$n-protocol.c"
	done
	# meson's config.h, what the two clients and os-compatibility.c ask
	cat > "$G/config.h" <<EOC
#define HAVE_MEMFD_CREATE 1
#define HAVE_POSIX_FALLOCATE 1
#define ENABLE_EGL 1
EOC
	cc="$CROSS/$TRIPLE-gcc -std=gnu11 -O2 -Wall -Wno-unused-parameter -D_GNU_SOURCE -I$G -I$WSRC -I$WSRC/include -I$WSRC/shared"
	gen(){ for n in "$@"; do echo "$G/$n-protocol.c"; done; }
	$cc -o "$O/weston-simple-shm" "$WSRC/clients/simple-shm.c" "$WSRC/shared/os-compatibility.c" \
		$(gen xdg-shell ivi-application) -lwayland-client -lffi
	# g++: libvulkan.a (EGL, GLES: zink) has C++ in it
	for c in "$WSRC/clients/simple-egl.c" "$WSRC/shared/matrix.c" $(gen xdg-shell ivi-application viewporter fractional-scale-v1 tearing-control-v1); do
		$cc -c -o "$G/$(basename "$c" .c).o" "$c"
	done
	"$CROSS/$TRIPLE-g++" -o "$O/weston-simple-egl" "$G/simple-egl.o" "$G/matrix.o" \
		$(for n in xdg-shell ivi-application viewporter fractional-scale-v1 tearing-control-v1; do echo "$G/$n-protocol.o"; done) \
		-Wl,--gc-sections -lvulkan -ldraw -l9 -lfmt -lutf -lwayland-egl -lwayland-cursor -lwayland-client -lffi -lm
	echo "weston: $O/weston-simple-shm $O/weston-simple-egl"
}

case ${1:-build} in
prepare|patches)	"$1" ;;
build)	build "$2" ;;
*)	echo "usage: weston.sh [prepare|patches|build] [outdir]" >&2; exit 1 ;;
esac
