#include "shader_m.h"

#include <iostream>

Shader::Shader()
{

}

Shader& Shader::Use()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::checkCompileErrors(unsigned int object, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -----------------------------------------------------------------------------------"
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER: Link-Time error: Type: " << type << "\n"
				<< infoLog << "\n ------------------------------------------------------------------------------------"
				<< std::endl;
		}
	}
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
	unsigned int sVertex, sFragment, gShader;
	// vertex shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");
	// fragment shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");
	// if geometry shader source code is given, also copmile geometry shader
	if (geometrySource != nullptr)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
		glCompileShader(gShader);
		checkCompileErrors(gShader, "GEOMETRY");
	}
	// shader program 
	ID = glCreateProgram();
	glAttachShader(ID, sVertex);
	glAttachShader(ID, sFragment);
	if (geometrySource != nullptr)
		glAttachShader(ID, gShader);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they are linked into our program now and no longer necessary 
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr)
		glDeleteShader(gShader);

}

void Shader::setBool(const std::string& name, bool value, bool useShader)
{
	if (useShader)
		Use();

	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value, bool useShader)
{
	if (useShader)
		Use();

	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value, bool useShader)
{
	if (useShader)
		Use();

	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value, bool useShader)
{
	if (useShader)
		Use();

	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string& name, float x, float y, bool useShader)
{
	if (useShader)
		Use();

	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value, bool useShader)
{
	if (useShader)
		Use();

	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float x, float y, float z, bool useShader)
{
	if (useShader)
		Use();

	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value, bool useShader)
{
	if (useShader)
		Use();

	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w, bool useShader)
{
	if (useShader)
		Use();

	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& value, bool useShader)
{
	if (useShader)
		Use();

	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(const std::string& name, const glm::mat3& value, bool useShader)
{
	if (useShader)
		Use();

	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& value, bool useShader)
{
	if (useShader)
		Use();

	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setPVM(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, bool useShader)
{
	
}

