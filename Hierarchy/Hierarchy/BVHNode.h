#ifndef BVHNODE
#define BVHNODE

using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

//enumera��o retornada pela fun��o IsInsideFrustum
enum FrustumCheck { INSIDE, OUTSIDE, INTERSECT };

extern bool def_check_plane[6];

class BVHNode
{

private:
	
	

public:
	
	void UpdateLUT();
	//coisas para a bounding box

	//para as subclasses, usamos as mesmas vari�veis, mas com a grande diferen�a de que:
	// para o renderobject, a bb est� definida no espa�o do objeto, ent�o pra IsInsideFrustum devo passar a matriz mvp
	// para o aggregator node, a bb est� definida no espa�o do mundo, ent�o pra IsInsideFrustum devo passar a matrix vp
	glm::vec3 Bmin;
	glm::vec3 Bmax;
	glm::vec3 LUT[8];

	//usado para as otimiza��es de coer�ncia temporal e elimina��o de redund�ncia quando descendo a �rvore
	unsigned char plane_order[6] = { 0,1,2,3,4,5 };


	//recebe como argumento as seis normais que definem os planos do frustum, no espa�o do mundo,
	//e os seis pontos que definem o deslocamento do plano em rela��o � origem
	//talvez nem precise dessa fun��o?
	FrustumCheck IsInsideFrustum(glm::mat4 ModelViewProjection, bool check_planes[6] = def_check_plane);

	//int: retorna quantos objs foram renderizados por essa fun��o
	virtual int CheckFrustumAndRender(unsigned int shaderID, glm::mat4 ViewProjection) = 0;
	virtual int Render(unsigned int shaderID) = 0;

	//� necess�rio, pq, fazendo o acesso direto, estar�amos descosiderando que as subclasses podem salvar este valor em espa�os diferentes:
	//no caso,  modelo ou mundo. Queremos que o retorno desta fun��o abaixo seja sempre em MUNDO
	virtual glm::vec3 GetBmin() = 0;
	virtual glm::vec3 GetBmax() = 0;

	virtual void FreeNode() = 0;

	virtual void PrintHierarchy(ofstream& myfile, FrustumCheck check, glm::mat4 ViewProjection, int tabulation) = 0;

	//auxiliary: swap
	template<typename T>
	void swap(T &a, T &b)
	{
		T t = a;
		a = b;
		b = t;
	}
	//place first in array
	template<typename T>
	void place_first(T arr[6], unsigned int pos)
	{
		unsigned int i = 0;
		T temp;
		while (i != pos)
		{
			temp = arr[i];
			arr[i] = arr[pos];
			arr[pos] = temp;
			i++;
		}
	}

};

#endif // !BVHNODE
