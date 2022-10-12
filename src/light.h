#pragma once

#include "object.h"
#include "texture.h"
#include <vector>


class Light : public Object {
public:
	Light();

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
	
	Texture2D* texture;

	unsigned int VAO;
	unsigned int VBO;

	glm::mat4 billboardTransform;

	void Draw(Shader* shader) override;
	void Update();

private:
	std::vector<float> vertices = {
		// back face
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f
	};
};