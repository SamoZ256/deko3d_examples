#version 460

layout (location = 0) in vec2 in_texCoord;
layout (location = 0) out vec4 out_color;

layout (binding = 0) uniform sampler2D tex;

void main() {
    out_color = texture(tex, in_texCoord);
}
