#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

class RenderObject
{

private:

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<uint32_t> indices;

	glm::mat4 model;

	//coisas da textura
	int width;
	int height;
	int nrChannels;
	unsigned char *data;
	unsigned int tex1;
	unsigned int tex2;

	void LoadModel(const char* objName, bool randomColors);
	unsigned int LoadTexture(char const * path);

public: 
	
	RenderObject();
	~RenderObject();

	//posição, escala e rotação do objeto
	float positionX, positionY, positionZ;
	float scale;
	float angle;

	void LoadObjectFromPath(char const * path);
	void Initialize();
	void Render(unsigned int shaderID);

};
