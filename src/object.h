#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader_m.h"
// asd
class Object 
{
	public:
		unsigned int objectID;
		bool bVisibility;
		glm::vec3 location;
		glm::vec3 rotation;

		Object();

		virtual void Draw(Shader* shader);
	private:
		
		

};