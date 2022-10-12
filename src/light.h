#pragma once

#include "object.h"

class Light : public Object {
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
};