#include "renderer.h"
#include "resourcemanager.h"
#include "simpleModel.h"

glm::mat4 Renderer::projection;
glm::mat4 Renderer::view;
Camera* Renderer::currentCamera;
unsigned int Renderer::gridVAO = 0;
unsigned int Renderer::gridVBO = 0;

//void Renderer::RenderScene()
//{
//
//	if (!currentCamera)
//	{
//		std::cout << "RENDERER::ERROR:: CAMERA NULL POINTER" << std::endl;
//		return;
//	}
//
//	Shader* dirLight = ResourceManager::GetShader("albedo");
//	if (!dirLight)
//	{
//		std::cout << "SHADER::ERROR:: MISSING SHADER: IN RENDERER" << std::endl;
//		return;
//	}
//
//	//std::cout << "HELLO THIS IS FINE HERER" << std::endl;
//	Renderer::projection = Renderer::currentCamera->GetProjectionMatrix();
//	Renderer::view = Renderer::currentCamera->GetViewMatrix();
//	// apply view and projection matrice
//	dirLight->Use();
//	dirLight->setMat4("projection", Renderer::projection);
//	dirLight->setMat4("view", Renderer::view);
//	dirLight->setVec3("light.direction", glm::vec3(Renderer::currentCamera->Front));
//	dirLight->setVec3("viewPos", Renderer::currentCamera->Position);
//
//	//light properties
//	dirLight->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
//	dirLight->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
//
//	Shader* outline = ResourceManager::GetShader("outline");
//	if (!outline)
//	{
//		std::cout << "SHADER::ERROR:: MISSING SHADER: IN RENDERER" << std::endl;
//		return;
//	}
//
//
//	glStencilFunc(GL_ALWAYS, 1, 0xFF);
//	glStencilMask(0xFF);
//	for (Object* currentObject : ResourceManager::objects)
//	{
//		if (!currentObject)
//			continue;
//
//		if (currentObject->bVisibility){
//			currentObject->Draw(dirLight);
//			if (currentObject->bSelected) {
//				Cube* selected = dynamic_cast<Cube*>(currentObject);
//				if (selected) {
//					// save data
//					glm::vec3 pLocation = selected->GetObjectLocation();
//					glm::vec3 pRotation = selected->GetObjectRotation();
//					glm::vec3 pScale = selected->GetCubeScale();
//					// increase scale of selection to render as outline
//					selected->SetCubeScale(pScale * glm::vec3(1.02f));
//
//					outline->Use();
//					outline->setMat4("projection", Renderer::projection);
//					outline->setMat4("view", Renderer::view);
//					// set stencil
//					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//					glStencilMask(0x00);
//					glDisable(GL_DEPTH_TEST);
//
//					selected->Draw(outline);
//					// set scale back to original
//					selected->SetCubeScale(pScale);
//					glEnable(GL_DEPTH_TEST);
//				}
//			}
//		}
//	}
//
//	//Renderer::RenderGrid();
//	
//	Renderer::RenderGrid();
//
//	glStencilMask(0xFF);
//	glStencilFunc(GL_ALWAYS, 0, 0xFF);
//	glEnable(GL_DEPTH_TEST);
//}

