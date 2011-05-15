#version 120

varying vec3 vert_norm_dir;
varying vec3 vert_light_dir;

uniform float uni_outerSpotAngle;
uniform float uni_innerSpotAngle;

void main () {

    // normalize everything necessary //
    vec3 N = normalize(vert_norm_dir);
    vec3 L = normalize(vert_light_dir);

    // diffuse component //
    float NdotL = max(0.0, dot(N, L));
    if(NdotL < 0.1f)
        gl_FragColor = vec4(0.5, 0.5, 0.0, 1.0);
    else if(NdotL < uni_outerSpotAngle)
        gl_FragColor = vec4(0.0, 0.5, 0.5, 1.0) * NdotL;
    else
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragColor += vec4(0.2, 0.2, 0.2, 0.0);

}
