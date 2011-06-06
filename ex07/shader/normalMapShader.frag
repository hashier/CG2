#version 120
//# TODO: load your diffuse texture and the normal map
//#       extract the normal for the current pixel
//#       compute lighting using the extracted normal and the pixel-to-eye vector in tangent space

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

varying vec3 eye_in_tangent_space;

void main(void) {
  vec3 surfaceColor = texture2D(diffuseTex, gl_TexCoord[0].st).rgb;
  vec3 normalColor = texture2D(normalTex, gl_TexCoord[0].st).rgb;

//  normalColor /= 255;
  normalColor -= vec3(0.5, 0.5, 0.5);
  normalColor *= 2;

  vec3 N = normalize(normalColor);
//  N = vec3(0, 0, 1);
  vec3 E = normalize(eye_in_tangent_space);
  float angle = max(0.0, dot(N, E));
  gl_FragColor = vec4(surfaceColor, 1) * angle;
}
