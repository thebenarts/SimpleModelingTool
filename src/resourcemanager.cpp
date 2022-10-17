#include "resourcemanager.h"

#include <iostream>
#include <sstream>
#include <fstream>


#include <stb_image.h>
#include "simpleModel.h"

#include "renderer.h"
#include "command.h"

// Instantiate static variables

std::map<std::string, Texture2D*> ResourceManager::texture2DMap;
std::map<std::string, Shader*> ResourceManager::shadersMap;
std::vector<Object*>ResourceManager::objects;
std::priority_queue<unsigned int, std::deque<unsigned int>, std::greater<unsigned int>>ResourceManager::freeObjectID;
std::priority_queue<unsigned int, std::deque<unsigned int>, std::greater<unsigned int>>ResourceManager::freePointLightID;
std::priority_queue<unsigned int, std::deque<unsigned int>, std::greater<unsigned int>>ResourceManager::freeSpotLightID;
std::vector<PointLight*> ResourceManager::pointLights(50, nullptr);
std::vector<SpotLight*> ResourceManager::spotLights(50, nullptr);
Camera* ResourceManager::currentCamera;
Camera* ResourceManager::defaultCamera;

unsigned int ResourceManager::resourceID = 0;
unsigned int ResourceManager::selectedID = 0;
unsigned int ResourceManager::pointLightID = 0;
unsigned int ResourceManager::spotLightID = 0;

std::deque<ICommand*>ResourceManager::undoStack;
std::deque<ICommand*>ResourceManager::redoStack;


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


Texture2D* ResourceManager::GetTexture2D(std::string name)
{
	return texture2DMap[name];
}

Texture2D* ResourceManager::LoadTexture2D(const char* file, bool alpha, std::string name)
{
	texture2DMap[name] = loadTextureFromFile(file, alpha);
	return texture2DMap[name];
}

Texture2D* ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
	// create texture object
	Texture2D* texture = new Texture2D();
	if (alpha)
	{
		texture->Internal_Format = GL_RGBA;
		texture->Image_Format = GL_RGBA;
	}

	// load image 
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	// now generate texture
	texture->Generate(width, height, data);
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
		glDeleteTextures(1, &iter.second->ID);
}

void ResourceManager::CreateCube()
{
	Cube* cObject = new Cube();
	SelectObject(cObject->objectID);
	new CreateCommand(cObject);
}
void ResourceManager::CreateLight()
{
	PointLight* light = new PointLight();
	SelectObject(light->objectID);
	new CreateCommand(light);
}

Camera* ResourceManager::CreateCamera()
{
	Camera* camera = new Camera(glm::vec3(-4.4f, 3.1f, 4.6f), glm::vec3(0.0f, 1.0f, 0.0f), -46.0f, -26.5f);
	SelectObject(camera->objectID);
	return camera;
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
	{
		if (ResourceManager::objects[0])
			return ResourceManager::selectedID = 0;
	}
	else
		++ResourceManager::selectedID;

	if (!ResourceManager::objects[selectedID])
	{
		
		if (ResourceManager::selectedID > ResourceManager::resourceID)
		{
			if(ResourceManager::objects[0])
			return ResourceManager::selectedID = 0;
		}
		while (ResourceManager::selectedID < ResourceManager::objects.size())
		{
			if (ResourceManager::objects[ResourceManager::selectedID])
				return ResourceManager::selectedID;

			selectedID++;
		}
	}

	if (ResourceManager::selectedID >= ResourceManager::objects.size())
	{
		ResourceManager::selectedID = 0;
		return UINT_MAX;
	}

	if (!ResourceManager::objects[selectedID])
		return UINT_MAX;

	return ResourceManager::selectedID;
}

