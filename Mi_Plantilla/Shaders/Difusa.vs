#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 posFrag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{   
    normal = mat3(transpose(inverse(model))) * aNormal;  

    //considerar que gl_Position es una variable de openGL y es vec4
    gl_Position =  projection * view * model * vec4(aPos, 1.0f);
    posFrag = vec3(model * vec4(aPos, 1.0f));
 
    
}