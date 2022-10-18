#pragma once

#include "light.h"

class PointLight : public Light {
public:
	unsigned int pointLightID;

	virtual ~PointLight();

	virtual void AddID() override;
	virtual void RemoveID() override;
	virtual void FreeID() override;

	PointLight(glm::vec3 amb = glm::vec3(0.1f), glm::vec3 diff = glm::vec3(1.0f), glm::vec3 spec = glm::vec3(0.0f));
};