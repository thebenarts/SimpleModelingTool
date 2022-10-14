#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<iostream>
#include <vector>
#include "object.h"

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

// Default camera values
const float YAW = -90.f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// An abstract camera class that processses input and calculates the corresponding Euler Angles, vectors and matrices for use in OpenGL

class Camera : public Object
{
public:
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
		lensType(PERSPECTIVE)
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
		lensType(projectionType)
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
		lensType(projectionType)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraProjectionMatrix();
		UpdateCameraVectors();
		objectName = "Camera" + std::to_string(objectID);
	}

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
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;

		location = Position;
	}

	// processes input received from a mouse input sytem. Expects the offset value in both the x and y direction
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.f)
				Pitch = 89.f;
			if (Pitch < -89.f)
				Pitch = -89.f;
		}

		// update Front, Right and Up vectors using the updated Euler angles
		UpdateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		FieldOfView -= (float)yoffset;
		if (FieldOfView < 1.0f)
			FieldOfView = 1.0f;
		if (FieldOfView > 100.0f)
			FieldOfView = 100.0f;

		if(lensType == PERSPECTIVE)
		UpdateCameraProjectionMatrix();
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
private:
	void UpdateCameraVectors()
	{
		// calculate the new front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		// also re-calculate the right and up vetor
		Right = glm::normalize(glm::cross(Front, WorldUp));	// Normalize the vectors, because their lenght gets closer to - the more you look up or down which results in slower movement
		Up = glm::normalize(glm::cross(Right, Front));
	}
};