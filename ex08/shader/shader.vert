#version 120
varying vec3 Pixel2Light_WS;
varying vec3 Normal_WS;

void main(void) {
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

  vec3 pixelPos_WS = (gl_ModelViewMatrix * gl_Vertex).xyz;
  vec3 lightPos_WS = (gl_LightSource[0].position).xyz;
  
  Pixel2Light_WS = lightPos_WS - pixelPos_WS;
  Normal_WS = gl_NormalMatrix * gl_Normal;
}
