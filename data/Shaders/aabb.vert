#version 330 core

uniform mat4 mvp;
uniform vec3 in_color;

in vec3 vtx_position;
out vec3 color;

void main()
{
  gl_Position = mvp * vec4(vtx_position, 1);
  color = in_color;
}
