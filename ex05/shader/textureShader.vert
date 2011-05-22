#version 120

uniform sampler2D tex0;
varying vec3 norm_dir;
varying vec3 pix_dir;
//varying vec2 texture_coordinate;

void main() {
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
  norm_dir = gl_NormalMatrix * gl_Normal;
  pix_dir = (gl_ModelViewMatrix * gl_Vertex).xyz;
//  texture_coordinate = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
