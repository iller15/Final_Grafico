#ifndef __ICHIGL_HPP__
#define __ICHIGL_HPP__


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define Ancho 2340
#define Alto 1080

using namespace std;
using namespace glm;

void re_size(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_llamado(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void inputTeclado(GLFWwindow* window);

//F no queira poner variables globales asi pero welp
double lastX = Ancho/2.0f, lastY = Alto/2.0f;
bool go = true;
enum Tecla {
	ADELANTE,
	ATRAS,
	IZQUIEDA,
	DERECHA
};

struct Vertex {
	int cantDatos, posicion, offset;
	Vertex(short cantDatos, short posicion, short offset): cantDatos(cantDatos),posicion(posicion),offset(offset) {}
	};
class Camara {
	vec3 posCamara = vec3(0.f, 0.f, 3.f);
	 //por ahora
	vec3 targetCamara = vec3(0.f, 0.f, 0.f);
	//el orden es importante para que la camara apunte al lado negativo de z (mirar el eje coordenado de View coordinates https://learnopengl.com/Getting-started/Camera) 
	vec3 direccionCam = normalize(posCamara - targetCamara);
	//usamos un vector que apunte hacie arriba en Worldcoords, despues hace cross con la vista para sacar la perpendicular a estos para sacar el camaraRigth;
	vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	//el orden es importante para que el vector resultante sea hacie +x
	vec3 rightCamara = cross(direccionCam, up);
	//para coger el vector que apunte encima de la camara
	vec3 upCamara = cross(direccionCam, rightCamara);
	//es el vector que apunta a menos Z (el frente de la cmara independiente de su direccion);
	vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	//Creando el view matrix, el cual se debera aplicar a todos los objetos;
	mat4 view;
	float deltaTime = 0.0f, lastFrame = 0.0f, yaw = -90, pitch = 0, fov = 45;
	float sensibilidad = 0.01f;

public:
	Camara() {
		//la funcion lookAt nos hace la matriz automaticamente; El orden es:
		//  posCamara
		//  targertCamara ( si es estatico ) target + posCamara permite que la camara se mueva siempre mirando al frente
		//  up  (este es un vector que apunta hacia arriba en las World Coordinates [no se si se escreibe asi y flojera revisar jsjs]);

		//temporal
		view = lookAt(this->posCamara,
					  posCamara + cameraFront,
					  this->upCamara);
		updateCameraVectors();
	}
	~Camara() {
	}
	vec3 getPosCamara() {
		return this->posCamara;
	}
	void setPosCamara(vec3 posCamara) {
		this->posCamara = posCamara;
	}
	mat4 getView() {
		view = lookAt(this->posCamara,
			posCamara+cameraFront,
			this->upCamara);
		return this->view;
	}
	void setView(mat4 view) {
		this->view = view;
	}
	vec3 getDireccion() {
		return this->direccionCam;
	}
	void setDireccion(vec3 direccion) {
		this->direccionCam = direccion;
	}
	vec3 getUpCamara() {
		return this->upCamara;
	}
	void setUpCamara(vec3 upCam) {
		this->upCamara = upCam;
	}
	void actualizarTime() {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
	float getDeltaTime() {
		return  this->deltaTime;
	}
	vec3 getCameraFront() {
		return this->cameraFront;
	}
	void procesarTecla(Tecla direction, float deltaTime)
	{
		float velocity = 2.5 * deltaTime;
		if (direction == ADELANTE)
			posCamara += cameraFront * velocity;
		if (direction == ATRAS)
			posCamara -= cameraFront * velocity;
		if (direction == IZQUIEDA)
			posCamara -= rightCamara * velocity;
		if (direction == DERECHA)
			posCamara += rightCamara * velocity;
	}
	void mirar(float cambioX, float cambioY) {
		
		cambioX*= sensibilidad;
		cambioY*= sensibilidad;
		yaw += cambioX;
		pitch += cambioY;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		updateCameraVectors();
		
	}
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		cameraFront= glm::normalize(front);
		rightCamara = glm::normalize(cross(cameraFront, up));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		upCamara= glm::normalize(cross(rightCamara, cameraFront));
	}
	void zoomMouse(float yoffset)
	{
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 45.0f)
			fov = 45.0f;
	}
	void setFov(float fov) {
		this->fov = fov;
	}
	float getFov() {
		return this->fov;
	}
};
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
		this->ventana = this->creandoVentana("Trabajo Final", Ancho, Alto);
		glfwMakeContextCurrent(this->ventana);
		glfwSetCursorPosCallback(getVentana(), mouse_llamado);
		glfwSetScrollCallback(getVentana(), scroll_callback);

		if (this->verificandoErrores()) {
			return;
		}
		glViewport(0, 0, Ancho, Alto);
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
	void setInt(const std::string& name, int value)
	{
		GLuint id = glGetUniformLocation(this->getProgram(), name.c_str());
		if (id == -1) { cout << "ERROR:: No se pudo encontrar el uniform: "<< name << endl; }
		glUniform1i(glGetUniformLocation(this->getProgram(), name.c_str()), value);
	}
	void setMat4(const std::string& name, const glm::mat4& mat)
	{
		GLuint id = glGetUniformLocation(this->getProgram(), name.c_str());
		if (id == -1) { cout << "ERROR:: No se pudo encontrar el uniform: " << name << endl; }
		glUniformMatrix4fv(id, 1, GL_FALSE, &mat[0][0]);
	}
};
class Objeto {
	class Modelo{
		mat4  transformada = mat4(1.0f), camara = mat4(1.0f), projectionRecivido =mat4(1.0f);
		ProgramShaders* program;
	public:
		Modelo(mat4 modelo, mat4 view, mat4 projection,ProgramShaders* program):transformada(modelo), camara(view),projectionRecivido(projection), program(program){}
		Modelo() {}
		void mandarMVP() {
			mat4 view = mat4(1.0f);
			
			program->setMat4("modelo",transformada);
			program->setMat4("view", view);
		}
		
