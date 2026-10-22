// SPDX-License-Identifier: MIT
// vktri: the triangle's colour, from a push constant
#version 450
layout(push_constant) uniform Pc { vec4 color; } pc;
layout(location = 0) out vec4 outColor;
void main() {
	outColor = pc.color;
}
