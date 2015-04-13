#version 130

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
varying vec4 vertex;
varying vec4 color;

void main(){
	vertex = gl_ModelViewMatrix * gl_Vertex;
    gl_Position =  projection * view * vertex;
	color = gl_Color;
}