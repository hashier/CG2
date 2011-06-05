#version 120
//#TODO: read in your tangent, bitangent and normal values here
//#      compute M' as matrix mapping world space to tangent space
//#      transform the pixel-to-eye vector into tangent space and pass it to the fragment shader

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec3 tangent;
uniform vec3 bitangent;

varying vec3 normal;
varying vec3 eyeDir;
varying vec3 eye_in_tangent_space;

void main(void) {
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  normal = gl_NormalMatrix * gl_Normal;
  eyeDir = -(gl_ModelViewMatrix * gl_Vertex).xyz;

  vec4 tangent_in_world_space = gl_ModelViewMatrix * vec4(tangent, 0);
  vec4 bitangent_in_world_space = gl_ModelViewMatrix * vec4(bitangent, 0);
  vec4 normal_in_world_space = gl_ModelViewMatrix * vec4(gl_Normal, 0);

  mat4 M = mat4(tangent_in_world_space.x, tangent_in_world_space.y, tangent_in_world_space.z, 0, bitangent_in_world_space.x, bitangent_in_world_space.y, bitangent_in_world_space.z, 0, normal_in_world_space.x, normal_in_world_space.y, normal_in_world_space.z, 0, 0, 0, 0, 1);

 eye_in_tangent_space = (transpose(M) * (-gl_ModelViewMatrix * gl_Vertex)).xyz; 
}
