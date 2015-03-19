uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
varying vec4 vertex;

void main(){
	vertex = gl_Vertex;
    gl_Position =  gl_ProjectionMatrix * view * gl_ModelViewMatrix * gl_Vertex;
}