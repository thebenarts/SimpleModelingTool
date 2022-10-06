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

	RENDERSTATE renderState;

	static void RenderScene(Camera& camera);

	static glm::mat4 projection; 
};