#pragma once

using namespace std;

class BVHNode
{

private:
	//recebe como argumento as seis normais que definem os planos do frustum, no espa�o do mundo,
	//e os seis pontos que definem o deslocamento do plano em rela��o � origem
	FrustumCheck IsInsideFrustum(glm::mat4 ViewProjection);

	
public:

	

	BVHNode();
	//~BVHNode();

	void CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection);
	void LoadObjectFromPath(char const * path);
	void Initialize();
	void Render(unsigned int shaderID);

};