#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader_m.h"

class Object 
{
	public:
		Object();

		virtual void Draw(Shader* shader);

		unsigned int objectID;
		bool bVisibility;

	protected:
	glm::vec3 location;
	glm::vec3 rotation;

};