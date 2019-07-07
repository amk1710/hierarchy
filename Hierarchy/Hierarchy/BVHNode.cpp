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
#include <time.h>
#include <limits>
#include <assert.h>

#include <array>
#include <vector>


#include "stb_image.h"

#include "BVHNode.h"
#include "AggregatorNode.h"
#include "RenderObject.h"

//plane frustum funcionality
// extrai os frustums planes da matrix mvp
// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
void ExtractFrustumPlanes(glm::mat4 mvp, std::array<glm::vec4, 6> &planes)
{
	//a transposta da mvp, pra destransformar os planos em espaço pós-projeção
	glm::mat4 mvpT = glm::transpose(mvp);

	//todos os planos estão a 1.0 de distância da origem, mas orientados diferentemente, com normais diferentes
	planes[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); //right
	planes[1] = glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f); //left
	planes[2] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); //up
	planes[3] = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f); //down
	planes[4] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); //far
	planes[5] = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f); //near

	for (int i = 0; i < 6; i++)
	{
		planes[i] = mvpT * planes[i];
		
		//planes[i] = planes[i] / planes[i].w;
		//normaliza plano
		planes[i] = planes[i] / (sqrt(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z));
	}

	return;

}

//os planos que definem, no opengl, o cubo para renderização, obtido após a multiplicação pela matriz de projeção
//a ideia é, depois, na verificação de se este objeto está no frustum, multiplicar estes planos pela inversa da mvp,
// de modo a obter o frustum no espaço do objeto e poder verificar
//checks if the computed bounding box for this object is inside the view frustum


//recebe como parametro as normais que definem cada plano, no espaço do mundo, e o ponto que marca o deslocamento deste plano a partir da origem
//ou não né, vamos ver

FrustumCheck BVHNode::IsInsideFrustum(glm::mat4 ModelViewProjection)
{
	std::array<glm::vec4, 6> planes;

	ExtractFrustumPlanes(ModelViewProjection, planes);

	//planos estão no espaço do modelo, direcionados para fora (?) acho que não
	//ax + by + cz + d = 0, em um glm::vec4?

	bool intersects = false;

	for (int i = 0; i < 6; i++)
	{
		glm::vec3 normal = glm::vec3(planes[i]);
		normal = glm::normalize(normal);

		float d = planes[i].w;
		//acessa LUT baseado na normal do plano
		int s1 = normal.x > 0 ? 1 : 0; //e se for == 0? Não importa?
		int s2 = normal.y > 0 ? 1 : 0;
		int s3 = normal.z > 0 ? 1 : 0;

		glm::vec3 Pmax = LUT[4 * s1 + 2 * s2 + s3]; //max ou min?
													//inverte LUTs (eu acho que pode ser feito de outro jeito, com 8 - x = y?)
		s1 = s1 == 1 ? 0 : 1;
		s2 = s2 == 1 ? 0 : 1;
		s3 = s3 == 1 ? 0 : 1;
		glm::vec3 Pmin = LUT[4 * s1 + 2 * s2 + s3]; //max ou min

													//calcula para o ponto Pmin e Pmax a distância até o plano
													//normais apontam para fora (?)
													//http://mathworld.wolfram.com/Point-PlaneDistance.html
		float distMin = glm::dot(normal, Pmin) + d; // >0 mesmo lado, <0 outro lado, ==0 no plano;
		float distMax = glm::dot(normal, Pmax) + d;

		//>0 tá dentro, < 0 tá fora
		if (distMin < 0.0f && distMax < 0.0f) //Pmin e Pmax estão fora
		{
			return OUTSIDE;
		}
		else if (distMin < 0.0f && distMax > 0.0f) //Pmin está dentro e Pmax está fora
		{
			intersects = true;
		}
		else
		{
			//está dentro para este plano, continuo checando
		}

	}

	if (intersects)
	{
		return INTERSECT;
	}
	else
	{
		return INSIDE;
	}

}

void BVHNode::UpdateLUT()
{
	//com Bmin e Bmax, constrói LUT para acesso rápido de Vmin, Vmax
	LUT[0] = glm::vec3(Bmin.x, Bmin.y, Bmin.z); // 0,0,0 -> min, min min
	LUT[1] = glm::vec3(Bmin.x, Bmin.y, Bmax.z); // 0,0,1 -> min, min, max
	LUT[2] = glm::vec3(Bmin.x, Bmax.y, Bmin.z); // 0,1,0 -> min,max,min
	LUT[3] = glm::vec3(Bmin.x, Bmax.y, Bmax.z); // 0,1,1 -> min,max,max
	LUT[4] = glm::vec3(Bmax.x, Bmin.y, Bmin.z); // 1,0,0 -> max,min,min
	LUT[5] = glm::vec3(Bmax.x, Bmin.y, Bmax.z); // 1,0,1 -> max,min,max
	LUT[6] = glm::vec3(Bmax.x, Bmax.y, Bmin.z); // 1,1,0 -> max,max,min
	LUT[7] = glm::vec3(Bmax.x, Bmax.y, Bmax.z); // 1,1,1 -> max,max,max
}