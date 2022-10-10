#pragma once
#include <string>

#include<glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	//the program ID
	unsigned int ID;

	Shader();

	~Shader() { glDeleteProgram(ID); }

	Shader& Use();

	void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);

	void setBool(const std::string& name, bool value, bool useShader = false);
	void setInt(const std::string& name, int value, bool useShader = false);
	void setUInt(const std::string& name, unsigned int value, bool useShader = false);
	void setFloat(const std::string& name, float value, bool useShader = false);
	void setVec2(const std::string& name, const glm::vec2& value, bool useShader = false);
	void setVec2(const std::string& name, float x, float y, bool useShader = false);
	void setVec3(const std::string& name, const glm::vec3& value, bool useShader = false);
	void setVec3(const std::string& name, float x, float y, float z, bool useShader = false);
	void setVec4(const std::string& name, const glm::vec4& value, bool useShader = false);
	void setVec4(const std::string& name, float x, float y, float z, float w, bool useShader = false);
	void setMat2(const std::string& name, const glm::mat2& value, bool useShader = false);
	void setMat3(const std::string& name, const glm::mat3& value, bool useShader = false);
	void setMat4(const std::string& name, const glm::mat4& value, bool useShader = false);
	void setPVM(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, bool useShader = false);

private:
	void checkCompileErrors(unsigned int object, std::string type);
};