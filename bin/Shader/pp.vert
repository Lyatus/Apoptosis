#version 130

void main(){
	gl_TexCoord[0] = (gl_Vertex+1)/2;
    gl_Position =  gl_Vertex;
}