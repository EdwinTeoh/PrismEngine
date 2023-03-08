#version 330 core

in vec2 texCoordOut;

layout (location = 0) out vec4 o_Color;

uniform bool useTexture;
uniform vec4 u_Color;
uniform sampler2D tex2d;
uniform float alpha;

void main()
{
    if (useTexture)
    {
        o_Color = vec4(1.0f, 1.0f, 1.0f, alpha) * texture2D(tex2d, texCoordOut);
    }
    else
    {
        o_Color = u_Color;
    }
}