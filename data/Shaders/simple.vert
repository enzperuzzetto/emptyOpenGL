#version 330 core

uniform mat4 mvp;

in vec3 vtx_position;

void main()
{
  gl_Position = mvp * vec4(vtx_position, 1);
}
