#version 130

uniform float ambientLevel;
uniform vec3 eye;
uniform sampler2D texture;
uniform sampler2D normalMap;

varying vec4 overtex; // Original vertex
varying vec4 vertex; // Vertex modified by wobbling
varying vec4 color;
varying vec3 in_normal;

vec4 triplanar(sampler2D sampler, vec3 position, vec3 normal){
	vec3 blending = normalize(abs(normal));
	float blendSum = blending.x + blending.y + blending.z;
	blending /= vec3(blendSum,blendSum,blendSum);
	return
		texture2D(sampler,position.yz)*blending.x +
		texture2D(sampler,position.xz)*blending.y +
		texture2D(sampler,position.xy)*blending.z;
}
void main(){
	// Normal computing
	vec3 onormal = normalize(cross(dFdx(overtex.xyz), dFdy(overtex.xyz))); // Original normal
	vec3 flatnormal = normalize(cross(dFdx(vertex.xyz),dFdy(vertex.xyz)).xyz); // Normal modified by wobbling
	vec3 smoothnormal = normalize(in_normal);
	vec3 normal = normalize(flatnormal);
	vec3 eyeNormal = normalize(eye-vertex.xyz);

	// Normal mapping
	//mat3 tangentMatrix = mat3(bitangent,tangent,normal);
	//normal = normalize(tangentMatrix*((2*triplanar(normalMap,overtex.xyz/32.0,onormal).xyz)-1));
	
	// Texture mapping
	vec4 textureColor = triplanar(texture,overtex.xyz/128.0,normal);
	//textureColor = vec3(1.0,1.0,1.0);
	
	// Lighting
	vec3 lightDirection = normalize(gl_LightSource[0].position.xyz);
	float diffuse = max(.0,dot(gl_NormalMatrix*normal,lightDirection));
	float specular = max(.0,pow(dot(reflect(-lightDirection,gl_NormalMatrix*normal),eyeNormal),32.0))*.2;
	//specular = 0;
	
	//
	vec4 shadowColor = vec4(0/255,0/255,255/255,1);
	float chose = (diffuse*(1.0-ambientLevel)+ambientLevel)+specular;
	vec4 color2 = color*chose+shadowColor*(1-chose);
	gl_FragColor = textureColor.a*textureColor + (1.0-textureColor.a)*color2;
	
	// Fresnel
	gl_FragColor += clamp(.5-abs(dot(-normal,eyeNormal)),.0,1.0);
}