#version 330 core
out vec4 FragColor;

  struct Material {
      sampler2D difusa;
      sampler2D especular;
      float     shininess;
  }; 

  struct Light {
      vec3 posLuz;
    
      vec3 ambiente;
      //difusa es el color de la luz que queremos;  
      vec3 difusa;
      vec3 especular;
  };
  
  in vec3 posFrag;
  in vec3 normal;
  in vec2 TextCoords;
  

  uniform vec3 posCamara;
  uniform Material material;
  uniform Light luz; 

  //quitar
  
void main()
{
    
    //iluminacion ambiental;
    vec3 Ambiental = luz.ambiente * texture(material.difusa,TextCoords).rgb;

    //iluminacion difusa;
    vec3 norm = normalize(normal);
    vec3 dirLuz = normalize(luz.posLuz- posFrag);  
    float diff = max(dot(norm, dirLuz), 0.0);
    vec3 difusa = luz.difusa * diff * texture(material.difusa,TextCoords).rgb;

    //calculando luz especular
    vec3 dirVista = normalize(posCamara- posFrag);
    vec3 dirReflejo = reflect(-dirLuz,norm);
    // el 32 es un valor de luminosidad, mientras mayor se vera más marcado el el punto brilloso en el objeto (trendra un radio mas notorio y mas pequeño)
    float spec =  pow(max(dot(dirVista,dirReflejo),0),material.shininess);
    vec3 especular = luz.especular * spec * texture(material.especular,TextCoords).rgb;

    vec3 colorFinal = (Ambiental + difusa + especular);

    FragColor = vec4(colorFinal,1.0);
}