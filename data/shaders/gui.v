
#version 450 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv_v;
layout (location = 3) in vec4 color_v;

uniform mat4 transform;

out vec2 uv_f;
out vec3 color_f

void main() {

	uv_f = uv_v;
	color_f = color_v;
	gl_Position = transform * vec4(pos.x, pos.y, 0.0, 1.0);
}
