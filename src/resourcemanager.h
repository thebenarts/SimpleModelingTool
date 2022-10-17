#pragma once

#include <map>
#include <string>

#include <glad/glad.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <queue>
#include <deque>
#include "texture.h"
#include "shader_m.h"
#include "object.h"
#include "point_light.h"
#include "spotLight.h"
#include "Camera.h"

// RESOURCE MANAGER Stores all Shaders, Textures
class ICommand;

#define UNDO_STACK_MAXSIZE 10

// static singleton resourcemanager
class ResourceManager
{

public:
	// unique ID incremented on every object creation
	static unsigned int resourceID;
	// currently selected ID
	static unsigned int selectedID;
	// unique ID for pointLights
	static unsigned int pointLightID;
	// unique ID for spotLights
	static unsigned int spotLightID;

	// resource storage
	static std::map<std::string, Shader*> shadersMap;
	static std::map<std::string, Texture2D*> texture2DMap;
	static std::vector<Object*>objects;
	static std::vector<PointLight*> pointLights;
	static std::vector<SpotLight*> spotLights;
	static std::priority_queue<unsigned int, std::deque<unsigned int>, std::greater<unsigned int>> freeObjectID;
	static std::priority_queue<unsigned int, std::deque<unsigned int>, std::greater<unsigned int>> freePointLightID;
	static std::priority_queue<unsigned int, std::deque<unsigned int>, std::greater<unsigned int>> freeSpotLightID;
	static Camera* currentCamera;
	static Camera* defaultCamera;

	static std::deque<ICommand*> undoStack;
	static std::deque<ICommand*> redoStack;

	// loads (and generates) a shader program from file loading vertex, fragment (and geometry) shadrer's source code.
	static Shader* LoadShader(std::string name, const char* vShaderfile, const char* fShaderFile, const char* gShaderFile);
	// retrieves stored shader
	static Shader* GetShader(std::string name);
	// loads (and generates) a texture2D from file
	static Texture2D* LoadTexture2D(const char* file, bool alpha, std::string name);
	// retrieves stored texture2D
	static Texture2D* GetTexture2D(std::string name);

	// properly de-allocates all loaded resources
	static void Clear();
	
	//------------------------------------------- RESOURCE ID`s-----------------------------------------------------
	static unsigned int GetResourceID();
	static unsigned int GetandAddResourceID(Object* inObject);
	static unsigned int GetandAddPointLightID(PointLight* light);
	static unsigned int GetandAddSpotLightID(SpotLight* light);
	static void AddResourceID(Object* inObject);
	static void AddPointID(PointLight* inObject);
	static void AddSpotID(SpotLight* inObject);
	static void RemoveResourceID(unsigned int inID);
	static void RemovePointID(unsigned int inID);
	static void RemoveSpotID(unsigned int inID);
	static void FreeResourceID(unsigned int inID);
	static void FreePointID(unsigned int inID);
	static void FreeSpotID(unsigned int inID);
	static void RemoveObject();
	//------------------------------------------- OBJECT SELECTION-----------------------------------------------------
	static unsigned int GetSelectedID() { return selectedID; }
	static Object* GetSelectedObject();
	static unsigned int SelectNextObject();
	static unsigned int SelectObject(unsigned int objectID);
	static Object* GetObject(unsigned int objectID);
	static void SetViewportCamera(Camera* camera);

	//------------------------------------------- OBJECT CREATION AND DESTRUCTION-----------------------------------------------------
	//static void RemoveObject();
	static void CreateCube();
	static void CreateLight();
	static void CreatePointLight();
	static void CreateSpotLight();
	static Camera* CreateCamera();


	//------------------------------------------- UNDO AND REDO-----------------------------------------------------
	static void AddUndo(ICommand* inCommand);
	static void AddRedo(ICommand* inCommand);
	static void ClearRedo();
	static void RegisterNewCommand(ICommand* inCommand);
	static void HandleUndoCommand();
	static void HandleRedoCommand();

private:
	// private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available(static)
	ResourceManager(){}

	// loads and generates a shader from file
	static Shader* loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

	// loads a single texture from file
	static Texture2D* loadTextureFromFile(const char* file, bool alpha);

	static unsigned int FindNextObjectID();
};