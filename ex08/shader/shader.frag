#version 120
varying vec3 Pixel2Light_WS;
varying vec3 Normal_WS;

void main(void) {
  vec3 diffuseColor = vec3(1.0, 0.6, 0.3);
  vec3 ambient = vec3(0.1, 0.1, 0.1);
  
  vec3 lD = normalize(Pixel2Light_WS);
  float cosLightAngle = max(0.0, dot(Normal_WS, lD));
  
  vec3 color = ambient + diffuseColor * cosLightAngle;
  gl_FragColor = vec4(color, 1.0);
}
