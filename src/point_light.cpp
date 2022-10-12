#include "point_light.h"
#include "resourcemanager.h"

PointLight::PointLight(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
	: pointLightID(ResourceManager::GetandAddPointLightID(this))
{
	ambient = amb;
	diffuse = diff;
	specular = spec;
}