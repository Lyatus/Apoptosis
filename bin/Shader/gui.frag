#version 130

uniform sampler2D texture;
varying vec4 color;

void main(){
	gl_FragColor = color*texture2D(texture,gl_TexCoord[0].xy);
}