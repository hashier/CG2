#version 120
uniform sampler2D inputImage0;
uniform sampler2D inputImage1;

void main(void) {
  vec3 color = vec3(0);
  color += texture2D(inputImage0, gl_TexCoord[0].st).rgb;
  color += texture2D(inputImage1, gl_TexCoord[0].st).rgb;
  gl_FragColor = vec4(color, 1.0);
}
