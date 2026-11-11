# cargostd dir target: cargo's target directory dir loses what it built for
# target when ../9front-rust's std is another than then (cargo looks at
# rustc's version only: a std built again by the same rustc leaves crates
# built with the old one, which rustc then refuses, "can't find crate").
# Sourced by scripts/build/cargo.sh and the tests that run cargo (tests/winit,
# tests/uiserver); needs $RUSTC.
cargostd(){
	local std
	std=$(ls -l --time-style=+%s "$("$RUSTC" --print sysroot)"/lib/rustlib/$2/lib/libstd-*.rlib 2>/dev/null)
	[ "$(cat "$1/$2.std" 2>/dev/null)" = "$std" ] && return
	rm -rf "${1:?}/$2"
	mkdir -p "$1"
	printf '%s\n' "$std" > "$1/$2.std"
}
