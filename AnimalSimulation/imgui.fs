#version 410

// Font/other texture.
uniform sampler2D sTexture;

// Attribs
in vec2 vUvVs;
in vec4 vColorVs;

// Output
out vec4 fragColor;

void main()
{
    fragColor = vColorVs * texture2D(sTexture, vUvVs.st);
}