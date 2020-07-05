#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoords;



out vec3 normal;
out vec3 posFrag;
out vec2 TextCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{   
    posFrag = vec3(model * vec4(aPos, 1.0f));
    normal = mat3(transpose(inverse(model))) * aNormal;  
    TextCoords = aTextCoords;
    


    //considerar que gl_Position es una variable de openGL y es vec4
    gl_Position =  projection * view *vec4(posFrag, 1.0f);
 
    
}