unsigned int ResourceManager::SelectNextObject()
{
	if (ResourceManager::selectedID <= ResourceManager::resourceID && ResourceManager::objects[ResourceManager::GetSelectedID()])
		ResourceManager::objects[ResourceManager::GetSelectedID()]->DeSelect();

	if (UINT_MAX == ResourceManager::FindNextObjectID())
	{
		std::cout << "OBJECT ID ERROR:: COULD NOT FIND OBJECT" << std::endl;
		return UINT_MAX;
	}

	// let the object know it's selected
	if (ResourceManager::selectedID < ResourceManager::objects.size() && ResourceManager::objects[ResourceManager::selectedID])
		ResourceManager::objects[ResourceManager::selectedID]->Select();

	return ResourceManager::selectedID;
}

unsigned int ResourceManager::SelectObject(unsigned int objectID)
{
	if (ResourceManager::objects.empty())
	{
		std::cout << "ERROR::COULD NOT FIND SELECTED OBJECT";
		return UINT_MAX;			// changed from 0 to UINT_MAX should be fine but be careful
	}

	if (objectID > ResourceManager::resourceID)
	{
		std::cout << "ERROR::COULD NOT FIND SELECTED OBJECT";
		return UINT_MAX;			// changed from 0 to UINT_MAX should be fine but be careful
	}

	if (ResourceManager::selectedID < ResourceManager::objects.size() && ResourceManager::objects[ResourceManager::selectedID])
		ResourceManager::objects[ResourceManager::selectedID]->DeSelect();

	// should probably check in freeID as well but for now it should be alright 
	if(ResourceManager::objects[objectID])
		ResourceManager::objects[objectID]->Select();
	ResourceManager::selectedID = objectID;

	return objectID;
}

Object* ResourceManager::GetObject(unsigned int objectID)
{
	if (ResourceManager::objects.empty() || objectID > ResourceManager::resourceID)
		return nullptr;

	if (ResourceManager::objects[objectID])
		return ResourceManager::objects[objectID];

	return nullptr;
}

//void ResourceManager::RemoveObject()
//{
//	Object* objectToRemove = GetSelectedObject();
//
//	if (objectToRemove == ResourceManager::defaultCamera)
//		return;
//
//	if (PointLight* pLight = dynamic_cast<PointLight*>(objectToRemove))
//	{
//		int dID = pLight->pointLightID;
//		pointLights[dID] = nullptr;
//		freePointLightID.push(dID);
//	}
//	else if(SpotLight* sLight = dynamic_cast<SpotLight*>(objectToRemove))
//	{
//		int dID = sLight->spotLightID;
//		spotLights[dID] = nullptr;
//		freeSpotLightID.push(dID);
//	}
//
//	if (objectToRemove)
//	{
//		int dID = objectToRemove->objectID;
//		freeObjectID.push(dID);
//		delete objectToRemove;
//		objects[dID] = nullptr;
//	}
//
//
//}

void ResourceManager::RemoveObject()
{
	Object* objectToRemove = GetSelectedObject();

	if (objectToRemove == ResourceManager::defaultCamera)
		return;

	new RemoveCommand(objectToRemove);
}

unsigned int ResourceManager::GetResourceID()
{
	if (ResourceManager::freeObjectID.empty())
		return resourceID++;

	unsigned int rID = ResourceManager::freeObjectID.top();
	freeObjectID.pop();

	return rID;
}

unsigned int ResourceManager::GetandAddResourceID(Object* inObject)
{
	if (ResourceManager::freeObjectID.empty())
	{
		ResourceManager::objects.push_back(inObject);
		return resourceID++;
	}

	unsigned int rID = ResourceManager::freeObjectID.top();
	freeObjectID.pop();

	ResourceManager::objects[rID] = inObject;
	return rID;
}

unsigned int ResourceManager::GetandAddPointLightID(PointLight* light) {
	if (ResourceManager::freePointLightID.empty()) 
	{
		ResourceManager::pointLights[pointLightID] = light;
		return pointLightID++;
	}
	unsigned int lightID = ResourceManager::freePointLightID.top();
	freePointLightID.pop();
	ResourceManager::pointLights[lightID] = light;
	return lightID;
}

