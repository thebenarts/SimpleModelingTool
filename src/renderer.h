#pragma once
#include "Camera.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

enum RENDERSTATE
{
	UNLIT,
	DIRLIGHT
};

class Renderer {
public:

	static RENDERSTATE renderState;

	static void SetCamera(Camera* camera);

	static void RenderScene();

	static void InitGrid();

	static void RenderGrid();

protected:

	static unsigned int gridVAO, gridVBO;
	static Camera* currentCamera;

	static glm::mat4 projection;

	static glm::mat4 view;
};