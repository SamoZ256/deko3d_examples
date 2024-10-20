#version 460

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texCoord;

layout (location = 0) out vec2 out_texCoord;

void main() {
    gl_Position = vec4(a_pos, 1.0);
    out_texCoord = a_texCoord;
}
