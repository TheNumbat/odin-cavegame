
#version 450 core

flat in uint f_t, f_norm;
in float f_ao;
in vec2 f_uv;

out vec4 color;

uniform sampler2D tex;

void main() {

	color = texture(tex, f_uv);
}