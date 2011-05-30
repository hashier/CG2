#version 120

uniform sampler2D earthmap;
uniform sampler2D earthspec;
uniform sampler2D earthcloudmap;
uniform sampler2D earthcloudmaptrans;
uniform sampler2D earthlights;

varying vec3 norm_dir, view_dir, light_dir;

void main () {
  vec3 N = normalize(norm_dir);
  vec3 V = normalize(view_dir);
  vec3 L = normalize(light_dir);

  float NdotL = max(0.0, dot(N,L));

  vec4 clouds = texture2D(earthcloudmap, gl_TexCoord[0].st);
  clouds *= texture2D(earthcloudmaptrans, gl_TexCoord[0].st);
  vec4 night = texture2D(earthlights, gl_TexCoord[0].st);

  gl_FragColor = (texture2D(earthmap, gl_TexCoord[0].st) + clouds) * NdotL + night * (1-NdotL);
}
