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
	std::vector<BVHNode> children;
public:

	void CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection);
	void Render(unsigned int shaderID);
};
