uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec4 overtex;
varying vec4 vertex;
varying vec4 color;
varying vec3 in_normal;

void main(){
	vertex = overtex = gl_ModelViewMatrix * gl_Vertex;
	vertex.z += (sin(time*2.0+overtex.x/4.0) + sin(time/8.0 + overtex.y/8.0))/4.0;
	vertex.y += (sin(time*2.0+overtex.x/4.0) + sin(time/8.0 + overtex.z/8.0))/4.0;
    gl_Position = gl_ProjectionMatrix * view * vertex;
	in_normal = normalize(gl_Normal);
	color = gl_Color;
}