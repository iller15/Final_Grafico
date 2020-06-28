#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ichiGl.hpp"

using namespace std;


int main() {
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };
 

    
    //Objeto* algo = new Objeto(vertices);
    Controladora* controladora = new Controladora();
    controladora->correr();
    
 	return 0;
}