#version 410

uniform vec4 vColor;
uniform vec3 vLightColor;
uniform vec3 vLightDirection;
uniform vec3 vLightPosition;
uniform vec3 vAmbientColor;

in vec3 vNormal0;
in vec3 vPosOut;

out vec4 vOutColor;

void main()
{
	vec3 normal=normalize(vNormal0);
	float lightIntensity=max(dot(normal, -vLightDirection), 0.0);
	//float lightIntensity=(dot(normal, -vLightDirection) + 1.0) * 0.5;
	
	vOutColor.rgb=vColor.rgb*(vLightColor*lightIntensity + vAmbientColor);
	vOutColor.a=vColor.a;
}