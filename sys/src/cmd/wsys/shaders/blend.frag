// SPDX-License-Identifier: MIT
// wsys -g: a layer with alpha over what is under it: the pixel of the
// client's buffer (a8r8g8b8, premultiplied, little endian: b g r a in memory)
// at this point of the frame, read from the buffer itself (a storage buffer
// of its words); the pipeline blends ONE, ONE_MINUS_SRC_ALPHA
#version 450

layout(push_constant) uniform Layer {
	ivec2	at;	// where the buffer's top left corner is in the frame
	int	stride;	// words a row
} l;

layout(std430, set = 0, binding = 0) readonly buffer Pixels {
	uint	px[];
} b;

layout(location = 0) out vec4 colour;

void
main()
{
	ivec2 q = ivec2(gl_FragCoord.xy) - l.at;
	vec4 c = unpackUnorm4x8(b.px[q.y*l.stride + q.x]);
	colour = vec4(c.z, c.y, c.x, c.w);
}
