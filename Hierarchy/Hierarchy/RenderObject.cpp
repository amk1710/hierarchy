
using namespace std;

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
#include <limits>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "BVHNode.h"
#include "RenderObject.h"

//constructor and destructor
//pretty useless right now
RenderObject::RenderObject()
{
	positionX = 0.0f;
	positionY = 0.0f;
	positionZ = 0.0f;
	scale = 1.0f;
}
RenderObject::~RenderObject()
{

}

//auxilary functions and structs


void RenderObject::CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection)
{
	return;
}

// MODEL LOADING FUNCTIONALITY

//pra usar a unordered map abaixo, precisamos implementar o == e uma função hash:

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return	((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texcoord) << 1);
		}
	};
}


void RenderObject::LoadModel(const char* objName, bool randomColors = false)
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
	aux_vertices = {};
	indices.clear();
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			//o tinyobj loader garante que são todos triangulos
			glm::vec3 pos = glm::vec3(attrib.vertices[index.vertex_index * 3], attrib.vertices[index.vertex_index * 3 + 1], attrib.vertices[index.vertex_index * 3 + 2]);
			glm::vec3 color = glm::vec3(attrib.colors[index.vertex_index * 3], attrib.colors[index.vertex_index * 3 + 1], attrib.colors[index.vertex_index * 3 + 2]);
			glm::vec3 normal = glm::vec3(attrib.normals[index.normal_index * 3], attrib.normals[index.normal_index * 3 + 1], attrib.normals[index.normal_index * 3 + 2]);
			glm::vec2 texcoord = glm::vec2(attrib.texcoords[index.texcoord_index * 2], attrib.texcoords[index.texcoord_index * 2 + 1]);
			Vertex vertex = {
				//pos, color, normal, texcoord, tangents[index.vertex_index], bitangents[index.vertex_index]  //tangente e bitangente foram calculados acima
				pos, color, normal, texcoord, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)   //tangente e bitangente são calculados abaixo
			};

			if (unique_vertices.count(vertex) == 0)
			{
				//se não há nenhum vértice como este até agora, o adiciona a lista de vertices únicos
				unique_vertices[vertex] = static_cast<uint32_t>(aux_vertices.size());
				aux_vertices.push_back(vertex);
			}

			indices.push_back(unique_vertices[vertex]);
		}
	}


	number_of_faces.resize(aux_vertices.size());
	//calcula, para cada triângulo, a tangente e a bitangente
	//ps: += tres para ir percorrendo a cada três vértices, ou seja, um triângulo. É certo que vértices serão percorridos mais de uma vez, porém conto com o resultado não ser discrepante o suficiente para impactar
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

		//adiciona valores para as médias de cada vertice
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


	//tem mais coordenadas de textura do que vértices!

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

FrustumCheck RenderObject::IsInsideFrustum(glm::mat4 ViewProjection)
{
	return BVHNode::IsInsideFrustum(ViewProjection * model);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int RenderObject::LoadTexture(char const * path)
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

void RenderObject::LoadObjectFromPath(char const * path)
{
	LoadModel(path, false);

	glActiveTexture(GL_TEXTURE0);
	//tex1 = loadTexture("stones/stones.jpg");
	tex1 = LoadTexture("golfball/white.png");
	//tex1 = loadTexture("golfball/Test-Pattern.jpg");
	glActiveTexture(GL_TEXTURE1);
	//tex2 = loadTexture("stones/stones_norm.jpg");
	tex2 = LoadTexture("golfball/golfball.png");

	ConstructBoundingBox();

}

//auxiliar para construir uma bounding box
void RenderObject::ConstructBoundingBox()
{
	float min = std::numeric_limits<float>::min();
	float max = std::numeric_limits<float>::max();
	Bmin = glm::vec3(max, max, max);
	Bmax = glm::vec3(min, min, min);
	for (Vertex vertex : aux_vertices)
	{
		for (int i = 0; i < 3; i++)
		{
			if (vertex.position[i] < Bmin[i])
			{
				Bmin[i] = vertex.position[i];
			}
			if (vertex.position[i] > Bmax[i])
			{
				Bmax[i] = vertex.position[i];
			}
		}
	}

	//na prática, 3,3,3 acabou funcionando melhor por causa de erro numérico, creio eu
	//Bmin = glm::vec3(-3.0f, -3.0f, -3.0f);
	//Bmax = glm::vec3(3.0f, 3.0f, 3.0f);


	//com Bmin e Bmax, constrói LUT para acesso rápido de Vmin, Vmax
	LUT[0] = glm::vec3(Bmin.x, Bmin.y, Bmin.z); // 0,0,0 -> min, min min
	LUT[1] = glm::vec3(Bmin.x, Bmin.y, Bmax.z); // 0,0,1 -> min, min, max
	LUT[2] = glm::vec3(Bmin.x, Bmax.y, Bmin.z); // 0,1,0 -> min,max,min
	LUT[3] = glm::vec3(Bmin.x, Bmax.y, Bmax.z); // 0,1,1 -> min,max,max
	LUT[4] = glm::vec3(Bmax.x, Bmin.y, Bmin.z); // 1,0,0 -> max,min,min
	LUT[5] = glm::vec3(Bmax.x, Bmin.y, Bmax.z); // 1,0,1 -> max,min,max
	LUT[6] = glm::vec3(Bmax.x, Bmax.y, Bmin.z); // 1,1,0 -> max,max,min
	LUT[7] = glm::vec3(Bmax.x, Bmax.y, Bmax.z); // 1,1,1 -> max,max,max




	return;
}

void ConstructBoundingCircle()
{

}

void RenderObject::Initialize()
{
	////cria objeto VAO para guardar o que vai ser configurado abaixo:
	glGenVertexArrays(1, &VAO);
	//bind VAO
	glBindVertexArray(VAO);

	//cria buffer object:
	//vertex buffer object: 
	//(por si só é usado para desenhar triângulos especificando somente vértices consecutivos, o que pode causar vértices especificados múltiplas vezes)
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//aqui estão todas as infos: de posição do vértice, cor e normais. Depois defino lá em baixo como é a navegação por essas infos (glVertexAttribPointer)
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), static_cast<void*>(vertices.data()), GL_STATIC_DRAW);


	//element buffer object:
	//nesse caso, usamos um element buffer object pra poder informar a ordem de desenhar os triângulos, e não os vértices múltiplas vezes
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), static_cast<void*>(indices.data()), GL_STATIC_DRAW);


	//o buffer é composto de: 
	//coordenadas(3) + cores(3) + normais(3) + coord_texturas(2) + tangentes(3) + bitangente(3)

	////inform openGL about our vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//inform openGL about our color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(3 * sizeof(float)));
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
}