		mat4 getTransformada() {
			return this->transformada;
		}
		void setTransformfada(mat4 trans) {
			this->transformada = trans;
		}
		mat4 getCamara() {
			return this->camara;
		}
		void setCamara(mat4 view) {
			this->camara = view;
		}
		
	};
	class Imagen {
	public:
		int width, height, nrChannels;
		char tipo[5];
		unsigned char* data_imagen;
		Imagen(string ubicacionImagen) {
			//para obtner el tipo de imagen
			ubicacionImagen.copy(tipo, ubicacionImagen.length() - ubicacionImagen.find("."), ubicacionImagen.find(".")+1);
			stbi_set_flip_vertically_on_load(true);
			this->data_imagen = stbi_load(ubicacionImagen.c_str(), &width, &height, &nrChannels, 0);
		}
	};
	GLuint VBO, VAO, EBO, textura;
	short nModelos, nAtributos, stride = 0;
	bool luz;
	string vertexShader, fragmentShader;
	ProgramShaders* program;
	unsigned char* data_imagen;

	//	 model servira para mover el objeto, view probablemente servira para mover la escena;
	vector<Modelo>* modelos;
	Imagen *imagen;
	vector<Vertex>* attribVertex;

	float coordenas[8] = {
		1.0f, 1.0f,   // top-right corner
		1.0f, 0.0f,  // lower-right corner	
		0.0f, 0.0f,  // lower-left corner  
		0.0f, 1.0f  // top-right corner
	};
	float vertices[180] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	unsigned int indices[6] = {  // note that we start from 0!
	  0, 1, 3,  // first Triangle
	  1, 2, 3   // second Triangle
	};

public:

