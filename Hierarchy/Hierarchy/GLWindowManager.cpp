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
#include "BVHNode.h"



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

	//move olho
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

	//move camera target
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		cameraTarget.x -= 0.01;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		cameraTarget.x += 0.01;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		cameraTarget.z += 0.01;
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		cameraTarget.z -= 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		cameraTarget.y += 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		cameraTarget.y -= 0.01f;
	}


	//rotaciona objeto?
	/*if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		angle += 0.001f;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		angle -= 0.001f;
	}*/

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

	//adicionei aqui este cálculo dos planos, que ajuda no cálculo do frustum culling. Posso depois deslocar isso para a classe da árvore

	//planes_normals[i] -> a normal, no espaço do mundo, do i-ésimo plano
	//planes_points[i] -> um ponto no i-ésimo plano que define sua distância até a origem

	//glm::mat4 vp_inv = glm::inverse(Projection * View);

	//glm::vec3 def_planes[6] { glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, -1) }; //em ordem: right, left, front, back, top, bottom
	////curiosamente, não preciso de def_points pq eles são iguais em valor ao def_planes
	//for (int i = 0; i < 6; i++)
	//{
	//	glm::vec4 temp_normal = vp_inv * glm::vec4(def_planes[i], 1.0f); //transformo, 
	//	temp_normal = temp_normal / temp_normal.w; //e divido para voltar pro espaço euclidiano

	//	planes_normals[i] = glm::vec3(temp_normal);

	//	//transformo o ponto( o cálculo é o mesmo!)
	//	planes_points[i] = planes_normals[i];

	//	//normalizo normal?
	//	planes_normals[i] = glm::normalize(planes_normals[i]);

	//	
	//}

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

	//instancia uma "matriz" de tres dimensões de objetos
	int n_side = 3; // NxNxN
	float offset = 5.0f; // a distância entre dois objs
	int startOffset = -offset * ((n_side - 1) / 2);

	//sample obj
	RenderObject *sample = new RenderObject();
	sample->LoadObjectFromPath("golfball/golfball.obj");

	for (int i = 0; i < n_side; i++)
	{
		for (int j = 0; j < n_side; j++)
		{
			for (int k = 0; k < n_side; k++)
			{
				RenderObject *obj = new RenderObject(*sample);
				//obj.LoadObjectFromPath("golfball/golfball.obj");
				obj->positionX = startOffset + i * offset;
				obj->positionY = startOffset + j * offset;
				obj->positionZ = startOffset + k * offset;
				obj->Initialize();
				objects.push_back(obj);

			}
		}
	}

	delete sample;

	//organiza todos os objs criados numa árvore de hierarquia. Para fazer a deleção posteriormente, também faço por essa árvore
	HierarchyRoot = RenderObject::ConstructHierarchy(objects);

	/*RenderObject obj = RenderObject();
	obj.LoadObjectFromPath("golfball/golfball.obj");
	obj.Initialize();
	obj.positionX = 5.0f;
	obj.positionY = 5.0f;
	obj.positionZ = 5.0f;*/

	//objects.push_back(obj);

	

	//default values for eye, camera target and up. 
	eye = glm::vec3(0.0f, 0.0f, 10.0f);
	cameraTarget = glm::vec3(0.0, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	//values for up to 4 lights
	int maxLights = 4;
	nLights = 1;
	
	lights = { 10,10,10,    0,0,1,   0.5f,0.5f,1.5f };
	lightsColors = { 0.5f, 0.5f, 0.5f,	1,1,1,	1,1,1,	1,1,1 };

	//END: LOAD MODELS

	cout << gPass << endl;
}

// reference: https://learnopengl.com/Getting-started/Hello-Window, https://learnopengl.com/Getting-started/Hello-Triangle
	
//will start the render loop and finish executing only when the window is closed
void GLWindowManager::StartRenderLoop()
{
	//the render loop
	
	lastTime = glfwGetTime();
	int nbFrames = 0;
	int count_obj = 0;
	while (!glfwWindowShouldClose(window))
	{
		//counts seconds per frame
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) //se já passou um segundo,
		{
			//printo também qtd de objs renderizados
			cout << count_obj << "object(s) rendered" << endl;
			//printo spf e reseto
			cout << 1000.0 / double(nbFrames) << "ms/frame" << endl;
			nbFrames = 0;
			lastTime += 1.0f;
			cout << "------------------" << endl;
		}
		
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

		count_obj = 0; //o número de objs renderizados
		//for (int i = 0; i < objects.size(); i++)
		//{
		//	/*FrustumCheck r = objects[i].IsInsideFrustum(Projection * View);
		//	objects[i].Render(gPass);
		//	cout << "Object " << i << ": " << r << endl;
		//	*/
		//	count_obj += objects[i]->CheckFrustumAndRender(gPass, Projection * View); //depois, devo armazenar o valor dessa multiplica p*v, pra não repetir a conta tantas vezes
		//}
		count_obj = HierarchyRoot->CheckFrustumAndRender(gPass, Projection * View);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "window closed" << std::endl;
	HierarchyRoot->FreeNode(); //libera memória da árvore de hierarquia
	glfwTerminate();
	return;
}
