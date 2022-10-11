#include "resourcemanager.h"

#include <iostream>
#include <sstream>
#include <fstream>


#include <stb_image.h>
#include "simpleModel.h"


// Instantiate static variables

std::map<std::string, Texture2D> ResourceManager::texture2DMap;
std::map<std::string, Shader*> ResourceManager::shadersMap;
std::map<unsigned int, Object*> ResourceManager::objectMap;
std::vector<Object*>ResourceManager::objects;
std::priority_queue<int, std::deque<int>, std::greater<int>>ResourceManager::freeID;

unsigned int ResourceManager::resourceID = 0;
unsigned int ResourceManager::selectedID = 0;

Shader* ResourceManager::GetShader(std::string name)
{
	return shadersMap[name];
}

Shader* ResourceManager::LoadShader(std::string name, const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	shadersMap[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return shadersMap[name];
}

Shader* ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
	// 1 retrieve the vertex/fragment (geometry) source code from filepath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);


		// read file's buffer contents into streams
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, load it
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

	// 2. now create shader object from source code
	Shader* shader = new Shader();
	shader->Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}


Texture2D& ResourceManager::GetTexture2D(std::string name)
{
	return texture2DMap[name];
}

Texture2D ResourceManager::LoadTexture2D(const char* file, bool alpha, std::string name)
{
	texture2DMap[name] = loadTextureFromFile(file, alpha);
	return texture2DMap[name];
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
	// create texture object
	Texture2D texture;
	if (alpha)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}

	// load image 
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	// now generate texture
	texture.Generate(width, height, data);
	// free image data
	stbi_image_free(data);

	return texture;
}

void ResourceManager::Clear()
{
	// properly delete all shaders
	for (auto iter : shadersMap)
		glDeleteProgram(iter.second->ID);
	// properly delete all textures
	for (auto iter : texture2DMap)
		glDeleteTextures(1, &iter.second.ID);
}

void ResourceManager::CreateCube()
{
	Cube* cObject = new Cube();
	objects.push_back(static_cast<Object*>(cObject));
	SelectNextObject();
}

Object* ResourceManager::GetSelectedObject()
{
	if (ResourceManager::objects.empty())
	{
		std::cout << "OBJECT MAP IS EMPTY:: NOTHING TO RETURN" << std::endl;
		return nullptr;
	}
	
	if (!ResourceManager::objects[ResourceManager::selectedID])
	{
		std::cout << "SELECTED ID : COULD NOT BE FOUND " << std::endl;
		if(UINT_MAX == SelectNextObject());
		return nullptr;
	}

	return ResourceManager::objects[ResourceManager::selectedID];
}

unsigned int ResourceManager::FindNextObjectID()
{
	if (objects.empty())
	{
		std::cout << "there is no item to select" << std::endl;
		return UINT_MAX;
	}

	if (ResourceManager::selectedID >= ResourceManager::objects.size() || ResourceManager::selectedID + 1 >= ResourceManager::objects.size())
		return ResourceManager::selectedID = 0;
	else
		++ResourceManager::selectedID;

	if (!ResourceManager::objects[selectedID])
	{
		++ResourceManager::selectedID;
		if (ResourceManager::selectedID > ResourceManager::resourceID)
			return ResourceManager::selectedID = 0;
		while (ResourceManager::selectedID < ResourceManager::resourceID)
		{
			if (ResourceManager::objects[ResourceManager::selectedID])
				return ResourceManager::selectedID;

			selectedID++;
		}
	}
	return ResourceManager::selectedID;
}

unsigned int ResourceManager::SelectNextObject()
{
	if (ResourceManager::selectedID <= ResourceManager::resourceID && ResourceManager::objects[ResourceManager::GetSelectedID()])
		ResourceManager::objects[ResourceManager::GetSelectedID()]->bSelected = false;

	if (UINT_MAX == ResourceManager::FindNextObjectID())
	{
		std::cout << "OBJECT ID ERROR:: COULD NOT FIND OBJECT" << std::endl;
		return UINT_MAX;
	}

	// let the object know it's selected
	if (ResourceManager::selectedID < ResourceManager::objects.size() && ResourceManager::objects[ResourceManager::selectedID])
		ResourceManager::objects[ResourceManager::selectedID]->bSelected = true;

	return ResourceManager::selectedID;
}

unsigned int ResourceManager::SelectObject(unsigned int objectID)
{
	if (ResourceManager::objects.empty())
	{
		std::cout << "ERROR::COULD NOT FIND SELECTED OBJECT";
		return 0;
	}

	if (objectID > ResourceManager::resourceID)
	{
		std::cout << "ERROR::COULD NOT FIND SELECTED OBJECT";
		return 0;
	}

	if (ResourceManager::selectedID < ResourceManager::objects.size() && ResourceManager::objects[ResourceManager::selectedID])
		ResourceManager::objects[ResourceManager::selectedID]->bSelected = false;

	// should probably check in freeID as well but for now it should be alright 
	if(ResourceManager::objects[objectID])
	ResourceManager::objects[objectID]->bSelected = true;
	ResourceManager::selectedID = objectID;

	return objectID;
}