#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

//enumeração retornada pela função IsInsideFrustum
enum FrustumCheck {INSIDE, OUTSIDE, INTERSECT};

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texcoord;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	bool operator==(const Vertex& other) const {
		return position == other.position && color == other.color && texcoord == other.texcoord; //não exigimos que as normais, tangentes e bitangentes sejam iguais
	}

};

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

	//coisas para a bounding box
	glm::vec3 Bmin;
	glm::vec3 Bmax;
	glm::vec3 LUT[8];
	std::vector<Vertex> aux_vertices; //inicializado quando loada o modelo, útil para construir a caixa
	void ConstructBoundingBox();
	

	void LoadModel(const char* objName, bool randomColors);
	unsigned int LoadTexture(char const * path);

public:

	//recebe como argumento as seis normais que definem os planos do frustum, no espaço do mundo,
	//e os seis pontos que definem o deslocamento do plano em relação à origem
	FrustumCheck IsInsideFrustum(glm::mat4 ViewProjection);


//public:
	
	RenderObject();
	~RenderObject();

	//posição, escala e rotação do objeto
	float positionX, positionY, positionZ;
	float scale;
	float angle;


	void CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection);
	void LoadObjectFromPath(char const * path);
	void Initialize();
	void Render(unsigned int shaderID);

};
