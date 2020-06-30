#ifndef __ICHIGL_HPP__
#define __ICHIGL_HPP__


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stb_image.h>



#define Ancho 2340
#define Largo 1080

using namespace std;

void re_size(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

class Ventana {
	GLFWwindow* ventana;
public:
	Ventana() {
	}
	~Ventana() {
		glfwTerminate();
	}
	GLFWwindow* getVentana() {
		return this->ventana;
	}
	void instanciando() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
	GLFWwindow* creandoVentana(const char* titulo,int ancho,int alto) {
		return glfwCreateWindow(ancho, alto, titulo, NULL, NULL);
	}
	
	void cerrarVentana(GLFWwindow* window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}
	bool verificandoErrores() {
		if (this->ventana == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return true;
		}
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return true;
		}
	}
	void inicializar(){
		this->instanciando();
		this->ventana = this->creandoVentana("Trabajo Final", Ancho, Largo);
		glfwMakeContextCurrent(this->ventana);
		if (this->verificandoErrores()) {
			return;
		}
		glViewport(0, 0, Ancho, Largo);
		glfwSetFramebufferSizeCallback(this->ventana, re_size);

	} 

};
class ProgramShaders{
	GLuint shaderProgram;
public:
	ProgramShaders(const char* rutaVertex, const char* rutaFragment) {
		string codigoVertex, codigoFragment;
		ifstream archivoVertex;
		ifstream archivoFragment;
		//para mostrar excepciones
		archivoVertex.exceptions(ifstream::failbit | ifstream::badbit);
		archivoFragment.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			archivoVertex.open(rutaVertex);
			archivoFragment.open(rutaFragment);
			//las variables son las que recibiran la info desde el archivo
			stringstream streamVertex, streamFragment;
			streamVertex << archivoVertex.rdbuf();
			streamFragment << archivoFragment.rdbuf();
			
			archivoVertex.close();
			archivoFragment.close();
			//ahora paso la informacion al string;
			codigoVertex = streamVertex.str();
			codigoFragment = streamFragment.str();
		}
		catch (ifstream::failure&) { cout << "hubo un error al intentar abrir los archivos" << endl; }

		//compilando el shader vertex;
		const char* origenVertex = codigoVertex.c_str();
		const char* origenFragment = codigoFragment.c_str();

		//creando los id de los shaders;
		GLuint idVertex, idFragment;

		idVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(idVertex, 1, &origenVertex, nullptr);
		glCompileShader(idVertex);
		verificandoErrores(idVertex, "Vertex");

		idFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(idFragment, 1, &origenFragment, NULL);
		glCompileShader(idFragment);
		verificandoErrores(idFragment, "Fragment");

		this->shaderProgram = glCreateProgram();
		glAttachShader(this->shaderProgram,idVertex);
		glAttachShader(this->shaderProgram, idFragment);
		glLinkProgram(this->shaderProgram);


		glDeleteShader(idFragment);
		glDeleteShader(idVertex);

	}
	~ProgramShaders() {
		glDeleteProgram(shaderProgram);
	}
	
	GLuint getProgram() {
		return this->shaderProgram;
	}
	void usar() {
		glUseProgram(this->shaderProgram);
	}
	bool verificandoErrores(GLint idShader, string tipo) {
		int  success;
		char infoLog[512];
		if (tipo == "Vertex" || tipo == "Fragment") {

			glGetShaderiv(idShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(idShader, 512, NULL, infoLog);
				cout << "ERROR::SHADER::" << tipo << "::COMPILATION_FAILED" << endl << infoLog << endl;
				return true;
			}
			else
				return false;
		}
		else {
			glGetProgramiv(idShader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(idShader, 512, NULL, infoLog);
				cout << "ERROR::PROGRAM::SHADER::COMPILATION_FAILED" << endl << infoLog << endl;
				return true;
			}
		}
	
	}
};
class Objeto {
	unsigned int VBO,VAO,EBO,textura;
	string vertexShader, fragmentShader;
	ProgramShaders* program;
	unsigned char* data_imagen;
	class Imagen {
	public:
		int width, height, nrChannels;
		unsigned char* data_imagen;
		Imagen(string ubicacionImagen) {
			stbi_set_flip_vertically_on_load(true);
			//cout << data_imagen << endl;
			this->data_imagen = stbi_load(ubicacionImagen.c_str(), &width, &height, &nrChannels, 0);
		}
	};
	Imagen *imagen;
	float coordenas[8] = {
		1.0f, 1.0f,   // top-right corner
		1.0f, 0.0f,  // lower-right corner	
		0.0f, 0.0f,  // lower-left corner  
		0.0f, 1.0f  // top-right corner
	};
	float vertices[32] = {
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};	
	unsigned int indices[6] = {  // note that we start from 0!
	  0, 1, 3,  // first Triangle
	  1, 2, 3   // second Triangle
	};

public:

