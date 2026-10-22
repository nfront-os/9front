#!/bin/bash
# Unpack the tarball to $SRC as a git repository (tag pristine), copy
# sys/src/mesa/overlay over it (new files) and apply the patches of
# sys/src/mesa/patches (git am: one commit each). Throws away $SRC.
set -e
. "$(dirname "$0")/common.sh"
f=$WORK/dl/$(basename "$MESA_URL")
[ -f "$f" ] || "$(dirname "$0")/fetch.sh"
rm -rf "$SRC"
mkdir -p "$WORK/src"
tar -C "$WORK/src" -xJf "$f"
cd "$SRC"
git init -q
git add -A
git -c user.name=pristine -c user.email=pristine@localhost commit -q -m "mesa $MESA_VERSION"
git tag pristine
if [ -n "$(ls -A "$MESA/overlay")" ]; then
	cp -r "$MESA/overlay"/. .
	git add -A
	git -c user.name=overlay -c user.email=overlay@localhost commit -q -m "plan9: overlay (sys/src/mesa/overlay)"
fi
git tag overlay
for p in "$MESA"/patches/*.patch; do
	[ -f "$p" ] || continue
	git -c user.name=port -c user.email=port@localhost am -q -3 "$p"
done
echo "prepare: $SRC ($(git rev-list --count overlay..HEAD) patches)"
