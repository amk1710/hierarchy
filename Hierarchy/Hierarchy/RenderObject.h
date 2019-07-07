#pragma once

#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <tiny_obj_loader.h>

#include "BVHNode.h"

using namespace std;

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

class RenderObject : public BVHNode
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
	/*glm::vec3 Bmin;
	glm::vec3 Bmax;
	glm::vec3 LUT[8];*/
	std::vector<Vertex> aux_vertices; //inicializado quando loada o modelo, útil para construir a caixa
	void ConstructBoundingBox();
	
	//aux: funcção recursiva
	static BVHNode* RenderObject::ConstructHierarchy_aux(std::vector<BVHNode*> objects, int s, int e, int depth); //as posições de começo e fim da lista, para fazer a recursão

	void LoadModel(const char* objName, bool randomColors);
	unsigned int LoadTexture(char const * path);

public:

	//recebe como argumento as seis normais que definem os planos do frustum, no espaço do mundo,
	//e os seis pontos que definem o deslocamento do plano em relação à origem
	FrustumCheck IsInsideFrustum(glm::mat4 ViewProjection);


//public:
	
	RenderObject();
	~RenderObject();
	//RenderObject(const RenderObject& rhs);

	//posição, escala e rotação do objeto
	float positionX, positionY, positionZ;
	float scale;
	float angle;


	//void CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection);
	void LoadObjectFromPath(char const * path);
	void Initialize();
	int Render(unsigned int shaderID);
	int CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection);

	void FreeNode();

	//constrói hierarquia de objetos, a partir de uma lista de objs concretos renderizáveis
	//se baseia nas subclasses aggregator node e renderobject
	//ps: talvez o mais correto fosse não deixar essas funções aqui, mas tá ok por agora
	static BVHNode* ConstructHierarchy(std::vector<BVHNode*> objects);

	glm::vec3 GetBmin();
	glm::vec3 GetBmax();

	void PrintHierarchy(ofstream& myfile, FrustumCheck check, glm::mat4 ViewProjection, int tabulation);


};
