#version 120
uniform bool ambientColorEnabled;
uniform vec4 ambientColor;

uniform bool diffuseColorEnabled;
uniform vec4 diffuseColor;
uniform bool diffuseTexEnabled;
uniform sampler2D diffuseTex;

uniform bool specularColorEnabled;
uniform vec4 specularColor;
uniform bool specularTexEnabled;
uniform sampler2D specularTex;

uniform bool emissiveColorEnabled;
uniform vec4 emissiveColor;
uniform bool emissiveTexEnabled;
uniform sampler2D emissiveTex;

uniform bool normalTexEnabled;
uniform sampler2D normalTex;

// view vectors //
varying vec3 Pixel2Eye_TS;
varying vec3 Pixel2Light_TS;

void main(void) {
  // ambient component //
  vec3 A = vec3(0);
  if (ambientColorEnabled) {
    A = ambientColor.rgb;
  }

  // diffuse component //
  vec3 D = vec3(0);
  if (diffuseColorEnabled) {
    if (diffuseTexEnabled) {
      D = texture2D(diffuseTex, gl_TexCoord[0].st).rgb;
    } else {
      D = diffuseColor.rgb;
    }
  }
  // specular component //
  vec3 S = vec3(0);
  float S_exp = 0;
  if (specularColorEnabled) {
    S = specularColor.rgb; 
    if (specularTexEnabled) {
      S_exp = texture2D(specularTex, gl_TexCoord[0].st).r;
    } else {
      S_exp = specularColor.a;
    }
  }
  // emissive component //
  vec3 E = vec3(0);
  if (emissiveColorEnabled) {
    if (emissiveTexEnabled) {
      E = texture2D(emissiveTex, gl_TexCoord[0].st).rgb;
    } else {
      E = emissiveColor.rgb;
    }
  }
  // normal component //
  vec3 normal_TS = vec3(0, 0, 1);
  if (normalTexEnabled) {
    normal_TS = normalize(texture2D(normalTex, gl_TexCoord[0].st).rgb * 2 - 1);
  }
  
  vec3 eD = normalize(Pixel2Eye_TS);
  vec3 lD = normalize(Pixel2Light_TS);
  float cosViewAngle = max(0.0, dot(normal_TS, eD));
  float cosLightAngle = max(0.0, dot(normal_TS, lD));
  
  // TODO: compute specular reflections ... //
  // compute half-way vector //
  float k = 1.0;
  if (specularColorEnabled) {
    vec3 H = normalize(eD + lD);
    k = max(0.0, dot(normal_TS, H));
  }
  
  // final shading //
  vec3 color = (A + D * cosLightAngle + S * pow(k, S_exp)) * cosViewAngle + E;
  gl_FragColor = vec4(color, 1.0);
}
