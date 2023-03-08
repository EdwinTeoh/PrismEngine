#version 330 core

in vec3 clrCoordOut;
in vec2 texCoordOut;
in vec2 modelVec;

layout (location=0) out vec4 color;

uniform bool useTexture;
uniform sampler2D tex2d;
uniform vec4 colorModifier;
// uniform vec4 lightPosition;
// uniform float lightSize;

void main(void) 
{
    color = vec4(clrCoordOut, 1.0f) * colorModifier;
    if (useTexture)
    {
        // color = texture2D(tex2d, texCoordOut) * vec4(colorModifier.xyz * lightSize / distance(vec2(lightPosition.x, lightPosition.y), modelVec), colorModifier.w);
        color = texture2D(tex2d, texCoordOut) * colorModifier;
    }
}