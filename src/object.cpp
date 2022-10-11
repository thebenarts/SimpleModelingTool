#include "object.h"
#include "resourcemanager.h"

Object::Object() :
objectID(ResourceManager::GetResourceID()),
bVisibility(true) 
{
	location = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
	objectName = "Object" + std::to_string(objectID);
}

void Object::Draw(Shader* shader) {}


glm::vec3 Object::GetObjectLocation()
{
	return location;
}

glm::vec3 Object::GetObjectRotation()
{
	return rotation;
}

void Object::SetObjectLocation(glm::vec3 newLocation)
{
	location = newLocation;
}

void Object::SetObjectRotation(glm::vec3 newRotation)
{
	rotation = newRotation;
}

