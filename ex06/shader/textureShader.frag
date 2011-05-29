#version 120

uniform sampler2D tex0;
varying vec3 norm_dir, view_dir, light_dir;

void main () {
  vec3 N = normalize(norm_dir);
  vec3 V = normalize(view_dir);
  vec3 L = normalize(light_dir);

  float NdotL = max(0.0, dot(N,L));

  gl_FragColor = texture2D(tex0, gl_TexCoord[0].st);
}
