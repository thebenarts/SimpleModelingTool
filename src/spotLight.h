#pragma once

#include "light.h"

class SpotLight : public Light {
public:
	unsigned int spotLightID;
	glm::vec3 direction = glm::vec3(0.0f);

	float innerCutOff = 12.5;
	float outerCutOff = 17.5;

	SpotLight(glm::vec3 amb = glm::vec3(0.1f), glm::vec3 diff = glm::vec3(1.0f), glm::vec3 spec = glm::vec3(0.0f));
	~SpotLight();
};