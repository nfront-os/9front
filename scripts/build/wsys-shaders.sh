#!/bin/bash
# wsys' shaders (sys/src/cmd/wsys/shaders: the GPU blending a layer with alpha,
# comp.rs) to SPIR-V with the host's glslc, checked by spirv-val; the .spv files
# are checked in (9front has no shader compiler) and included by comp.rs.
# usage: scripts/build/wsys-shaders.sh
set -e
D=$(git -C "$(dirname "${BASH_SOURCE[0]}")" rev-parse --show-toplevel)/sys/src/cmd/wsys/shaders
for s in blend.vert blend.frag; do
	glslc -O -o "$D/$s.spv.new" "$D/$s"
	spirv-val "$D/$s.spv.new"
	if cmp -s "$D/$s.spv.new" "$D/$s.spv"; then rm "$D/$s.spv.new"; else mv "$D/$s.spv.new" "$D/$s.spv"; echo "$s.spv made"; fi
done
