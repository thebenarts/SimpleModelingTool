#pragma once
#include "simpleModel.h"
#include <iostream>

Cube::Cube()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	scale = glm::vec3(1.0f);
}

void Cube::Draw(Shader* shader) {
	if (!shader) {
		std::cout << "SHADER::ERROR on object::Draw Id: " << objectID << std::endl;
	}
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, location);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0, 0, 1));
	shader->Use();
	shader->setMat4("model", model);

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	
	// not true yet
	// glDrawArrays is gonna be called from Renderer


}

void Cube::SetCubeScale(glm::vec3 newScale)
{
	scale = newScale;
}

glm::vec3 Cube::GetCubeScale()
{
	return scale;
}