#include "Camera.h"
#include "resourcemanager.h"

void Camera::Draw(Shader* shader)
{

}

void Camera::SetObjectLocation(glm::vec3 newLocation)
{
	location = newLocation;
	Position = location;
}

void Camera::Select()
{
	Object::Select();

	ResourceManager::SetViewportCamera(this);
}

void Camera::DeSelect()
{
	Object::DeSelect();

	ResourceManager::SetViewportCamera(nullptr);
}