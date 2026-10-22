#!/bin/bash
# Write sys/src/mesa/patches from the commits after the overlay in $SRC
# (one patch per commit, git format-patch), and copy the overlay's files
# back from $SRC (changes to new files are made there too).
set -e
. "$(dirname "$0")/common.sh"
cd "$SRC"
[ -z "$(git status --porcelain)" ] || { echo "patches: commit the changes in $SRC first" >&2; exit 1; }
rm -f "$MESA"/patches/*.patch
git format-patch -q --no-signature --zero-commit -N -o "$MESA/patches" overlay..HEAD
for f in $(git diff --name-only --diff-filter=A pristine overlay); do
	mkdir -p "$MESA/overlay/$(dirname "$f")"
	git show "HEAD:$f" > "$MESA/overlay/$f"
done
ls "$MESA/patches"
