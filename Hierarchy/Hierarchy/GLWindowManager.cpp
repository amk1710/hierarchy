#include <string>
#include <iostream>
#include <fstream>
#include <istream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <time.h>
#include <unordered_map>

#include "lodepng.h"

#include <random>


//#include <tiny_obj_loader.h>

#include "GLWindowManager.h"
#include "RenderObject.h"



using namespace std;

//one callback definition(could not put it inside the class for some reason)
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void GLWindowManager::processInput(GLFWwindow *window)
{
	//habilita usuário a fechar a janela apertando escape
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		eye.x -= 0.01;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		eye.x += 0.01;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		eye.z += 0.01;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		eye.z -= 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		eye.y += 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		eye.y -= 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		//tex3active = !tex3active;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		angle += 0.001f;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		angle -= 0.001f;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		incNLights = min(incNLights + 0.1f, 32.0f);
		nLights = int(incNLights);
		cout << "nLights: " << nLights << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		incNLights = max(incNLights - 0.1f, 0.0f);
		nLights = int(incNLights);
		cout << "nLights: " << nLights << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		usingDebug = true;
		cout << "Using Debug: true" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
	{
		usingDebug = false;
		cout << "Using Debug: false" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		selectTexture = 1;
		cout << "selectTexture: Position" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		selectTexture = 2;
		cout << "selectTexture: Bump normal" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		selectTexture = 3;
		cout << "selectTexture: color" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		selectTexture = 4;
		cout << "selectTexture: tangent" << endl;
	}

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		selectTexture = 5;
		cout << "selectTexture: bitangent" << endl;
	}




}

std::string GLWindowManager::readShaderFile(const char* name)
{
	std::ifstream in(name);
	std::string shader;

	if (in.fail())
	{
		//aborta
		cerr << "IO error opening file on vertex.vert. Aborting" << std::endl;
		exit(1);
	}

	char a;
	while (in.get(a) && a != EOF)
	{
		shader += a;
	}
	shader += '\0';
	in.close();
	return shader;
}

void GLWindowManager::BuildShader(const char* filepath, unsigned int* shaderID, int shader_enum)
{
	std::string shaderString = readShaderFile(filepath);

	//cria objeto shader
	*shaderID = glCreateShader(shader_enum);
	//attach shader source code to the shader object and compile the shader
	const char *c_str = shaderString.c_str(); // glShaderSource espera no 3o param um array de c-like strings. Nesse caso, só temos uma string nesse array. No 2o param está explícita esta informação
	glShaderSource(*shaderID, 1, &c_str, NULL);
	glCompileShader(*shaderID);

	//checks for errors:
	int success;
	char infoLog[512];
	glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(*shaderID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX/FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		exit(1);
	}
}

	//updates model-view-projection matrix based on current values for the eye, scale etc.
void GLWindowManager::UpdateMVPMatrix()
{
	//construct model, view and projection matrices:

	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(eye, cameraTarget, up);

}

GLWindowManager::GLWindowManager()
{

	
}

GLWindowManager::~GLWindowManager()
{

	
}

void GLWindowManager::randomPointInSphere(float *x, float *y, float *z, float radius)
{
	//https://math.stackexchange.com/questions/1585975/how-to-generate-random-points-on-a-sphere

	
}


void GLWindowManager::InitializeSceneInfo()
{
	// reference: https://learnopengl.com/Getting-started/Hello-Window, https://learnopengl.com/Getting-started/Hello-Triangle

	usingDebug = false;

	//initialize opengl
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//create glfw window
	
	window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	
	
	//ask GLAD to get opengl function pointers for us
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}

	//set viewport's size
	glViewport(0, 0, screenWidth, screenHeight);
	//set window's resize callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //talvez seja melhor nem ter isso, já que não to atualizando direito as demais coisas

	//configure global opengl state
	//enables depth testing
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders ------------------

	//shaders do G pass
	unsigned int vertexShaderG;
	BuildShader("Gvertex.vert", &vertexShaderG, GL_VERTEX_SHADER);
	
	unsigned int fragmentShaderG;
	BuildShader("Gfragment.frag", &fragmentShaderG, GL_FRAGMENT_SHADER);

	// SHADER PROGRAM G PASS
	int success;
	char infoLog[512];

	gPass = glCreateProgram();
	//attach shader to program
	glAttachShader(gPass, vertexShaderG);
	glAttachShader(gPass, fragmentShaderG);
	glLinkProgram(gPass);

	//check for linking errors:
	glGetProgramiv(gPass, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(gPass, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::LINKING_ERROR\n" << infoLog << std::endl;
	}
	glUseProgram(gPass);



	//deleta os objetos shader, que já cumpriram sua função
	glDeleteShader(vertexShaderG);
	glDeleteShader(fragmentShaderG);

	// END: build and compile shaders

	//LOAD OBJECTS

	RenderObject obj = RenderObject();
	obj.LoadObjectFromPath("golfball/golfball.obj");
	obj.Initialize();
	objects.push_back(obj);
	

	//default values for eye, camera target and up. 
	eye = glm::vec3(0.0f, 0.0f, 10.0f);
	cameraTarget = glm::vec3(0.0, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	//values for up to 4 lights
	int maxLights = 4;
	nLights = 4;
	
	lights = { 5,5,5,    0,0,1,   0.5f,0.5f,1.5f };
	lightsColors = { 0.5f, 0.5f, 0.5f,	1,1,1,	1,1,1,	1,1,1 };

	//END: LOAD MODELS

	cout << gPass << endl;
}

// reference: https://learnopengl.com/Getting-started/Hello-Window, https://learnopengl.com/Getting-started/Hello-Triangle
	
//will start the render loop and finish executing only when the window is closed
void GLWindowManager::StartRenderLoop()
{
	//the render loop
	cout << gPass << endl;
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//clear pixels
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(gPass);

		//atualiza matrizes view e projection
		UpdateMVPMatrix();
		//passa as matrizes v,p pra placa
		int pParam = glGetUniformLocation(gPass, "p");
		glUniformMatrix4fv(pParam, 1, GL_FALSE, glm::value_ptr(Projection));
		int vParam = glGetUniformLocation(gPass, "v");
		glUniformMatrix4fv(vParam, 1, GL_FALSE, glm::value_ptr(View));

		//passa eye, light positions e nLights
		int eyeParam = glGetUniformLocation(gPass, "eye");
		glUniform3f(eyeParam, eye.x, eye.y, eye.z);

		int nLightsParam = glGetUniformLocation(gPass, "nLights");
		glUniform1i(nLightsParam, nLights);

		int lightParam = glGetUniformLocation(gPass, "lightPositions");
		glUniform3fv(lightParam, 4, &lights[0]);

		/*for (RenderObject obj : objects)
		{
			obj.Render(gPass);
		}*/

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i].Render(gPass);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "window closed" << std::endl;
	glfwTerminate();
	return;
}
