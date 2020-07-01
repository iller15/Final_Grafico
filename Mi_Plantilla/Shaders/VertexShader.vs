#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{   
    //considerar que gl_Position es una variable de openGL y es vec4
    gl_Position =  projection * view * model * vec4(aPos, 1.0f);
    color = aColor;
    texCoord = aTexCoord;
}