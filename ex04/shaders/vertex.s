#version 120

varying vec3 vert_light_dir;
varying vec3 vert_norm_dir;

void main() {
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

    vert_light_dir = (gl_ModelViewMatrix * vec4(0, 0, 0, 1) - gl_ModelViewMatrix * gl_Vertex).xyz;
    vert_norm_dir = (gl_ModelViewMatrix * vec4(0, 0, 1, 1)).xyz;

}
