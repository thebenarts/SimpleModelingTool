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
		bool bSelected;

		glm::vec3 GetObjectLocation();
		glm::vec3 GetObjectRotation();

		void SetObjectLocation(glm::vec3 newLocation);
		void SetObjectRotation(glm::vec3 newRotation);

	protected:
	glm::vec3 location;
	glm::vec3 rotation;

};