#version 120
uniform int blurAxis; // 0 - horizontal, 1 - vertical //
uniform int width;
uniform int blurRadius;
uniform sampler2D inputImage;


void main(void) {
  // pixel wise stepping //
  float sampleStep = 1.0 / float(width);
  vec3 color = texture2D(inputImage, gl_TexCoord[0].st).rgb;
  int rad = blurRadius;
  for (int i = -rad; i <= rad; ++i) {
    if (i != 0) {
      color += texture2D(inputImage, gl_TexCoord[0].st + vec2((1 - blurAxis) * i * sampleStep, blurAxis * i * sampleStep)).rgb; 
    }
  }
  color /= 2 * float(rad) + 1;

  gl_FragColor = vec4(color, 1.0);
}
