#include "object.h"
#include "resourcemanager.h"

Object::Object() :
objectID(ResourceManager::getResourceID()),
bVisibility(true) 
{
	location = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
}

void Object::Draw(Shader* shader) {}