	Objeto(string vertex, string fragment) {
		program = new ProgramShaders(vertex.c_str(), fragment.c_str());
		this->imagen = new Imagen("Texturas/once_Punch_Horizontal.jpg");
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
		glGenTextures(1, &this->textura);
		this->bindVAO();
		this->inicializarVertices();
		this->setAtributo(0,3,8,0);
		this->setAtributo(1, 3, 8, 3);
		this->setAtributo(2, 2, 8, 6);
		//orden importante, activar textura despues de su atributo;
		this->bindTextura();

		this->inicializarImgaen();


	}
	~Objeto() {
		glDeleteVertexArrays(1,&this->VAO);
		glDeleteBuffers(1, &this->VBO);
		glDeleteBuffers(1, &this->EBO);
	}
	ProgramShaders* getProgram() {
		return this->program;
	}
	void inicializarVertices() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices), indices, GL_STATIC_DRAW);
	}
	void setAtributo(short posEnArreglo, short cantDatosAtrib,short stride, short offset) {					
		// ver las imagenes de opengl como referencia: https://learnopengl.com/Getting-started/Shaders
		//stride: cant de bytes entre el mismo tipo de atributos (aqui los ponemos en cantidad de floats y luego lo multiplicamos por su tama�o)
		//offset: cant de bytes desde el inicio del vertex al atributo (aqui los ponemos en cantidad de floats y luego lo multiplicamos por su tama�o)
		glVertexAttribPointer(posEnArreglo, cantDatosAtrib, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset*sizeof(float)));
		glEnableVertexAttribArray(posEnArreglo);
	}
	void bindVAO() {
		glBindVertexArray(this->VAO);
	}
	void bindTextura() {
		glBindTexture(GL_TEXTURE_2D, this->textura);
	}
	void dandoColor() {
		//obtenemos el tiempo (desde que comenzo el programa?)
		float time = glfwGetTime();
		//funcion para que redValue siempre este entre 0 y 1;
		float redValue = (sin(time) / 2.0f) + 0.5f;
		//obteniendo la ubicacion del uniform que queremos;
		int colorVertxLocation = glGetUniformLocation(program->getProgram(), "color");
		//verificando que nos haya dado una locacion;
		if (colorVertxLocation == -1) { cout << "ERROR:: No se pudo encontrar el uniform" << endl; }
		//pasando el nuevo valor al uniform; IMPORTANTE: activar (useProgram) al shader program en el que se encuentra el uniform;
		glUniform4f(colorVertxLocation, 0.f, redValue*0.5, redValue, 1);
	}
	void setUniform(string uniform, short dataLength, float *info) {
		GLint uniformLoc = glGetUniformLocation(this->program->getProgram(), uniform.c_str());
		if (uniformLoc == -1) { cout << "ERROR:: No se pudo encontrar el uniform" << endl; }
	}
	void inicializarImgaen() {
		this->bordeImagen();
		if (this->imagen->data_imagen)
		{
			//glBindTexture(GL_TEXTURE_2D, this->textura);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imagen->width, this->imagen->height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->imagen->data_imagen);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(this->imagen->data_imagen);
	}
	void bordeImagen() {
		//Se indica que en tanto el eje x=s como y=t, se use el metodo mirrored_Repeat para llenar lo que falte para encajar la imagen en el objeto;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//Se especifica que metodo usar al agrandar o disminuir la imagen;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //solo se suna mipmap al minimizar la imagen, si se pone en agrandar la imagen no abra efecto y dara error
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
};
class Controladora {
	Ventana *ventana;
	Objeto *cuadrado;
public:
	Controladora() {
		ventana = new Ventana();
		ventana->inicializar();
		this->cuadrado = new Objeto("Shaders/VertexShader.vs", "Shaders/FragmentShader.fs");
	}
	~Controladora() {
		delete this->ventana;
	}

	void correr() {
		while (!glfwWindowShouldClose(ventana->getVentana()))
		{

			//inputs de teclado
			ventana->cerrarVentana(ventana->getVentana());
			//rendering commands
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			this->cuadrado->getProgram()->usar();
			//this->cuadrado->dandoColor();
			this->cuadrado->bindTextura();
			cuadrado->bindVAO();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			//checkear y llamar eventos ademas de resize del buffer
			glfwSwapBuffers(ventana->getVentana());
			glfwPollEvents();
		}

	}


};


#endif