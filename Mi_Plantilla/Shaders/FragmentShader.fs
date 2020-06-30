#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D textura;

void main()
{
    FragColor = texture(textura,texCoord);
};