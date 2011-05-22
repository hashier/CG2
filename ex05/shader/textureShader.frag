#version 120

uniform sampler2D tex0;
varying vec3 norm_dir;
varying vec3 pix_dir;

void main () {
  vec3 N = normalize(norm_dir);
  vec3 P = normalize(pix_dir);
  float NdotP = max(0, dot(N, -P));

  gl_FragColor = texture2D(tex0, gl_TexCoord[0].st);
  
  gl_FragColor *= NdotP;
}
