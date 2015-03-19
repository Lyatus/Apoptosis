varying vec4 vertex;

float border(float n){
	return clamp(abs(fract(n)-.5)*2.0,.9,1.0);
}
void main(){
	gl_FragColor.rgb = vec3(1,1,0)*border(vertex.x)*border(vertex.y)*border(vertex.z);
	gl_FragColor.a = .5;
}