#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader_m.h"

class Object 
{
	public:
		Object();

		virtual ~Object();

		virtual void Draw(Shader* shader);

		unsigned int objectID;
		std::string objectName;
		bool bVisibility;
		bool bSelected;

		glm::vec3 GetObjectLocation();
		glm::vec3 GetObjectRotation();
		glm::vec3 GetObjectScale();

		virtual void SetObjectLocation(glm::vec3 newLocation);
		virtual void SetObjectRotation(glm::vec3 newRotation);
		virtual void SetObjectScale(glm::vec3 newScale);

		virtual void Select();
		virtual void DeSelect();

		virtual void AddID();
		virtual void RemoveID();
		virtual void FreeID();

		bool bRemoved;		// this should be fixed... technically speaking we could just use shared_ptrs for storing objects in commands.

	protected:
	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 scale;

};