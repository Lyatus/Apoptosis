varying vec4 vertex;
varying vec4 color;

float border(float n){
	return pow(abs(fract(n)-.5)*2.0,32);
}
void main(){
	gl_FragColor.rgb = color.rgb*(.5+(border(vertex.x)+border(vertex.y)+border(vertex.z))/6.0);
	gl_FragColor.a = .75;
}