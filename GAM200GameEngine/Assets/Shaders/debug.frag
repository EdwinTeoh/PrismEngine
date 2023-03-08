#version 330 core

in vec3 clrCoordOut;

layout (location=0) out vec4 color;

void main(void) 
{
    color = vec4(clrCoordOut, 1.0f);
}