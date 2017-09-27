#version 330 core

in vec4 color_for_fragment_shader;
layout (location=0) out vec4 color;

void main()
{
    color = color_for_fragment_shader;
}
