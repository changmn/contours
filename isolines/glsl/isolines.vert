#version 330 core

layout(location = 0) in vec2 a_texcoord;

void main() {
    gl_Position = vec4(a_texcoord, 0.0, 1.0);
}
