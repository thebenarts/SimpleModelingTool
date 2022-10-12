#include "spotLight.h"
#include "resourcemanager.h"

SpotLight::SpotLight(glm::vec3 amb, glm::vec3 diff , glm::vec3 spec)
	: spotLightID(ResourceManager::GetandAddSpotLightID(this))
{
	ambient = amb;
	diffuse = diff;
	specular = spec;
}