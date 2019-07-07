#pragma once

using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

//enumera��o retornada pela fun��o IsInsideFrustum
enum FrustumCheck { INSIDE, OUTSIDE, INTERSECT };

class BVHNode
{

private:


protected:
	
	//coisas para a bounding box
	glm::vec3 Bmin;
	glm::vec3 Bmax;
	glm::vec3 LUT[8];

public:

	//recebe como argumento as seis normais que definem os planos do frustum, no espa�o do mundo,
	//e os seis pontos que definem o deslocamento do plano em rela��o � origem
	//talvez nem precise dessa fun��o?
	FrustumCheck IsInsideFrustum(glm::mat4 ModelViewProjection);

	virtual void CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection) = 0;
	virtual void Render(unsigned int shaderID) = 0;

};