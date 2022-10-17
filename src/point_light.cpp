#include "point_light.h"
#include "resourcemanager.h"

PointLight::PointLight(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
	: pointLightID(ResourceManager::GetandAddPointLightID(this))
{
	ambient = amb;
	diffuse = diff;
	specular = spec;

	objectName = "pointLight" + std::to_string(pointLightID);
}

PointLight::~PointLight()
{
	Object::~Object();
	ResourceManager::pointLights[objectID] = nullptr;
	ResourceManager::freePointLightID.push(objectID);
}

void PointLight::AddID()
{
	Object::AddID();
	ResourceManager::AddPointID(this);
}
void PointLight::RemoveID()
{
	Object::RemoveID();
	ResourceManager::RemovePointID(pointLightID);
}
void PointLight::FreeID()
{
	Object::FreeID();
	ResourceManager::FreePointID(pointLightID);
}