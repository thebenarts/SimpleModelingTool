#include "renderer.h"
#include "resourcemanager.h"

glm::mat4 Renderer::projection = glm::perspective(glm::radians(90.f), (float)2560/ (float)1440, 0.1f, 100.f);

void Renderer::RenderScene(Camera& camera)
{
	Shader* dirLight = ResourceManager::GetShader("dirLight");
	if (!dirLight)
	{
		std::cout << "SHADER::ERROR:: MISSING SHADER: IN RENDERER" << std::endl;
		return;
	}

	//std::cout << "HELLO THIS IS FINE HERER" << std::endl;

	// apply view and projection matrice
	dirLight->Use();
	dirLight->setMat4("projection", Renderer::projection);
	dirLight->setMat4("view", camera.GetViewMatrix());
	dirLight->setVec3("light.direction", glm::vec3(camera.Front));
	dirLight->setVec3("viewPos", camera.Position);

	//light properties
	dirLight->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	dirLight->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);

	for (std::pair<const unsigned int, Object*>& currentObject : ResourceManager::objectMap)
	{
		if (currentObject.second->bVisibility)
			currentObject.second->Draw(dirLight);
	}

}