
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