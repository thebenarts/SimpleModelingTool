#pragma once

#include<vector>
#include<iostream>
#include "resourcemanager.h"
#include "light.h"

Light::Light()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],GL_DYNAMIC_DRAW);
	// link vertex attributes
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	texture = ResourceManager::GetTexture2D("light");
	billboardTransform = glm::mat4(1.0f);
}


void Light::Draw(Shader* shader)
{

	shader = ResourceManager::GetShader("billboard");

	if (!shader) {
		std::cout << "SHADER::ERROR on object::Draw Id: " << objectID << std::endl;
	}

	//Update();

	shader->Use();
	shader->setMat4("projection", ResourceManager::currentCamera->GetProjectionMatrix());
	glm::mat4 view = ResourceManager::currentCamera->GetViewMatrix();
	shader->setMat4("view", view);
	shader->setVec3("CameraRight_ws", glm::vec3(view[0][0], view[1][0], view[2][0]));
	shader->setVec3("CameraUp_ws", glm::vec3(view[0][1], view[1][1], view[2][1]));
	shader->setVec2("size", glm::vec2(0.5, 0.5));

	shader->setMat4("model", billboardTransform);
	shader->setVec3("location", location);
	shader->setUInt("resourceID", objectID);

	if (texture)
	{
		glActiveTexture(GL_TEXTURE0);
		texture->Bind();
		shader->setInt("albedo", 0);
	}

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	texture->unBind();
}

void Light::Update()
{
	Camera* camera = ResourceManager::currentCamera;

	glm::vec3 directionFromCamera = location - camera->Position;
	float theta( glm::atan(directionFromCamera.y, directionFromCamera.x) );
	float distance2D(glm::sqrt(directionFromCamera.x * directionFromCamera.x + directionFromCamera.y * directionFromCamera.y));
	float phi(glm::atan(-directionFromCamera.z, distance2D));

	billboardTransform = glm::mat4(1.0f);
	billboardTransform = glm::translate(billboardTransform, location);
	//billboardTransform = glm::rotate(billboardTransform, phi, glm::vec3(0, 1, 0));
	//billboardTransform = glm::rotate(billboardTransform, theta, glm::vec3(0, 0, 1));
	billboardTransform = glm::rotate(billboardTransform, theta, glm::vec3(0,0,-1));
	billboardTransform = glm::rotate(billboardTransform, phi, glm::vec3(0, 1, 0));
}