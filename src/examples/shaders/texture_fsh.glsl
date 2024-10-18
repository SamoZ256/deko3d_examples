#version 460

layout (location = 0) in vec2 in_texCoord;
layout (location = 0) out vec4 out_color;

void main() {
    out_color = vec4(in_texCoord, 0.0, 1.0);
}
