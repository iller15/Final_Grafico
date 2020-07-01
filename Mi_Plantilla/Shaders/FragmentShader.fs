#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D textura;

void main()
{
    //FragColor es una variable de opengl para el color y es vec4
    FragColor = texture(textura,texCoord);
};