//considera que várias globais já foram definidas, como:
//luzes
//matrizes v e p
//shader a ser usado (já está binded)
//clears
void RenderObject::Render(unsigned int shaderID)
{

	glBindVertexArray(VAO); //bind

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);

	//passa a model deste objeto pra placa
	//quero rotacionar/escalar primeiro, depois transladar
	model = glm::scale(glm::mat4(1.0f), glm::vec3(scale)) * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(positionX, positionY, positionZ));
	
	
	int mParam = glGetUniformLocation(shaderID, "m");
	glUniformMatrix4fv(mParam, 1, GL_FALSE, glm::value_ptr(model));

	//passa texturas:
	glUniform1i(glGetUniformLocation(shaderID, "texture_data"), 0);
	glUniform1i(glGetUniformLocation(shaderID, "bumpmap"), 1);

	//glUniform3f(glGetUniformLocation(gPass, "Ka"), materials[0].ambient[0], materials[0].ambient[1], materials[0].ambient[2]);
	//glUniform3f(glGetUniformLocation(gPass, "Ks"), materials[0].specular[0], materials[0].specular[1], materials[0].specular[2]);
	//glUniform1f(glGetUniformLocation(gPass, "Mshi"), 128.0f);

	glUniform3f(glGetUniformLocation(shaderID, "Ka"), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(shaderID, "Ks"), 0.3f, 0.3f, 0.3f);
	glUniform1f(glGetUniformLocation(shaderID, "Mshi"), 100.0f);

	//devo passar indices.size, que é a qtd de indices usados (3 para cada triangulo)
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	
	glBindVertexArray(0); //unbind

}



////(x,y,z) é um ponto, então (x,y,z) está na região "dentro" do plano <----> x*planes.x + y*planes.y + z*planes.z + planes.w > 0
//
////transforma os pontos Bmin e Bmax para o espaço NPC
//
//glm::mat4 mvp = ViewProjection * model;
//glm::vec4 ObjMinimum = mvp * glm::vec4(Bmin, 1.0f);
//ObjMinimum = ObjMinimum / ObjMinimum.w;
//glm::vec4 ObjMaximum = mvp * glm::vec4(Bmax, 1.0f);
//ObjMaximum = ObjMaximum / ObjMaximum.w;
//
////
//
////verifica, para cada plano, se está do lado de dentro, fora ou interseccionando:
//bool intersects = false;
//
//
//for (int i = 0; i < 3; i++)
//{
//	if (ObjMaximum[i] < -1.0f || ObjMinimum[i] > 1.0f)
//	{
//		//corta pq está fora de um plano LEFT/BOTTOM/FAR ou RIGHT/TOP/NEAR
//		return OUTSIDE;
//	}
//	else if (ObjMaximum[i] > 1.0f || ObjMinimum[i] < -1.0f) //aqui temos ObjMinimum[i] <= 1.0f && ObjMaximum[i] >= -1.0f implicitos do if acima
//	{
//		//este plano intersecciona com o obj, mas devo ainda sim checar os outros
//		intersects = true;
//	}
//	//senão, tudo aqui está dentro, e tenho que checar os proximos planos
//}
//
////se pelo menos um plano intersecciona o obj e nenhum outro apontou que ele está fora:
//if (intersects)
//{
//	return INTERSECT;
//}
//
////se não está fora de nenhum plano nem intersecta nenhum, está total dentro
////chequei todos os planos, nenhum apontou que o objeto está 
//
//if (ObjMaximum.x < -1.0f)
//{
//	//corta pq está fora de um plano LEFT
//	return OUTSIDE;
//}
//else if (ObjMinimum.x > 1.0f)
//{
//	//corta pq está fora do RIGHT
//	return OUTSIDE;
//}
//
//return INSIDE;