#version 330 core

uniform vec4 vertex_color;
uniform mat4 vertex_transform;
uniform mat4 projection;

layout (location=0) in vec4 vertex_position;
out vec4 color_for_fragment_shader;

void main()
{
    gl_Position = projection * (vertex_transform * vertex_position);
    color_for_fragment_shader = vertex_color;
}
