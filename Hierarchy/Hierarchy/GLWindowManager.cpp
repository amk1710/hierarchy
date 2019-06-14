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


#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLWindowManager.h"



using namespace std;

void GLWindowManager::FBO_2_file()
{
	FILE    *output_image;
	int     output_width, output_height;

	output_width = screenWidth;
	output_height = screenHeight;

	/// READ THE PIXELS VALUES from FBO AND SAVE TO A FILE
	int             i, j, k;
	unsigned char   *pixels = (unsigned char*)malloc(output_width*output_height * 4);

	/// READ THE CONTENT FROM THE FBO
	//glPixelStorei(GL_PACK_ALIGNMENT, 1); ??
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, output_width, output_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	//Encode the image
	unsigned error = lodepng::encode("output.png", pixels, width, height);

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	free(pixels);
}

//one callback definition(could not put it inside the class for some reason)
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int GLWindowManager::loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);

		cout << width << "," << height << endl;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


void GLWindowManager::processInput(GLFWwindow *window)
{
	//habilita usu�rio a fechar a janela apertando escape
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
		tex3active = !tex3active;
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
	const char *c_str = shaderString.c_str(); // glShaderSource espera no 3o param um array de c-like strings. Nesse caso, s� temos uma string nesse array. No 2o param est� expl�cita esta informa��o
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

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	glm::mat4 View = glm::lookAt(eye, cameraTarget, up);

	model = glm::scale(glm::mat4(1.0f), glm::vec3(scale)) * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));


	
	view = View;
	modelView = View * model;
	modelViewProjection = Projection * View * model;

	//a inversa transposta da modelView
	//primeiro inverte e depois tranp�e?
	ITmodelView = glm::transpose(glm::inverse(modelView));
}

GLWindowManager::GLWindowManager()
{
	IsTextureLoaded = false;
	IsBumpmapLoaded = false;
	data = NULL;
	bump_data = NULL;
	
}

GLWindowManager::~GLWindowManager()
{
	//libera mem�ria, se != null
	if (data != NULL)
	{
		stbi_image_free(data);
	}

	if (bump_data != NULL)
	{
		stbi_image_free(bump_data);
	}
	
}

//pra usar a unordered map abaixo, precisamos implementar o == e uma fun��o hash:
struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texcoord;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	bool operator==(const Vertex& other) const {
		return position == other.position && color == other.color && texcoord == other.texcoord; //n�o exigimos que as normais, tangentes e bitangentes sejam iguais
	}

};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return	((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texcoord) << 1);
		}
	};
}




