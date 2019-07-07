#pragma once

using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "BVHNode.h"

class AggregatorNode : public BVHNode
{
private:
	std::vector<BVHNode*> children;
public:

	FrustumCheck IsInsideFrustum(glm::mat4 ModelViewProjection);
	int CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection);
	int Render(unsigned int shaderID);

	void PushBVHNode(BVHNode* node);

	void FreeNode();

	glm::vec3 GetBmin();
	glm::vec3 GetBmax();
};
