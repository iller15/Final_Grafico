#version 330 core
out vec4 FragColor;
  
  struct Material {
      vec3 ambiente;
      vec3 diffusa;
      vec3 especular;
      float shininess;
  }; 
    
  uniform Material material;
  
  

  in vec3 normal;
  in vec3 posFrag;

  uniform vec3 posLuz;
  uniform vec3 colorModelo;
  uniform vec3 posCamara;
  
void main()
{
    //constante que depende del material;
    float fuerzaEspecular = 0.5;
    //constante de valor iluminacion ambiental;
    float fuerzaAmbiental= 0.1;
    vec3 iluAmbiental = colorModelo * fuerzaAmbiental;

    vec3 norm = normalize(normal);
    vec3 dirLuz = normalize(posLuz- posFrag);  

    vec3 dirVista = normalize(posCamara- posFrag);
    vec3 dirReflejo = reflect(-dirLuz,norm);

    //calculando la luz difusa;
    float diff = max(dot(norm, dirLuz), 0.0);

    //calculando luz especular
    // el 32 es un valor de luminosidad, mientras mayor se vera más marcado el el punto brilloso en el objeto (trendra un radio mas notorio y mas pequeño)
    float spec =  pow(max(dot(dirVista,dirReflejo),0),32);
    vec3 especular = spec * colorModelo * fuerzaEspecular;

    //aqui probablemente deberis ahaer que entre el color de la luz que le cae
    vec3 diffuse = diff * vec3(1.0,1.0,1.0);
    //debo mandar su propio color 
    vec3 colorFinal = colorModelo * (diffuse + iluAmbiental+ especular);

    FragColor = vec4(colorFinal,1.0);
}