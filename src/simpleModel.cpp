#pragma once
#include "simpleModel.h"
#include <iostream>

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