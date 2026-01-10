#version 410

uniform vec4 vColor;
uniform vec3 vLightColor;
uniform vec3 vLightDirection;
uniform vec3 vLightPosition;
uniform vec3 vAmbientColor;

uniform vec3 vLightAttenuation;

in vec3 vNormal0;
in vec3 vPosOut;

out vec4 vOutColor;

void main()
{
	vec3 normal=normalize(vNormal0);
	vec3 toLight = vLightPosition - vPosOut;
	vec3 lightDirection = normalize(toLight);
	
	float lightIntensity=max(dot(normal, lightDirection), 0.0);
	//float lightIntensity=(dot(normal, lightDirection) + 1.0) * 0.5;
	
	float distance = length(toLight);
	float attenuation = 1.0 / (vLightAttenuation.x + vLightAttenuation.y * distance + vLightAttenuation.z * (distance * distance));
	
	vOutColor.rgb=vColor.rgb*(vLightColor*lightIntensity*attenuation + vAmbientColor);
	//vOutColor.rgb=vec3(attenuation);
	vOutColor.a=vColor.a;
}