unsigned int ResourceManager::GetandAddSpotLightID(SpotLight* light) {
	if (ResourceManager::freeSpotLightID.empty())
	{
		ResourceManager::spotLights[spotLightID]=light;
		return spotLightID++;
	}
	unsigned int lightID = ResourceManager::freeSpotLightID.top();
	freeSpotLightID.pop();
	ResourceManager::spotLights[lightID] = light;
	return lightID;
}
void ResourceManager::CreatePointLight()
{
	if (ResourceManager::freePointLightID.empty() && ResourceManager::pointLightID >= ResourceManager::pointLights.size()) 
	{
		std::cout << "ERROR::POINTLIGHT LIMIT REACHED (50)" << std::endl;
		return;
	}
	PointLight* light = new PointLight();
	SelectObject(light->objectID);
}

void ResourceManager::CreateSpotLight()
{
	if (ResourceManager::freeSpotLightID.empty() && ResourceManager::spotLightID >= ResourceManager::spotLights.size())
	{
		std::cout << "ERROR::SPOTLIGHT LIMIT REACHED (50)" << std::endl;
		return;
	}
	SpotLight* light = new SpotLight();
	SelectObject(light->objectID);
}

void ResourceManager::SetViewportCamera(Camera* camera)
{
	if (camera)
		ResourceManager::currentCamera = camera;
	else
		ResourceManager::currentCamera = ResourceManager::defaultCamera;


	Renderer::SetCamera(currentCamera);
}

void ResourceManager::AddUndo(ICommand* inCommand)
{
	if (ResourceManager::undoStack.size() == UNDO_STACK_MAXSIZE)
		undoStack.pop_front();

	undoStack.push_back(inCommand);
}

void ResourceManager::AddRedo(ICommand* inCommand)
{
	if (ResourceManager::redoStack.size() == UNDO_STACK_MAXSIZE)
		redoStack.pop_front();

	redoStack.push_back(inCommand);
}

void ResourceManager::ClearRedo()
{
	ResourceManager::redoStack.clear();
}

void ResourceManager::RegisterNewCommand(ICommand* inCommand)
{
	AddUndo(inCommand);
	ClearRedo();
}

void ResourceManager::HandleUndoCommand()
{
	if (ResourceManager::undoStack.size())
	{
		ICommand* currentCommand = ResourceManager::undoStack.back();
		if (!currentCommand)
		{
			std::cout << "ERROR::COMMAND WAS NULLPTR" << std::endl;
			return ;
		}
		currentCommand->Undo();
		ResourceManager::undoStack.pop_back();
		ResourceManager::AddRedo(currentCommand);
	}
}

void ResourceManager::HandleRedoCommand()
{
	if (ResourceManager::redoStack.size())
	{
		ICommand* currentCommand = ResourceManager::redoStack.back();
		if (!currentCommand)
		{
			std::cout << "ERROR::COMMAND WAS NULLPTR" << std::endl;
			return;
		}
		currentCommand->Execute();
		ResourceManager::redoStack.pop_back();
		ResourceManager::AddUndo(currentCommand);
	}
}

void ResourceManager::AddResourceID(Object* inObject)
{
	if (!inObject)
		return;

	if (!ResourceManager::objects[inObject->objectID])
		ResourceManager::objects[inObject->objectID] = inObject;
}

void ResourceManager::AddPointID(PointLight* inObject)
{
	if (!inObject)
		return;

	if (!ResourceManager::pointLights[inObject->pointLightID])
		ResourceManager::pointLights[inObject->pointLightID] = inObject;
}

void ResourceManager::AddSpotID(SpotLight* inObject)
{
	if (!inObject)
		return;

	if (!ResourceManager::spotLights[inObject->spotLightID])
		ResourceManager::spotLights[inObject->spotLightID] = inObject;
}

void ResourceManager::RemoveResourceID(unsigned int inID)
{
	if(inID < ResourceManager::objects.size())
		ResourceManager::objects[inID] = nullptr;
}

void ResourceManager::RemovePointID(unsigned int inID)
{
	ResourceManager::pointLights[inID] = nullptr;
}

void ResourceManager::RemoveSpotID(unsigned int inID)
{
	ResourceManager::spotLights[inID] = nullptr;
}