#version 130

uniform mat4 projection;
varying vec4 color;

void main(){
    gl_Position =  projection * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	color = gl_Color;
}