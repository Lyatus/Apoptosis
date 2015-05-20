#version 130

uniform mat4 view;
uniform mat4 projection;

varying vec4 color;

void main(){
    gl_Position = projection * view * gl_ModelViewMatrix * gl_Vertex;
	color = gl_Color;
}