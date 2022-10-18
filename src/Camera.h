#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include <vector>
#include "object.h"

class Camera;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum Camera_ProjectionMode
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

enum Camera_Orbit {
	ORBITING,		//if middlemouse held
	STRAFING,		//if shift+mmb held
	DEFAULT		
};

// Default camera values
const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processses input and calculates the corresponding Euler Angles, vectors and matrices for use in OpenGL
class CameraState
{
public:
	virtual ~CameraState(){}
	virtual void SetContext(Camera* inCamera) = 0;
	virtual CameraState* SwapState() = 0;
	virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) = 0;
	virtual void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;
	virtual void ProcessMouseScroll(float yoffset) = 0;
	virtual void UpdateCameraVectors() = 0;
};

class FlyingCameraState : public CameraState
{
public:
	FlyingCameraState();
	FlyingCameraState(Camera* inCamera);
	~FlyingCameraState(){}
	void SetContext(Camera* inCamera) override;
	CameraState* SwapState()override;
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;
	void ProcessMouseScroll(float yoffset) override;
	void UpdateCameraVectors() override;

private:
	Camera* camera;
};

class OrbitingCameraState : public CameraState
{
public:
	OrbitingCameraState();
	OrbitingCameraState(Camera* inCamera);
	~OrbitingCameraState(){}
	void SetContext(Camera* inCamera) override;
	CameraState* SwapState() override;
	void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;
	void ProcessMouseScroll(float yoffset) override;
	void UpdateCameraVectors() override;
private:
	Camera* camera;
};

class Camera : public Object
{
public:
	friend CameraState;
	friend FlyingCameraState;
	friend OrbitingCameraState;
	// camera attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::mat4 projectionMatrice;

	// euler Angles 
	float Yaw;
	float Pitch;

	//orbiting camera variables
	Camera_Orbit orbitState = DEFAULT;
	glm::vec3 center = glm::vec3(0.0f);
	float radius = 10.0f;
	float minRadius = 1.0f;
	float maxRadius = 50.0f;
	float azimuthAngle = 1.0f;
	float polarAngle = 0.5f;

	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float FieldOfView;
	float nearPlane;
	float farPlane;
	unsigned int screenWidth;
	unsigned int screenHeight;

	Camera_ProjectionMode lensType;

	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED),
		MouseSensitivity(SENSITIVITY),
		FieldOfView(ZOOM),
		nearPlane(0.1f),
		farPlane(100.f),
		screenWidth(2560),
		screenHeight(1440),
		lensType(PERSPECTIVE),
		camState(new OrbitingCameraState(this))
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraProjectionMatrix();
		UpdateCameraVectors();
		objectName = "Camera" + std::to_string(objectID);
	}

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int scrWidth = 2560, unsigned int scrHeight = 1440, float nearP = 0.1f, float farP = 100.f, Camera_ProjectionMode projectionType = PERSPECTIVE, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED),
		MouseSensitivity(SENSITIVITY),
		FieldOfView(ZOOM),
		nearPlane(nearP),
		farPlane(farP),
		screenWidth(scrWidth),
		screenHeight(scrHeight),
		lensType(projectionType),
		camState(new OrbitingCameraState(this))
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraProjectionMatrix();
		UpdateCameraVectors();
		objectName = "Camera" + std::to_string(objectID);
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch , unsigned int scrWidth, unsigned int scrHeight, float nearP , float farP, Camera_ProjectionMode projectionType ) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED),
		MouseSensitivity(SENSITIVITY),
		FieldOfView(ZOOM),
		nearPlane(nearP),
		farPlane(farP),
		screenWidth(scrWidth),
		screenHeight(scrHeight),
		lensType(projectionType),
		camState(new OrbitingCameraState(this))
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraProjectionMatrix();
		UpdateCameraVectors();
		objectName = "Camera" + std::to_string(objectID);
	}

	void SetCameraState(CameraState* desiredState);
	void SwapState();

	// returns the view matrix calculated using euler angles and the lookAt matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 GetProjectionMatrix()
	{
		return projectionMatrice;
	}

	void UpdateCameraProjectionMatrix()
	{
		if (lensType == PERSPECTIVE)
			projectionMatrice = glm::perspective(glm::radians(FieldOfView), (float)screenWidth / (float)screenHeight, nearPlane, farPlane);
		else
			projectionMatrice = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
	}
	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM( to abstract it from windowing system
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		if(camState)
		camState->ProcessKeyboard(direction, deltaTime);
	}

	// processes input received from a mouse input sytem. Expects the offset value in both the x and y direction
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		if (camState)
			camState->ProcessMouseMovement(xoffset, yoffset, constrainPitch);
	}


	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (camState)
			camState->ProcessMouseScroll(yoffset);
	}

	// Orthographic / Perspective
	void setLensType(Camera_ProjectionMode mode)
	{
		lensType = mode;
		UpdateCameraProjectionMatrix();
	}

	void Draw(Shader* shader) override;
	void SetObjectLocation(glm::vec3 newLocation) override;
	void Select()override;
	void DeSelect()override;

	// ------------------------- Orbiting Camera ---------------------------------

	// Rotate "horizontal"
	void rotateAzimuth(float radians) {
		azimuthAngle += radians;
		const float fullCircle = 2.0f * glm::pi<float>();
		azimuthAngle = fmodf(azimuthAngle, fullCircle);
		if (azimuthAngle < 0.0f) {
			azimuthAngle = fullCircle + azimuthAngle;
		}
	}
	// Rotate "vertical"
	void rotatePolar(float radians) {
		polarAngle += radians;
		const float polarCap = glm::pi<float>() / 2.0f - 0.001f;
		if (polarAngle > polarCap)
			polarAngle = polarCap;
		if (polarAngle < -polarCap)
			polarAngle = -polarCap;
	}

	// new center for Orbiting Camera
	void setCenter(const glm::vec3 newCenter) {
		center = newCenter;
		UpdateCameraVectors();
	}

	// moves Orbiting camera horizontally moving the orbit center with it
	void moveHorizontal(float distance) {
		glm::vec3 strafeVector = glm::normalize(glm::cross(Front, Up));
		center += strafeVector * distance;
	}

	// moves Orbiting camera vertically moving the orbit center with it
	void moveVertical(float distance) {
		center += Up * distance;
	}
	// -----------------------------------------------------------------------------
private:
	void UpdateCameraVectors()
	{
		if (camState)
			camState->UpdateCameraVectors();
	}

	CameraState* camState;
};