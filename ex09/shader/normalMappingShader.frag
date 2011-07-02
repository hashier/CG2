#version 120
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

varying vec3 Pixel2Eye_TS;

void main(void) {
  // earth color //
  vec3 diffuseColor = texture2D(diffuseTex, gl_TexCoord[0].st).rgb;
  
  // read in pixel normal (in tangent space) //
  vec3 normal_TS = normalize(texture2D(normalTex, gl_TexCoord[0].st).rgb * 2 - 1);
  vec3 eD = normalize(Pixel2Eye_TS);
  float cosViewAngle = max(0.0, dot(normal_TS, eD));
  
  // final shading //
  vec3 color = diffuseColor * cosViewAngle;
  gl_FragColor = vec4(color, 1.0);
}
