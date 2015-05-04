#version 130

uniform float aspect;
uniform sampler2D color;
uniform sampler2D depth;

int sampling = 4;
vec3 blur(float size){
	float step = size;
	vec3 wtr = vec3(0,0,0);
	for(int x=-sampling;x<=sampling;x++)
		for(int y=-sampling;y<=sampling;y++)
			if(x*x+y*y<=sampling*sampling)
				wtr += texture2D(color,gl_TexCoord[0].xy+vec2(x*step,y*step)).rgb;
	return wtr/(3.14*sampling*sampling);//((sampling*2+1)*(sampling*2+1));
}
void main(){
	vec2 v = (gl_TexCoord[0]*2-1).xy;
	float atten = min(1,1.1-length(v)*.5);
	gl_FragColor.rgb = atten * blur(.002* (1-atten));
	gl_FragColor.a = 1;
}