	Objeto(string vertex, string fragment, string textura, short Modelos, short atributos,vector<Vertex>* attribVertex, bool luz = false): nModelos(Modelos), nAtributos(atributos), attribVertex(attribVertex){
		//this->projection = mat4(1.0f);
		program = new ProgramShaders(vertex.c_str(), fragment.c_str());
		this->program->usar();
		this->modelos = new vector<Modelo>;
		for (short i = 0; i < this->nModelos; i++)
			this->modelos->push_back(Modelo());
		
		if(!luz) this->imagen = new Imagen(textura);

		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		//glGenBuffers(1, &this->EBO);
		this->bindVAO();
		this->inicializarBuffers();
		for (short i = 0; i < this->attribVertex->size(); i++) {
			this->stride += attribVertex->at(i).cantDatos;
		}
		this->setAtributos();
		/*this->setAtributo(0,3,5,0);
		this->setAtributo(1, 2, 5, 3);*/
		//this->setAtributo(1, 3, 6, 3);

		//orden importante, activar textura despues de su atributo;
		this->program->usar();
		if (!luz) {
			glGenTextures(1, &this->textura);
			this->bindTextura();
			this->inicializarImgaen(this->imagen->tipo);
			this->program->setInt("textura", 0);
		}

		
		//this->iniciarProjection();

	}
	~Objeto() {
		glDeleteVertexArrays(1,&this->VAO);
		glDeleteBuffers(1, &this->VBO);
		glDeleteBuffers(1, &this->EBO);
		this->modelos->clear();
		delete modelos;
	}
	ProgramShaders* getProgram() {
		return this->program;
	}
	void inicializarBuffers() {
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices), indices, GL_STATIC_DRAW);
	}
	void setAtributo(short posEnArreglo, short cantDatosAtrib,short stride, short offset) {					
		// ver las imagenes de opengl como referencia: https://learnopengl.com/Getting-started/Shaders
		//stride: cant de bytes entre el mismo tipo de atributos (aqui los ponemos en cantidad de floats y luego lo multiplicamos por su tamaño)
		//offset: cant de bytes desde el inicio del vertex al atributo (aqui los ponemos en cantidad de floats y luego lo multiplicamos por su tamaño)
		glVertexAttribPointer(posEnArreglo, cantDatosAtrib, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset*sizeof(float)));
		glEnableVertexAttribArray(posEnArreglo);
	}
	void setAtributos() {
		
		for (short i = 0; i < this->attribVertex->size(); i++) {
			Vertex atributoVertex = attribVertex->at(i);
			setAtributo(atributoVertex.posicion,atributoVertex.cantDatos,stride,atributoVertex.offset);
		}

	}
	void bindVAO() {
		glBindVertexArray(this->VAO);
	}
	void bindTextura() {
		glBindTexture(GL_TEXTURE_2D, this->textura);
	}
	void activarTextura() {
		glActiveTexture(GL_TEXTURE0);
	}
	//PRUEBAS DE LEARNOPENGL
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
	void volteando() {
		GLuint trans;
		//Creando matriz identidad (diagonal de ceros)
		mat4 transformada = mat4(1.0f);
		//para rotar le pasamos los parametro (matriz a transformar,angulo que queremos girar,en que eje queremos girar (entre 0 y 1 los valores));
		//transformada = scale(transformada, vec3(0.5, 0.5, 0.5));
		transformada = rotate(transformada, radians(-55.f), vec3(1.0f, 0.0f, 0.0));
		this->setUniformMatrixfv(trans, transformada, "trans");
	}
	void primer3d() {
		program->usar();
		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		
		
		transMatrix(model, "Rotacion", vec3(0.5f, 1.0f, 0.0f),true, (float)glfwGetTime());
		//transMatrix(model, "Escalar", vec3(.5f, 0.5f, 0.5f));
		transMatrix(view, "Traslacion", vec3(.0f, .0f, -3.0f));
		
		
		//projection = perspective(radians(45.0f), (float)(Ancho / Alto), 0.1f, 100.0f);

		this->program->setMat4("model", model);
		this->program->setMat4("view", view);

	}
	// */
	void agregarModelo(vec3 mov) {

		//probablemnte aquie deberia ir un for para todos los cambios que quiera hacerle a las matrices del modelo, eso cuando tenga un documento de donde leer jsjs;
		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);
		this->modelos->push_back(Modelo(model, view, projection, program));
	}
	void trasladarObject(vec3 data, mat4 model) {
		transMatrix(model, "Traslacion", data);
		this->program->setMat4("model", model);
	}
	void rotarObject(vec3 data, mat4 model, bool time, float angulo = 1) {
		if (time) {
			transMatrix(model, "Rotacion", data, true, glfwGetTime() * angulo);
		}
		else {
			transMatrix(model, "Rotacion", data, false, angulo);
		}
		this->program->setMat4("model", model);
	}
	void transMatrix(mat4 &matrix,string tipo, vec3 data,bool radianes = true, float angulo = 0) {
		//para transformar una matrix en el aspecto que queramos
		if (tipo == "Traslacion") {
			matrix = translate(matrix, data);
		}
		else if (tipo == "Rotacion") {
			if(radianes)
				matrix = rotate(matrix,	 (angulo), data);
			else
				matrix = rotate(matrix, radians(angulo), data);
		}
		else {
			matrix = scale(matrix, data);
		}

	}
	void getUniform(GLuint &idUniform,string uniform) {
		idUniform= glGetUniformLocation(this->program->getProgram(), uniform.c_str());
		if (idUniform == -1) { cout << "ERROR:: No se pudo encontrar el uniform" << endl;  }
		
	}
	void setUniformMatrixfv(GLuint &idUniform, mat4 transformada, string uniform) {
		this->getUniform(idUniform, uniform);
		//				idUniform; matrices a mandar; transponer matrix; la matriz en si ( la funcion es para que opegnGl lo acepte ya que viene desde glm
		glUniformMatrix4fv(idUniform, 1, GL_FALSE, value_ptr(transformada));
	}
	void inicializarImgaen(string tipo) {
		this->bordeImagen();
		if (this->imagen->data_imagen)
		{
			if (tipo == "jpg") {
				//glBindTexture(GL_TEXTURE_2D, this->textura);
											//Como queremos guardar la data;						 Como llega la data;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imagen->width, this->imagen->height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->imagen->data_imagen);
			}
			else
			{                                  //por ahora se quedra en RGB y no en RGBA
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->imagen->width, this->imagen->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->imagen->data_imagen);
			}
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //solo se suna mipmap al minimizar la imagen, si se pone en agrandar la imagen no abra efecto y dara error
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	short getNModelos() {
		return this->nModelos;
	}
	vector<Modelo>* getModelos() {
		return this->modelos;
	}
	void setProjection(ProgramShaders* program) {

	}
	//temporal
	void temporalSetView(mat4 view = mat4(1.0f)) {
		transMatrix(view, "Traslacion", vec3(.0f, .0f, -3.0f));
		this->program->setMat4("view", view);
	}
	void setView(mat4 view = mat4(1.0f)) {
		this->program->setMat4("view", view);
	}

};
class Controladora {
	Ventana *ventana;
	vector<Objeto*> *objetos;
	Camara* camara;
	//temporal
	float vertices[180] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float* caja = vertices;
	vec3 cubePositions[10] = {
		vec3(0.0f,  0.0f,  0.0f),
		vec3(2.0f,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3(2.4f, -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f, -2.0f, -2.5f),
		vec3(1.5f,  2.0f, -2.5f),
		vec3(1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};

public:
	Controladora() {
		this->objetos = new vector<Objeto*>;
		ventana = new Ventana();
		ventana->inicializar();
		glEnable(GL_DEPTH_TEST);

		//para que desaparezca el mouse;
		glfwSetInputMode(ventana->getVentana(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		vector<Vertex>* atributos = new vector<Vertex>;
		atributos->push_back(Vertex(3, 0, 0));
		atributos->push_back(Vertex(2, 1, 3));

		vector<Vertex>* atributos2 = new vector<Vertex>;
		atributos2->push_back(Vertex(3, 0, 0));
		atributos2->push_back(Vertex(2, 1, 3));

		//aqui voy a tener que leer otro archivo
		this->objetos->push_back(new Objeto("Shaders/VertexShader.vs", "Shaders/FragmentShader.fs", "Texturas/catlul.png",10,2,atributos));
		this->objetos->push_back(new Objeto("Shaders/VertexShader.vs", "Shaders/FragmentShader.fs", "Texturas/once_Punch_Horizontal.jpg", 1, 2,atributos2));

		this->camara = new Camara();
	}
	~Controladora() {
		delete this->ventana;
	}

	void correr() {
		while (!glfwWindowShouldClose(ventana->getVentana()))
		{
			//cogiendo tiempos
			this->camara->actualizarTime();


			//inputs de teclado
			inputTeclado(ventana->getVentana());
			//rendering commands
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (short i = 0; i < this->objetos->size(); i++) {
				this->objetos->at(i)->activarTextura();
				this->objetos->at(i)->bindTextura();

				this->objetos->at(i)->getProgram()->usar();

				//this->cuadrado->primer3d();
				//this->cuadrado->bindTextura();
				//esto debera estar en una funcion
				//this->cuadrado->setProjection();
				mat4 projection = mat4(1.0f);
				projection = perspective(radians(this->camara->getFov()), (float)(Ancho / Alto), 0.1f, 100.0f);
				this->objetos->at(i)->getProgram()->setMat4("projection", projection);

				mat4 view = this->camara->getView();
				//deberia hacer set para cada objeto diferente;
				this->objetos->at(i)->setView(view);

				objetos->at(i)->bindVAO();
				for (short e = 0; e < this->objetos->at(i)->getNModelos(); e++) {
					mat4 model = mat4(1.0f);
					if (i == 1) {
						this->objetos->at(i)->transMatrix(model, "Traslacion", vec3(0.0f, 1.0f, 0.0f));
					}
					else {
						this->objetos->at(i)->transMatrix(model, "Traslacion", this->cubePositions[e]);
						this->objetos->at(i)->transMatrix(model, "Rotacion", vec3(1.0f, 0.3f, .5f), false, e * glfwGetTime());
					}
					this->objetos->at(i)->getProgram()->setMat4("model", model);
					//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
			//checkear y llamar eventos ademas de resize del buffer
			glfwSwapBuffers(ventana->getVentana());
			glfwPollEvents();
		}

	}
	Camara* getCamara() {
		return  this->camara;
	}

};

Controladora* controladora = new Controladora();
void inputTeclado(GLFWwindow* window)
{
	const float delta = controladora->getCamara()->getDeltaTime(); // adjust accordingly

	//Cerrar pagina
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else {
		//Movimiento Camara
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)						
			controladora->getCamara()->procesarTecla(ADELANTE, delta);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)						
			controladora->getCamara()->procesarTecla(ATRAS, delta);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)						
			controladora->getCamara()->procesarTecla(IZQUIEDA, delta);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)						
			controladora->getCamara()->procesarTecla(DERECHA, delta);
		
	}
}
void mouse_llamado(GLFWwindow* window, double xpos, double ypos) {
	if (go)
	{
		lastX = xpos;
		lastY = ypos;
		go = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top

	lastX = xpos;
	lastY = ypos;
	controladora->getCamara()->mirar(xoffset,yoffset);
	}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	controladora->getCamara()->zoomMouse(yoffset);
}

#endif