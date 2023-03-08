#version 330 core

layout (location = 0) in vec3 posCoord;
layout (location = 1) in vec3 clrCoord;
layout (location = 2) in vec2 texCoord;

out vec3 clrCoordOut;
out vec2 texCoordOut;
// out vec2 modelVec;

uniform mat4 combined;
// uniform mat4 model;

uniform vec4 texOffsetScale;

void main(void) 
{
    // modelVec = (model * vec4(posCoord, 1.0f)).xy;
    gl_Position = combined * vec4(posCoord, 1.0f);
    clrCoordOut = clrCoord;
    texCoordOut = vec2(texCoord.x / texOffsetScale.z + texOffsetScale.x,
                       texCoord.y / texOffsetScale.w + texOffsetScale.y);
}