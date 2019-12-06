#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 textcoord;
in vec3 tangente;
in vec3 bitangente;

uniform mat3 normal_mat;
//uniform sampler2D text2D;
//uniform sampler2D text2Ddif;
//uniform sampler2D text2D_2;
//uniform sampler2D text2D_3;
uniform sampler2D earth_normal;
uniform sampler2D earth;
uniform sampler2D earth_cloud;
uniform vec3 lightDir;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if(dc>0) {
    res = dCol * dc;
  }
  return res;
}

//vec4 normalized(int min, int max, int Min, int Max, vec4 vec)
//{
//    float a = (max-min)/(Max-Min);
//    float b = (min*Max - max*Min)/(Max - Min);
//    vec4 c = vec4(b,b,b,0);

//    vec4 norm = a * vec + c;
//    return norm;
//}

void main(void) {
  float ambient = 0.4;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);
//  out_color = mix(
//              (mix(
//                   texture(text2D_2,textcoord),
//                   texture(text2D,textcoord),
//                   0.5))
//              , texture(text2D_3,textcoord)
//              , max(dot(v_normal, lightDir),0));

  //vec3 color = mix( texture(earth,textcoord), texture(earth_cloud,textcoord), 0.5).xyz;
  vec3 color = texture(earth,textcoord).xyz;
  //vec3 normal = texture(text2D, textcoord).xyz*2.-1.;
  vec3 normal = texture(earth_normal, textcoord).xyz*2.-1.;

  vec3 tv = normalize(tangente);
  vec3 bv = normalize(bitangente);
  vec3 nv = normalize(v_normal);

  mat3 tbnvMatrix = mat3(tv, bv, nv);

  vec3 v_v = v_view * tbnvMatrix;
  vec3 l = lightDir * tbnvMatrix;

  //vec4 color_dif = texture(text2Ddif, textcoord);
  //normal = mat_view * normalized(-1, 1, 0, 1, normal);
  //vec3 color = color_dif.xyz;
  //normal = normal * normal_mat;

  //out_color = texture(text2D, textcoord);
  out_color = vec4(ambient * color + blinn(normalize(normal),normalize(v_v), l, v_color, spec_color, shininess),1.0);
  //out_color = vec4(tangente,1);
}
