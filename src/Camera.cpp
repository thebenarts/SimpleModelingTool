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

void Camera::SetCameraState(CameraState* desiredState)
{
	if (camState)
		delete camState;

	camState = desiredState;
}

void Camera::SwapState()
{
	if (camState)
	{
		CameraState* toBeDeleted = camState->SwapState();
		delete toBeDeleted;
	}
}

//------------------------------------------------------------------------------------FLYING CAMERA STATE----------------------------------------------------------------------------
FlyingCameraState::FlyingCameraState() : camera(nullptr)
{

}

FlyingCameraState::FlyingCameraState(Camera* inCamera) : camera(inCamera)
{

}

void FlyingCameraState::SetContext(Camera* inCamera)
{
	FlyingCameraState::camera = inCamera;
}

CameraState* FlyingCameraState::SwapState()
{
	camera->camState = new OrbitingCameraState(camera);
	return this;
}

void FlyingCameraState::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = camera->MovementSpeed * deltaTime;
	if (direction == FORWARD)
		camera->Position += camera->Front * velocity;
	if (direction == BACKWARD)
		camera->Position -= camera->Front * velocity;
	if (direction == LEFT)
		camera->Position -= camera->Right * velocity;
	if (direction == RIGHT)
		camera->Position += camera->Right * velocity;

	camera->location = camera->Position;
}

void FlyingCameraState::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	if (!camera)
		return;

	xoffset *= camera->MouseSensitivity;
	yoffset *= camera->MouseSensitivity;

	camera->Yaw += xoffset;
	camera->Pitch += yoffset;
	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (camera->Pitch > 89.f)
			camera->Pitch = 89.f;
		if (camera->Pitch < -89.f)
			camera->Pitch = -89.f;
	}

	this->UpdateCameraVectors();
}

void FlyingCameraState::ProcessMouseScroll(float yoffset)
{
	camera->FieldOfView -= (float)yoffset;
	if (camera->FieldOfView < 1.0f)
		camera->FieldOfView = 1.0f;
	if (camera->FieldOfView > 100.0f)
		camera->FieldOfView = 100.0f;
	camera->UpdateCameraProjectionMatrix();
	if (camera->lensType == PERSPECTIVE)
	this->UpdateCameraVectors();
}

void FlyingCameraState::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(camera->Yaw)) * cos(glm::radians(camera->Pitch));
	front.y = sin(glm::radians(camera->Pitch));
	front.z = sin(glm::radians(camera->Yaw)) * cos(glm::radians(camera->Pitch));
	camera->Front = glm::normalize(front);

	// also re-calculate the right and up vetor
	camera->Right = glm::normalize(glm::cross(camera->Front, camera->WorldUp));	// Normalize the vectors, because their lenght gets closer to - the more you look up or down which results in slower movement
	camera->Up = glm::normalize(glm::cross(camera->Right, camera->Front));
}

//------------------------------------------------------------------------------------ORBITING CAMERA STATE----------------------------------------------------------------------------
OrbitingCameraState::OrbitingCameraState() :camera(nullptr)
{

}

OrbitingCameraState::OrbitingCameraState(Camera* inCamera) : camera(inCamera)
{

}

void OrbitingCameraState::SetContext(Camera* inCamera)
{
	camera = inCamera;
}

CameraState* OrbitingCameraState::SwapState()
{
	camera->camState = new FlyingCameraState(camera);
	return this;
}

void OrbitingCameraState::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{

}

void OrbitingCameraState::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	if (camera->orbitState == ORBITING) {
		xoffset *= camera->MouseSensitivity / 10;
		yoffset *= camera->MouseSensitivity / 10;

		camera->rotateAzimuth(-xoffset);
		camera->rotatePolar(-yoffset);
	}
	if (camera->orbitState == STRAFING) {
		xoffset *= camera->MouseSensitivity;
		yoffset *= camera->MouseSensitivity;
		camera->moveHorizontal(xoffset);
		camera->moveVertical(yoffset);
	}

	// update Front, Right and Up vectors using the updated Euler angles
	this->UpdateCameraVectors();
}

void OrbitingCameraState::ProcessMouseScroll(float yoffset)
{
	camera->radius -= yoffset;
	if (camera->radius < camera->minRadius)
		camera->radius = camera->minRadius;
	if (camera->radius > camera->maxRadius)
		camera->radius = camera->maxRadius;
	if (camera->lensType == PERSPECTIVE)
		this->UpdateCameraVectors();
}

void OrbitingCameraState::UpdateCameraVectors()
{
	camera->Position.x = camera->center.x + camera->radius * cos(camera->polarAngle) * cos(camera->azimuthAngle);
	camera->Position.y = camera->center.y + camera->radius * sin(camera->polarAngle);
	camera->Position.z = camera->center.z + camera->radius * cos(camera->polarAngle) * sin(camera->azimuthAngle);
	camera->Front = glm::normalize(camera->center - camera->Position);

	// also re-calculate the right and up vetor
	camera->Right = glm::normalize(glm::cross(camera->Front, camera->WorldUp));	// Normalize the vectors, because their lenght gets closer to - the more you look up or down which results in slower movement
	camera->Up = glm::normalize(glm::cross(camera->Right, camera->Front));
}

