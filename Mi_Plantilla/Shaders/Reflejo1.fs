#version 330 core
out vec4 FragColor;
  
  in vec2 texCoord;

  uniform sampler2D textura;
  
void main()
{
    FragColor =  texture(textura,texCoord) * vec4(.1,.1,.1,1.0);
}