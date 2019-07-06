#pragma once
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <tiny_obj_loader.h>

#include "RenderObject.h"

using namespace std;

class GLWindowManager
{

private:

	//temporário, enquanto eu não implemento a árvore
	std::vector<RenderObject> objects;

	GLFWwindow* window;
	
	unsigned int colorBuffer;

	unsigned int screenWidth = 800;
	unsigned int screenHeight = 600;


	int width_bm;
	int height_bm;
	int nrChannels_bm;
	unsigned char *bump_data;

	unsigned int texture;
	unsigned int bumpmap;

	glm::mat4 Projection;
	glm::mat4 View;

	float angle;

	float incNLights;
	int nLights;
	std::array<float, 3 * 32> lights;
	std::array<float, 3 * 32> lightsColors;

	//aux functions
	std::string GLWindowManager::readShaderFile(const char* name);
	void GLWindowManager::UpdateMVPMatrix();
	void GLWindowManager::BuildShader(const char* filepath, unsigned int* shaderID, int shader_enum);
	void GLWindowManager::randomPointInSphere(float *x, float *y, float *z, float radius);


	//NEW STUFF
	//gBuffer framebuffer stuff
	unsigned int gPass;
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gColorSpec, gTangent, gBitangent;

	unsigned int rboDepth;

	unsigned int lPass;

	unsigned int dPass; //debug stage
	bool usingDebug;
	unsigned int selectTexture;

	//seconds per frame counter
	double lastTime;
	int nbFrames;

	//stuff for helping with view frustum calculation
	glm::vec3 planes_normals[6];
	glm::vec3 planes_points[6];


	

public:
	GLWindowManager();
	~GLWindowManager();
	void InitializeSceneInfo();
	void GLWindowManager::StartRenderLoop();

	glm::vec3 eye;
	glm::vec3 cameraTarget;
	glm::vec3 up;

	void processInput(GLFWwindow *window);
};