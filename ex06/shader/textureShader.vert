#version 120

uniform sampler2D earthmap;
varying vec3 norm_dir, view_dir, light_dir;

void main() {
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

  norm_dir = gl_NormalMatrix * gl_Normal;
  view_dir = -(gl_ModelViewMatrix * gl_Vertex).xyz;
  light_dir = gl_LightSource[0].position.xyz + view_dir;

  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
