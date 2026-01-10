#version 410

uniform vec4 vColor;
uniform vec3 vLightColor;
uniform vec3 vLightDirection;

in vec3 vNormal0;

out vec4 vOutColor;

void main()
{
	vec3 normal=normalize(vNormal0);
	float lightIntensity=max(dot(normal, -vLightDirection), 0.0);
	
	vOutColor.rgb=vColor.rgb*(vLightColor*lightIntensity);
	vOutColor.a=vColor.a;
}