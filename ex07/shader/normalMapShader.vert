#version 120
//#TODO: read in your tangent, bitangent and normal values here
//#      compute M' as matrix mapping world space to tangent space
//#      transform the pixel-to-eye vector into tangent space and pass it to the fragment shader

attribute vec3 tangent;
attribute vec3 bitangent;

varying vec3 eye_in_tangent_space;

void main(void) {
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  vec3 eyeDir = -(gl_ModelViewMatrix * gl_Vertex).xyz;

  vec3 tangent_in_world_space = gl_NormalMatrix * tangent;
  vec3 bitangent_in_world_space = gl_NormalMatrix * bitangent;
  vec3 normal_in_world_space = gl_NormalMatrix * gl_Normal;

  mat3 M = mat3(tangent_in_world_space.x, tangent_in_world_space.y, tangent_in_world_space.z, bitangent_in_world_space.x, bitangent_in_world_space.y, bitangent_in_world_space.z, normal_in_world_space.x, normal_in_world_space.y, normal_in_world_space.z);

 eye_in_tangent_space = transpose(M) * eyeDir; 
}
