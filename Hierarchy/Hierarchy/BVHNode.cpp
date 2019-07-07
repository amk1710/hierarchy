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

#include <array>
#include <vector>


#include "stb_image.h"

#include "BVHNode.h"

//plane frustum funcionality
// extrai os frustums planes da matrix mvp
// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
void ExtractFrustumPlanes(glm::mat4 mvp, std::array<glm::vec4, 6> &planes)
{
	//// left
	//planes[0] = glm::vec4(
	//	mvp[3][0] + mvp[0][0],
	//	mvp[3][1] + mvp[0][1],
	//	mvp[3][2] + mvp[0][2],
	//	mvp[3][3] + mvp[0][3]
	//);

	////right
	//planes[1] = glm::vec4(
	//	mvp[3][0] - mvp[0][0],
	//	mvp[3][1] - mvp[0][1],
	//	mvp[3][2] - mvp[0][2],
	//	mvp[3][3] - mvp[0][3]
	//);

	////bottom
	//planes[2] = glm::vec4(
	//	mvp[3][0] + mvp[1][0],
	//	mvp[3][1] + mvp[1][1],
	//	mvp[3][2] + mvp[1][2],
	//	mvp[3][3] + mvp[1][3]
	//);


	////top
	//planes[3] = glm::vec4(
	//	mvp[3][0] - mvp[1][0],
	//	mvp[3][1] - mvp[1][1],
	//	mvp[3][2] - mvp[1][2],
	//	mvp[3][3] - mvp[1][3]
	//);

	////near
	//planes[4] = glm::vec4(
	//	mvp[3][0] + mvp[2][0],
	//	mvp[3][1] + mvp[2][1],
	//	mvp[3][2] + mvp[2][2],
	//	mvp[3][3] + mvp[2][3]
	//);

	////cout << "mvp mat" << endl;
	////cout << mvp[2][0] <<", " << mvp[2][1] << ", " << mvp[2][2] << ", " << mvp[2][3] << endl;

	////far
	//planes[5] = glm::vec4(
	//	mvp[3][0] - mvp[2][0],
	//	mvp[3][1] - mvp[2][1],
	//	mvp[3][2] - mvp[2][2],
	//	mvp[3][3] - mvp[2][3]
	//);

	////normaliza os planos, o os orienta "para fora"
	//for (int i = 0; i < 6; i++)
	//{
	//	//NormalizePlane(planes[i]);
	//}
	//return;

	//a transposta da mvp, pra destransformar os planos em espa�o p�s-proje��o
	glm::mat4 mvpT = glm::transpose(mvp);

	//todos os planos est�o a 1.0 de dist�ncia da origem, mas orientados diferentemente, com normais diferentes
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

//os planos que definem, no opengl, o cubo para renderiza��o, obtido ap�s a multiplica��o pela matriz de proje��o
//a ideia �, depois, na verifica��o de se este objeto est� no frustum, multiplicar estes planos pela inversa da mvp,
// de modo a obter o frustum no espa�o do objeto e poder verificar
//checks if the computed bounding box for this object is inside the view frustum


//recebe como parametro as normais que definem cada plano, no espa�o do mundo, e o ponto que marca o deslocamento deste plano a partir da origem
//ou n�o n�, vamos ver

FrustumCheck BVHNode::IsInsideFrustum(glm::mat4 ModelViewProjection)
{
	std::array<glm::vec4, 6> planes;

	ExtractFrustumPlanes(ModelViewProjection, planes);

	//planos est�o no espa�o do modelo, direcionados para fora (?) acho que n�o
	//ax + by + cz + d = 0, em um glm::vec4?

	bool intersects = false;

	for (int i = 0; i < 6; i++)
	{
		glm::vec3 normal = glm::vec3(planes[i]);
		normal = glm::normalize(normal);

		float d = planes[i].w;
		//acessa LUT baseado na normal do plano
		int s1 = normal.x > 0 ? 1 : 0; //e se for == 0? N�o importa?
		int s2 = normal.y > 0 ? 1 : 0;
		int s3 = normal.z > 0 ? 1 : 0;

		glm::vec3 Pmax = LUT[4 * s1 + 2 * s2 + s3]; //max ou min?
													//inverte LUTs (eu acho que pode ser feito de outro jeito, com 8 - x = y?)
		s1 = s1 == 1 ? 0 : 1;
		s2 = s2 == 1 ? 0 : 1;
		s3 = s3 == 1 ? 0 : 1;
		glm::vec3 Pmin = LUT[4 * s1 + 2 * s2 + s3]; //max ou min

													//calcula para o ponto Pmin e Pmax a dist�ncia at� o plano
													//normais apontam para fora (?)
													//http://mathworld.wolfram.com/Point-PlaneDistance.html
		float distMin = glm::dot(normal, Pmin) + d; // >0 mesmo lado, <0 outro lado, ==0 no plano;
		float distMax = glm::dot(normal, Pmax) + d;

		//>0 t� dentro, < 0 t� fora
		if (distMin < 0.0f && distMax < 0.0f) //Pmin e Pmax est�o fora
		{
			return OUTSIDE;
		}
		else if (distMin < 0.0f && distMax > 0.0f) //Pmin est� dentro e Pmax est� fora
		{
			intersects = true;
		}
		else
		{
			//est� dentro para este plano, continuo checando
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