void GLWindowManager::LoadModel(const char* objName, bool randomColors = false)
{
	std::string warn, err;

	srand(17);

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objName)) {
		cout << warn + err << endl;
		throw std::runtime_error(warn + err);
	}

	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<int> number_of_faces;

	std::unordered_map<Vertex, uint32_t> unique_vertices = {};
	std::vector<Vertex> aux_vertices = {};
	indices.clear();
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			//o tinyobj loader garante que s�o todos triangulos
			glm::vec3 pos = glm::vec3(attrib.vertices[index.vertex_index * 3], attrib.vertices[index.vertex_index * 3 + 1], attrib.vertices[index.vertex_index * 3 + 2]);
			glm::vec3 color = glm::vec3(attrib.colors[index.vertex_index*3], attrib.colors[index.vertex_index*3 + 1], attrib.colors[index.vertex_index*3 + 2]);
			glm::vec3 normal = glm::vec3(attrib.normals[index.normal_index*3], attrib.normals[index.normal_index*3 + 1], attrib.normals[index.normal_index*3 + 2]);
			glm::vec2 texcoord = glm::vec2(attrib.texcoords[index.texcoord_index*2], attrib.texcoords[index.texcoord_index*2 + 1]);
			Vertex vertex = {
				//pos, color, normal, texcoord, tangents[index.vertex_index], bitangents[index.vertex_index]  //tangente e bitangente foram calculados acima
				pos, color, normal, texcoord, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)   //tangente e bitangente s�o calculados abaixo
			};

			if (unique_vertices.count(vertex) == 0)
			{
				//se n�o h� nenhum v�rtice como este at� agora, o adiciona a lista de vertices �nicos
				unique_vertices[vertex] = static_cast<uint32_t>(aux_vertices.size());
				aux_vertices.push_back(vertex);
			}

			indices.push_back(unique_vertices[vertex]);
		}
	}


	number_of_faces.resize(aux_vertices.size());
	//calcula, para cada tri�ngulo, a tangente e a bitangente
	//ps: += tres para ir percorrendo a cada tr�s v�rtices, ou seja, um tri�ngulo. � certo que v�rtices ser�o percorridos mais de uma vez, por�m conto com o resultado n�o ser discrepante o suficiente para impactar
	for (int i = 0; i < indices.size(); i += 3)
	{
		//adaptado de: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/

		//cout << "triangle:" << i << endl;
		// Shortcuts for vertices
		Vertex v0 = aux_vertices[indices[i + 0]];
		Vertex v1 = aux_vertices[indices[i + 1]];
		Vertex v2 = aux_vertices[indices[i + 2]];

		//edges of the triangle: position delta
		glm::vec3 deltaPos1 = v1.position - v0.position;
		glm::vec3 deltaPos2 = v2.position - v0.position;

		// UV delta
		glm::vec2 deltaUV1 = v1.texcoord - v0.texcoord;
		glm::vec2 deltaUV2 = v2.texcoord - v0.texcoord;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

		//adiciona valores para as m�dias de cada vertice
		number_of_faces[indices[i]] += 1;
		number_of_faces[indices[i + 1]] += 1;
		number_of_faces[indices[i + 2]] += 1;

		aux_vertices[indices[i]].tangent += tangent;
		aux_vertices[indices[i + 1]].tangent += tangent;
		aux_vertices[indices[i + 2]].tangent += tangent;

		aux_vertices[indices[i]].bitangent += bitangent;
		aux_vertices[indices[i + 1]].bitangent += bitangent;
		aux_vertices[indices[i + 2]].bitangent += bitangent;

	}

	//para cada tangente e bitangente, tira a media e normaliza
	for (int i = 0; i < aux_vertices.size(); i++)
	{
		//cout << "vertex: " << i << ", number of faces: " << number_of_faces[i] << endl;

		aux_vertices[i].tangent = glm::vec3(aux_vertices[i].tangent.x / number_of_faces[i], aux_vertices[i].tangent.y / number_of_faces[i], aux_vertices[i].tangent.z / number_of_faces[i]);
		aux_vertices[i].bitangent = glm::vec3(aux_vertices[i].bitangent.x / number_of_faces[i], aux_vertices[i].bitangent.y / number_of_faces[i], aux_vertices[i].bitangent.z / number_of_faces[i]);

		glm::normalize(aux_vertices[i].tangent);
		glm::normalize(aux_vertices[i].bitangent);

	}


	//simply dump aux_vertices into vertex buffer:
	//preenche vbo:
	vertices.clear();
	for (int i = 0; i < aux_vertices.size(); i++)
	{
		vertices.push_back(aux_vertices[i].position.x); 
		vertices.push_back(aux_vertices[i].position.y); 
		vertices.push_back(aux_vertices[i].position.z);
		
		//colors
		if (randomColors)
		{
			vertices.push_back((static_cast <float> (rand() / 2.0) / static_cast <float> (RAND_MAX)));
			vertices.push_back((static_cast <float> (rand() / 2.0) / static_cast <float> (RAND_MAX)));
			vertices.push_back((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
		}
		else
		{
			vertices.push_back(aux_vertices[i].color.x);
			vertices.push_back(aux_vertices[i].color.y);
			vertices.push_back(aux_vertices[i].color.z);
		}
		//normals
		vertices.push_back(aux_vertices[i].normal.x);
		vertices.push_back(aux_vertices[i].normal.y);
		vertices.push_back(aux_vertices[i].normal.z);

		//texcoords
		vertices.push_back(aux_vertices[i].texcoord.x);
		vertices.push_back(aux_vertices[i].texcoord.y);


		// bi/tangents, pre calculadas acima
		vertices.push_back(aux_vertices[i].tangent.x);
		vertices.push_back(aux_vertices[i].tangent.y);
		vertices.push_back(aux_vertices[i].tangent.z);

		vertices.push_back(aux_vertices[i].bitangent.x);
		vertices.push_back(aux_vertices[i].bitangent.y);
		vertices.push_back(aux_vertices[i].bitangent.z);

	}


	//tem mais coordenadas de textura do que v�rtices!

	printf("# of vertices infos  = %d\n", (int)(vertices.size()));
	printf("# of vertices in vector aux  = %d\n", (int)(aux_vertices.size()));
	printf("# of triangles  = %d\n", (int)(indices.size()) / 3);
	//printf("# of triangles v2 = %d\n", (int)(tangents.size()));
	printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
	printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
	printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
	printf("# of materials = %d\n", (int)materials.size());
	printf("# of shapes    = %d\n", (int)shapes.size());


}

void GLWindowManager::LoadTexture(const char* filepath)
{
	data = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if (data)
	{
		IsTextureLoaded = true;

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	
}

void GLWindowManager::LoadBumpmap(const char* filepath)
{
	bump_data = stbi_load(filepath, &width_bm, &height_bm, &nrChannels_bm, 0);
	if (bump_data)
	{
		IsBumpmapLoaded = true;
		
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //talvez seja melhor nem ter isso, j� que n�o to atualizando direto as demais coisas(texturas do g pass etc)

	//configure global opengl state
	//enables depth testing
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders ------------------

	//shaders do G pass
	unsigned int vertexShaderG;
	BuildShader("Gvertex.vert", &vertexShaderG, GL_VERTEX_SHADER);
	
	unsigned int fragmentShaderG;
	BuildShader("Gfragment.frag", &fragmentShaderG, GL_FRAGMENT_SHADER);

	//shader do L pass
	unsigned int vertexShaderL;
	BuildShader("Lvertex.vert", &vertexShaderL, GL_VERTEX_SHADER);

	unsigned int fragmentShaderL;
	BuildShader("Lfragment.frag", &fragmentShaderL, GL_FRAGMENT_SHADER);

	//shader Debug
	unsigned int DebugL;
	BuildShader("debugShader.frag", &DebugL, GL_FRAGMENT_SHADER);

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
	//glUseProgram(gPass);

	// SHADER PROGRAM L PASS

	lPass = glCreateProgram();
	//attach shader to program
	glAttachShader(lPass, vertexShaderL);
	glAttachShader(lPass, fragmentShaderL);
	glLinkProgram(lPass);

	//check for linking errors:
	glGetProgramiv(lPass, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(lPass, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::LINKING_ERROR\n" << infoLog << std::endl;
	}
	//glUseProgram(gPass);

	//SHADER PROGRAM DEBUG L PASS
	dPass = glCreateProgram();
	//attach shader to program
	glAttachShader(dPass, vertexShaderL);
	glAttachShader(dPass, DebugL);
	glLinkProgram(dPass);

	//check for linking errors:
	glGetProgramiv(dPass, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(dPass, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::LINKING_ERROR\n" << infoLog << std::endl;
	}



	//deleta os objetos shader, que j� cumpriram sua fun��o
	glDeleteShader(vertexShaderG);
	glDeleteShader(fragmentShaderG);
	glDeleteShader(vertexShaderL);
	glDeleteShader(fragmentShaderL);

	// END: build and compile shaders

	//LOAD MODELS

	//default values for eye, camera target and up. 
	scale = 1.0f;
	angle = 0.0f;
	eye = glm::vec3(0.0f, 0.0f, 10.0f);
	cameraTarget = glm::vec3(0.0, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);


	
	//values for up to 32 lights
	int maxLights = 32;
	nLights = 32; //o numero de luzes a ser realmente usado
	incNLights = 32.0f;
	srand(17); //set random seed
	//normal distribution
	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.0, 12.0);
	float radius = 100.0f;
	for (int i = 0; i < maxLights; i++)
	{
		//gera tr�s pontos aletorios numa esfera de raio r
		float x, y, z;
		do
		{
			x = distribution(generator);
			y = distribution(generator);
			z = distribution(generator);
		} while (x == 0.0f && y == 0.0f && z == 0.0f); //previne os tr�s de darem 0, o que quebraria a conta a seguir

		//normaliza
		float f = 1 / (x*x + y*y + z*z);
		x *= f;
		y *= f;
		z *= f;
		
		lights[3 * i] = x * radius;
		lights[3 * i + 1] = y * radius;
		lights[3 * i + 2] = z * radius;

		float reductionFactor = i >= 8 ? 0.1f : 1.0f;
		reductionFactor = i == 8 ? 0 : reductionFactor;

		//sorteia cores da luz
		lightsColors[3 * i] = reductionFactor * static_cast <float> ((rand()) / static_cast <float> (RAND_MAX));
		lightsColors[3 * i + 1] = reductionFactor * static_cast <float> ((rand()) / static_cast <float> (RAND_MAX));
		lightsColors[3 * i + 2] = reductionFactor * static_cast <float> ((rand()) / static_cast <float> (RAND_MAX));


		cout << lights[3 * i] << ", " << lights[3 * i + 1] << ", " << lights[3 * i + 2] << endl;

	}
	
	//lights = { 5,5,5,    0,0,1,   0.5f,0.5f,1.5f };
	//lightsColors = { 0.5f, 0.5f, 0.5f,	1,1,1,	1,1,1,	1,1,1 };

	//END: LOAD MODELS

	////cria objeto VAO para guardar o que vai ser configurado abaixo:
	glGenVertexArrays(1, &VAO);
	//bind VAO
	glBindVertexArray(VAO);

	//cria buffer object:
	//vertex buffer object: 
	//(por si s� � usado para desenhar tri�ngulos especificando somente v�rtices consecutivos, o que pode causar v�rtices especificados m�ltiplas vezes)
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//aqui est�o todas as infos: de posi��o do v�rtice, cor e normais. Depois defino l� em baixo como � a navega��o por essas infos (glVertexAttribPointer)
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), static_cast<void*>(vertices.data()), GL_STATIC_DRAW);


	//element buffer object:
	//nesse caso, usamos um element buffer object pra poder informar a ordem de desenhar os tri�ngulos, e n�o os v�rtices m�ltiplas vezes
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), static_cast<void*>(indices.data()), GL_STATIC_DRAW);


	//reference: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
	//cria gBuffer framebuffer object, usado para a primeira passada (geometry pass) no algoritmo de deferred shading
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	////texturas para o output da computa��o do geometry pass:

	//position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0); //GL_COLOR_ATTACHMENT0

	//normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0); //GL_COLOR_ATTACHMENT1

	//color + specular color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0); //GL_COLOR_ATTACHMENT2

	//tangent buffer
	glGenTextures(1, &gTangent);
	glBindTexture(GL_TEXTURE_2D, gTangent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gTangent, 0); //GL_COLOR_ATTACHMENT3

	//bitangent buffer. Talvez n�o precise, dado que bitange pode ser calcula tendo-se a normal e a tangente, usando produto vetorial
	glGenTextures(1, &gBitangent);
	glBindTexture(GL_TEXTURE_2D, gBitangent);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gBitangent, 0); //GL_COLOR_ATTACHMENT4

	// - tell openGl which color attachments we'll use (of this framebuffer) for rendering
	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 , GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	//create and attach depth buffer(render buffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind

	glActiveTexture(GL_TEXTURE0);
	//tex1 = loadTexture("stones/stones.jpg");
	tex1 = loadTexture("golfball/white.png");
	glActiveTexture(GL_TEXTURE1);
	//tex2 = loadTexture("stones/stones_norm.jpg");
	tex2 = loadTexture("golfball/golfball.png");
	tex3 = loadTexture("golfball/Test-Pattern.jpg");
	
	IsTextureLoaded = true;
	IsBumpmapLoaded = true;
	tex3active = false;

	//o buffer � composto de: 
	//coordenadas(3) + cores(3) + normais(3) + coord_texturas(2) + tangentes(3) + bitangente(3)

	////inform openGL about our vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//inform openGL about our color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	//inform openGL about our normal attributes
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//inform openGL about our texture coordinate attributes
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	//inform openGL about our tangent attributes
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	//inform openGL about our bi-tangent attributes
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(14 * sizeof(float)));
	glEnableVertexAttribArray(5);


	//unbind VAO
	glBindVertexArray(0);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clears color and depth testing buffers

		//gBuffer render:
		{
			glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(gPass);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex2);

			glBindVertexArray(VAO); //bind

			//atualiza matriz model-view-projection
			UpdateMVPMatrix();
			//passa as matrizes mvp pra placa
			int mvpParam = glGetUniformLocation(gPass, "mvp");
			glUniformMatrix4fv(mvpParam, 1, GL_FALSE, glm::value_ptr(modelViewProjection));
			int mvParam = glGetUniformLocation(gPass, "mv");
			glUniformMatrix4fv(mvParam, 1, GL_FALSE, glm::value_ptr(modelView));
			int vParam = glGetUniformLocation(gPass, "v");
			glUniformMatrix4fv(vParam, 1, GL_FALSE, glm::value_ptr(view));

			//� uma mat3?
			int ITmvParam = glGetUniformLocation(gPass, "ITmv");
			glUniformMatrix4fv(ITmvParam, 1, GL_FALSE, glm::value_ptr(ITmodelView));

			//passa eye, light positions e nLights
			int eyeParam = glGetUniformLocation(gPass, "eye");
			glUniform3f(eyeParam, eye.x, eye.y, eye.z);

			//passa texturas:

			glUniform1i(glGetUniformLocation(gPass, "texture_data"), 0);
			glUniform1i(glGetUniformLocation(gPass, "bumpmap"), 1);

			//glUniform3f(glGetUniformLocation(gPass, "Ka"), materials[0].ambient[0], materials[0].ambient[1], materials[0].ambient[2]);
			//glUniform3f(glGetUniformLocation(gPass, "Ks"), materials[0].specular[0], materials[0].specular[1], materials[0].specular[2]);
			//glUniform1f(glGetUniformLocation(gPass, "Mshi"), 128.0f);

			glActiveTexture(GL_TEXTURE0);
			if (tex3active)
			{
				glBindTexture(GL_TEXTURE_2D, tex3);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, tex1);
			}
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex2);
			
			

			//devo passar indices.size, que � a qtd de indices usados (3 para cada triangulo)
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			glBindVertexArray(0); //unbind

			glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind
		}

		//lighting pass:
		{
			if (usingDebug)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(dPass);

				glUniform1i(glGetUniformLocation(dPass, "gPosition"), 0);
				glUniform1i(glGetUniformLocation(dPass, "gNormal"), 1);
				glUniform1i(glGetUniformLocation(dPass, "gColorSpec"), 2);
				glUniform1i(glGetUniformLocation(dPass, "gTangent"), 3);
				glUniform1i(glGetUniformLocation(dPass, "gBitangent"), 4);

				int selectParam = glGetUniformLocation(dPass, "selectTexture");
				glUniform1i(selectParam, selectTexture);

				//passa resultados do geometry pass como texturas
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gPosition);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, gNormal);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, gColorSpec);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, gTangent);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, gBitangent);

			}
			else
			{

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glUseProgram(lPass);

				glUniform1i(glGetUniformLocation(lPass, "gPosition"), 0);
				glUniform1i(glGetUniformLocation(lPass, "gNormal"), 1);
				glUniform1i(glGetUniformLocation(lPass, "gColorSpec"), 2);
				glUniform1i(glGetUniformLocation(lPass, "gTangent"), 3);
				glUniform1i(glGetUniformLocation(lPass, "gBitangent"), 4);

				int eyeParam = glGetUniformLocation(lPass, "eye");
				glUniform3f(eyeParam, eye.x, eye.y, eye.z);

				int nLightsParam = glGetUniformLocation(lPass, "nLights");
				glUniform1i(nLightsParam, nLights);

				int lightParam = glGetUniformLocation(lPass, "lightPositions");
				glUniform3fv(lightParam, 32, &lights[0]);

				int lightColorParam = glGetUniformLocation(lPass, "lightColors");
				glUniform3fv(lightColorParam, 32, &lightsColors[0]);

				glUniform3f(glGetUniformLocation(lPass, "Ka"), 0.5f, 0.5f, 0.5f);
				glUniform3f(glGetUniformLocation(lPass, "Ks"), 1.0f, 1.0f, 1.0f);
				glUniform1f(glGetUniformLocation(lPass, "Mshi"), 10000.0f);

				int mParam = glGetUniformLocation(lPass, "m");
				glUniformMatrix4fv(mParam, 1, GL_FALSE, glm::value_ptr(model));

				//passa resultados do geometry pass como texturas
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gPosition);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, gNormal);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, gColorSpec);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, gTangent);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, gBitangent);
			}

			//render a quad, with the textures
			renderQuad();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "window closed" << std::endl;
	glfwTerminate();
	return;
}


void GLWindowManager::renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
