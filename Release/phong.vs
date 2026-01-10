#version 410

uniform mat4 mTransform;
uniform mat4 mNormal;

in vec3 vPos;
in vec3 vNormal;

out vec3 vNormal0;

void main()
{
	vNormal0=normalize(mNormal*vec4(vNormal,0)).xyz;
	gl_Position=mTransform*vec4(vPos, 1);
}