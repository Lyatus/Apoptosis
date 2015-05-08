#version 130

uniform float ambientLevel;
uniform vec3 eye;
uniform vec3 sphereCenter;
uniform float sphereRadius;

varying vec3 in_normal;
varying vec4 vertex;

void main(){
	// Normal computing
	vec3 normal = normalize(in_normal);
	vec3 eyeNormal = normalize(eye-vertex.xyz);
	
	// Lighting
	vec3 lightDirection = normalize(gl_LightSource[0].position.xyz);
	float diffuse = max(.0,dot(gl_NormalMatrix*normal,lightDirection));
	float specular = max(.0,pow(dot(reflect(-lightDirection,gl_NormalMatrix*normal),eyeNormal),32.0))*.2;
	//specular = 0;
	
	//
	vec4 shadowColor = vec4(0/255,0/255,255/255,1);
	float chose = (diffuse*(1.0-ambientLevel)+ambientLevel)+specular;
	gl_FragColor = vec4(1,1,1,1)*chose+shadowColor*(1-chose);
	
	// Fresnel
	gl_FragColor += clamp(.5-abs(dot(-normal,eyeNormal)),.0,1.0);
	
	// Sphere clipping
	gl_FragColor.a = min(8,max(0,sphereRadius-distance(vertex.xyz,sphereCenter)))/8;
}