#version 330 core

uniform sampler2D texture1;
uniform float opacity;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(texture1, texCoord).xyz, opacity);
} 
