#version 120
varying vec3 Pixel2Eye_TS;
varying vec3 Pixel2Light_TS;

// atributes from vertex array //
attribute vec3 vertex_OS;
attribute vec2 texCoord_OS;
attribute vec3 normal_OS;
attribute vec3 tangent_OS;
attribute vec3 bitangent_OS;

void main(void) {
  // standard stuff //
  gl_TexCoord[0] = gl_TextureMatrix[0] * vec4(texCoord_OS, 0, 1);
  gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_OS, 1);

  // eye position in world space //
  vec3 pixelPos_WS = (gl_ModelViewMatrix * vec4(vertex_OS, 1)).xyz;
  
  // tangent, bitangent and normal (in world space) //
  vec3 tangent = gl_NormalMatrix * tangent_OS;
  vec3 bitangent = gl_NormalMatrix * bitangent_OS;
  vec3 normal = gl_NormalMatrix * normal_OS;
    
  // use transpose of matrix //
  mat3 World2TangentSpace = mat3(tangent.x, bitangent.x, normal.x,
                                 tangent.y, bitangent.y, normal.y,
                                 tangent.z, bitangent.z, normal.z);
  // eye vector in tangent space //
  Pixel2Eye_TS = World2TangentSpace * (-pixelPos_WS);
  Pixel2Light_TS = World2TangentSpace * (gl_LightSource[0].position.xyz - pixelPos_WS);
}