void Renderer::RenderScene()
{

	if (!currentCamera)
	{
		std::cout << "RENDERER::ERROR:: CAMERA NULL POINTER" << std::endl;
		return;
	}

	Shader* dirLight = ResourceManager::GetShader("mLights");
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
	dirLight->setVec3("viewPos", Renderer::currentCamera->Position);

	//dirLight properties
	dirLight->setVec3("dirLight.direction", glm::vec3(Renderer::currentCamera->Front));
	dirLight->setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
	dirLight->setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.2f);

	// fill up the point light data
	for (int i = 0; i < ResourceManager::pointLights.size(); ++i)
	{
		std::string number = std::to_string(i);

		if (ResourceManager::pointLights[i])
		{
			dirLight->setVec3("pointLights[" + number + "].position", ResourceManager::pointLights[i]->GetObjectLocation());
			dirLight->setFloat("pointLights[" + number + "].constant", ResourceManager::pointLights[i]->constant);
			dirLight->setFloat("pointLights[" + number + "].linear", ResourceManager::pointLights[i]->linear);
			dirLight->setFloat("pointLights[" + number + "].quadratic", ResourceManager::pointLights[i]->quadratic);
			dirLight->setVec3("pointLights[" + number + "].ambient", ResourceManager::pointLights[i]->ambient);
			dirLight->setVec3("pointLights[" + number + "].diffuse", ResourceManager::pointLights[i]->diffuse);
			dirLight->setVec3("pointLights[" + number + "].specular", ResourceManager::pointLights[i]->specular);

			dirLight->setBool("pointLights[" + number + "].bCalculate", true);
		}
		else
		{
			dirLight->setBool("pointLights[" + number + "].bCalculate", false);
		}
	}

	// fill up the spot light data
	for (int i = 0; i < ResourceManager::spotLights.size(); ++i)
	{
		std::string number = std::to_string(i);

		if (ResourceManager::spotLights[i])
		{
			dirLight->setVec3("spotLights[" + number + "].position", ResourceManager::spotLights[i]->GetObjectLocation());
			dirLight->setVec3("spotLights[" + number + "].direction", ResourceManager::spotLights[i]->direction);

			dirLight->setFloat("spotLights[" + number + "].innerCutOff", ResourceManager::spotLights[i]->innerCutOff);
			dirLight->setFloat("spotLights[" + number + "].outerCutOff", ResourceManager::spotLights[i]->outerCutOff);

			dirLight->setFloat("spotLights[" + number + "].constant", ResourceManager::spotLights[i]->constant);
			dirLight->setFloat("spotLights[" + number + "].linear", ResourceManager::spotLights[i]->linear);
			dirLight->setFloat("spotLights[" + number + "].quadratic", ResourceManager::spotLights[i]->quadratic);
			dirLight->setVec3("spotLights[" + number + "].ambient", ResourceManager::spotLights[i]->ambient);
			dirLight->setVec3("spotLights[" + number + "].diffuse", ResourceManager::spotLights[i]->diffuse);
			dirLight->setVec3("spotLights[" + number + "].specular", ResourceManager::spotLights[i]->specular);

			dirLight->setBool("spotLights[" + number + "].bCalculate", true);
		}
		else
		{
			dirLight->setBool("spotLights[" + number + "].bCalculate", false);
		}
	}



	Shader* outline = ResourceManager::GetShader("outline");
	if (!outline)
	{
		std::cout << "SHADER::ERROR:: MISSING SHADER: IN RENDERER" << std::endl;
		return;
	}


	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for (Object* currentObject : ResourceManager::objects)
	{
		if (!currentObject)
			continue;

		if (currentObject->bVisibility) {
			currentObject->Draw(dirLight);
			if (currentObject->bSelected) {
				Cube* selected = dynamic_cast<Cube*>(currentObject);
				if (selected) {
					// save data
					glm::vec3 pLocation = selected->GetObjectLocation();
					glm::vec3 pRotation = selected->GetObjectRotation();
					glm::vec3 pScale = selected->GetCubeScale();
					// increase scale of selection to render as outline
					selected->SetCubeScale(pScale * glm::vec3(1.02f));

					outline->Use();
					outline->setMat4("projection", Renderer::projection);
					outline->setMat4("view", Renderer::view);
					// set stencil
					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
					glStencilMask(0x00);
					glDisable(GL_DEPTH_TEST);

					selected->Draw(outline);
					// set scale back to original
					selected->SetCubeScale(pScale);
					glEnable(GL_DEPTH_TEST);
				}
			}
		}
	}

	//Renderer::RenderGrid();

	Renderer::RenderGrid();

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
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