#version 330 core

layout (location = 0) in vec3 posCoord;
layout (location = 1) in vec3 clrCoord;

out vec3 clrCoordOut;

uniform mat4 combined;

uniform vec2 texOffset;

void main(void) 
{
    gl_Position = combined * vec4(posCoord, 1.0f);
    clrCoordOut = clrCoord;
}