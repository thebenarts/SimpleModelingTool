#include "renderer.h"
#include "resourcemanager.h"

glm::mat4 Renderer::projection;
glm::mat4 Renderer::view;
Camera* Renderer::currentCamera;
unsigned int Renderer::gridVAO = 0;
unsigned int Renderer::gridVBO = 0;

void Renderer::RenderScene()
{

	if (!currentCamera)
	{
		std::cout << "RENDERER::ERROR:: CAMERA NULL POINTER" << std::endl;
		return;
	}

	Shader* dirLight = ResourceManager::GetShader("dirLight");
	if (!dirLight)
	{
		std::cout << "SHADER::ERROR:: MISSING SHADER: IN RENDERER" << std::endl;
		return;
	}

	//std::cout << "HELLO THIS IS FINE HERER" << std::endl;
	Renderer::projection = Renderer::currentCamera->GetProjectionMatrix();
	Renderer::view = Renderer::currentCamera->GetViewMatrix();
	// apply view and projection matrice
	dirLight->Use();
	dirLight->setMat4("projection", Renderer::projection);
	dirLight->setMat4("view", Renderer::view);
	dirLight->setVec3("light.direction", glm::vec3(Renderer::currentCamera->Front));
	dirLight->setVec3("viewPos", Renderer::currentCamera->Position);

	//light properties
	dirLight->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	dirLight->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);

	for (std::pair<const unsigned int, Object*>& currentObject : ResourceManager::objectMap)
	{
		if (currentObject.second->bVisibility)
			currentObject.second->Draw(dirLight);
	}

	Renderer::RenderGrid();
}

void Renderer::InitGrid()
{
	float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &Renderer::gridVAO);
	glGenBuffers(1, &Renderer::gridVBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, Renderer::gridVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(Renderer::gridVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}
void Renderer::RenderGrid()
{
	if (Renderer::gridVAO == 0)
		Renderer::InitGrid();

	Shader* guideGrid = ResourceManager::GetShader("guideGrid");
	if (!guideGrid)
	{
		std::cout << "SHADER::ERROR::MISSING SHADER: GUIDE GRID IN RENDERER" << std::endl;
		return;
	}

	guideGrid->Use();
	guideGrid->setMat4("projection", Renderer::projection);
	guideGrid->setMat4("view", Renderer::view);

	// render Cube
	glBindVertexArray(Renderer::gridVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Renderer::SetCamera(Camera* camera)
{
	if(camera)
	currentCamera = camera;
}