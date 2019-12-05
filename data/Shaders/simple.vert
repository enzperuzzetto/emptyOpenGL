#version 330 core

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;

in vec3 vtx_position;
in vec3 vtx_color;

out vec3 v_color;


void main()
{
  v_color = vtx_color;
  gl_Position = proj_mat * view_mat * (obj_mat * vec4(vtx_position, 1.));
}