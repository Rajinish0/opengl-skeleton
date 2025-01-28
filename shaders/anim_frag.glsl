#version 430 core

in vec2 tCoord;
out vec4 fcol;

uniform sampler2D texture_diffuse1;

void main()
{
	fcol = texture(texture_diffuse1, tCoord );
    // fcol = vec4(0.1, 0.2, 0.4, 1.0);
}