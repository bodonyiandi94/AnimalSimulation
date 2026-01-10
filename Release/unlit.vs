#version 410

uniform mat4 mTransform;

in vec3 vPos;

void main()
{
	gl_Position=mTransform*vec4(vPos, 1);
}