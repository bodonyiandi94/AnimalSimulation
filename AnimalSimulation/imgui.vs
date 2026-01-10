#version 410

// Projection matrix
uniform mat4 mProj;

// Input attribs.
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vUv;
layout (location = 2) in vec4 vColor;

// Output attribs.
out vec2 vUvVs;
out vec4 vColorVs;

void main()
{
	vUvVs = vUv;
	vColorVs = vColor;
    
	gl_Position = mProj * vec4(vPos.xy, 0, 1);
}