#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Object 
{
	public:



		void Draw() {};
	private:
		unsigned int objectID;
		bool bVisibility;
		glm::vec3 location;
		glm::vec3 rotation;

};