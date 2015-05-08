#version 130

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 in_normal;
varying vec4 vertex;

void main(){
    gl_Position = projection * view * (vertex = gl_ModelViewMatrix * gl_Vertex);
	in_normal = normalize(gl_Normal);
}