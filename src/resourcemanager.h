#pragma once

#include <map>
#include <string>

#include <glad/glad.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "texture.h"
#include "shader_m.h"
#include "object.h"

// RESOURCE MANAGER Stores all Shaders, Textures


// static singleton resourcemanager
class ResourceManager
{

public:
	// unique ID incremented on every object creation
	static unsigned int resourceID;
	// currently selected ID
	static unsigned int selectedID;
	// resource storage
	static std::map<std::string, Shader*> shadersMap;
	static std::map<std::string, Texture2D> texture2DMap;
	static std::map<unsigned int, Object*> objectMap;
	// loads (and generates) a shader program from file loading vertex, fragment (and geometry) shadrer's source code.
	static Shader* LoadShader(std::string name, const char* vShaderfile, const char* fShaderFile, const char* gShaderFile);
	// retrieves stored shader
	static Shader* GetShader(std::string name);
	// loads (and generates) a texture2D from file
	//static Texture2D LoadTexture2D(const char* file, bool alpha, std::string name);
	// retrieves stored texture2D
	//static Texture2D& GetTexture2D(std::string name);

	// properly de-allocates all loaded resources
	static void Clear();
	
	static unsigned int getResourceID() { return resourceID++; }

	static unsigned int getSelectedID() { return selectedID; }

	static void CreateCube();

private:
	// private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available(static)
	ResourceManager(){}

	// loads and generates a shader from file
	static Shader* loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

	// loads a single texture from file
	//static Texture2D loadTextureFromFile(const char* file, bool alpha);
};