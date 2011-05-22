#version 120

uniform sampler2D tex0;
varying vec3 norm_dir;
varying vec3 pix_dir;
//varying vec2 texture_coordinate;

void main () {
  vec3 N = normalize(norm_dir);
  vec3 P = normalize(pix_dir);
  vec3 E = vec3(0, 0, -1);
  float NdotP = max(0, dot(N, -P));
  float NdotE = max(0, dot(N, -E));

//  gl_FragColor = texture2D(tex0, texture_coordinate);
  gl_FragColor = texture2D(tex0, gl_TexCoord[0].st);
//  gl_FragColor = vec4(0.9, 0.9, 0.9, 1.0);
  
//  gl_FragColor *= NdotE;
  gl_FragColor *= NdotP;
}
