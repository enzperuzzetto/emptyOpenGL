#version 330 core

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat3 normal_mat;
uniform mat2 textcoord_mat;

in vec3 vtx_position;
in vec3 vtx_color;
in vec3 vtx_normal;
in vec2 vtx_texcoord;
in vec3 vtx_tangente;
in vec3 vtx_bitangente;

out vec3 v_normal;
out vec3 v_view;
out vec3 v_color;
out vec2 textcoord;
out vec3 tangente;
out vec3 bitangente;


void main()
{
  tangente = normalize(normal_mat * vtx_tangente);
  bitangente = normalize(normal_mat * vtx_bitangente);
  v_normal = normalize(normal_mat * vtx_normal);
  v_color = vtx_color;
  vec4 p = view_mat * (obj_mat * vec4(vtx_position, 1.));
  v_view = normalize(-p.xyz);
  textcoord = vtx_texcoord;//textcoord_mat * vtx_textcoord;
  gl_Position = proj_mat * p;
}
