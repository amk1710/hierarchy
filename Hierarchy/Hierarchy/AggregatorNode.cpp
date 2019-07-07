
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


#include "BVHNode.h"
#include "AggregatorNode.h"

//constructor and destructor
//pretty useless right now
//AggregatorNode::AggregatorNode()
//{
//
//}
//AggregatorNode::~AggregatorNode()
//{
//
//}

void AggregatorNode::FreeNode()
{
	int i = 0;
	for (BVHNode* node : children)
	{
		if (node != NULL)
		{
			node->FreeNode();
			delete node;
			i++;
		}		
	}
	//cout << i << endl;
}

FrustumCheck AggregatorNode::IsInsideFrustum(glm::mat4 ModelViewProjection)
{
	return BVHNode::IsInsideFrustum(ModelViewProjection);
}

int AggregatorNode::Render(unsigned int shaderID)
{
	int count = 0;
	for (BVHNode* node : children)
	{
		count += node->Render(shaderID);
	}
	return count;
}

int AggregatorNode::CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection)
{
	FrustumCheck check = IsInsideFrustum(ViewProjection);
	int count = 0;
	switch (check)
	{
	case INSIDE:
		return Render(shaderID);
	case INTERSECT:
		
		for (BVHNode* node : children)
		{
			count += node->CheckFrustumAndRender(shaderID, ViewProjection);
		}
		return count;
	
	case OUTSIDE:
		return 0;
	
	//não deve cair aqui, só um caso pra tirar o warning
	default:
		return 0;
	}
}

void AggregatorNode::PushBVHNode(BVHNode* node)
{
	children.push_back(node);
}

glm::vec3 AggregatorNode::GetBmin()
{
	return Bmin;
}
glm::vec3 AggregatorNode::GetBmax()
{
	return Bmax;
}

void AggregatorNode::PrintHierarchy(ofstream& myfile, FrustumCheck check, glm::mat4 ViewProjection, int tabulation)
{
	if (check == INTERSECT)
	{
		check = IsInsideFrustum(ViewProjection);
	}
	myfile << std::string(tabulation, ' ') << "AggNode: " << check << endl;
	for (BVHNode* node : children)
	{
		node->PrintHierarchy(myfile, check, ViewProjection, tabulation + 1);
	}

}