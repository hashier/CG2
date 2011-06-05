#version 120
//# TODO: load your diffuse texture and the normal map
//#       extract the normal for the current pixel
//#       compute lighting using the extracted normal and the pixel-to-eye vector in tangent space

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

varying vec3 normal;
varying vec3 eyeDir;
varying vec3 eye_in_tangent_space;

void main(void) {
  vec3 surfaceColor = texture2D(diffuseTex, gl_TexCoord[0].st).rgb;
  vec3 normalColor = texture2D(normalTex, gl_TexCoord[0].st).rgb;
  gl_FragColor = vec4(surfaceColor, 1);

  normalColor /= 255;
  normalColor -= vec3(0.5, 0.5, 0.5);
  normalColor *= 2;
  // TODO ich weiß noch gar nicht, ob normalmapping so wirklich funktioniert
  vec3 mapped_normal = normal + normalColor;

  // TODO ist nur drin um zu gucken ob das ambiente licht richtig gemacht wird
//  mapped_normal = normal;

  vec3 N = normalize(mapped_normal);
  // TODO irgendwie ist das ambiente gucken noch kaputt und textur geht auch nicht
  vec3 E = normalize(eye_in_tangent_space);
//  vec3 E = normalize(eyeDir);
  float angle = max(0.0, dot(N, E));
  gl_FragColor *= angle;
}
