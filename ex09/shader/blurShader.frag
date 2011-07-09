#version 120
uniform int blurAxis; // 0 - horizontal, 1 - vertical //
uniform int width;
uniform sampler2D inputImage;
uniform sampler2D depthImage;

uniform float focusDepth;
uniform float blurStrength;

void main(void) {
  float depth = texture2D(depthImage, gl_TexCoord[0].st).r; 
  
  float dDepth = abs(focusDepth - depth);
  int sampleRadius = int(floor(dDepth * blurStrength));
  float sampleStep = 1.0f / width;
  vec3 color = vec3(0.0);
  for (int i = -sampleRadius; i <= sampleRadius; ++i) {
    color += texture2D(inputImage, gl_TexCoord[0].st + vec2((1 - blurAxis) * i * sampleStep, blurAxis * i * sampleStep)).rgb; 
  }
  color /= 2 * sampleRadius + 1;

  gl_FragColor = vec4(color, 1.0);
}
