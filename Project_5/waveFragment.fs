#version 330 core

in vec2 TexCoords;
in vec4 color;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    FragColor = texColor * color; 
}