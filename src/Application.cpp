#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<assimp/Importer.hpp>

#include "Camera.h"
//#include "Shader.h"	// includes glad/glad.h

#include <stb_image.h>

#include "model.h"
#include "resourcemanager.h"
#include "renderer.h"
#include "simpleModel.h"

#include <GLFW/glfw3.h>
// RUN THIS ON X64
// This is a test change

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void renderPlane(); unsigned int planeVBO = 0, planeVAO = 0;
void renderCube();	unsigned int cubeVAO = 0, cubeVBO = 0;
void renderSphere();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int SRC_WIDTH = 1600;
int SRC_HEIGHT = 900;

int SCREEN_RES_W = 2560;
int SCREEN_RES_H = 1440;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Camera
Camera* camera = new Camera(glm::vec3(-4.4f, 3.1f, 4.6f), glm::vec3(0.0f, 1.0f, 0.0f), -46.0f, -26.5f);

float lastX = (float)SRC_WIDTH / 2.f, lastY = (float)SRC_HEIGHT / 2.f;
bool bFirstMouse = true;
bool bViewPortActive = true;
int selectedShape = 1;

//float location[3] = { 0.0,0.0,0.0 };
glm::vec3 location = glm::vec3(0.0f);
glm::vec3 rotation = glm::vec3(0.0f);
glm::vec3 scale = glm::vec3(1.0f);

unsigned int colorRender = 0;

enum userState
{
	Normal,
	Move,
	Scale,
	Rotate
};

float interactX = lastX;

userState uState = Normal;

int main(void)
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "ImGui + GLFW", NULL, NULL);
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Load GLAD so it configures OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = SRC_WIDTH, y = SRC_HEIGHT
	glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
   //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//---------------------------------- IF WE WANT CUSTOM FONTS WE SHOULD LOAD THEM HERE ---------------------------------------
	// 	   EXAMPLE
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);


	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	//stencil testing
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_CULL_FACE);


	// Variables to be changed in the ImGUI window
	bool bDirLightToggle = true;
	bool bVertexNormalToggle = false;
	float size = 1.0f;
	float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };
	//TODO : encapsualte geometry data
	//float scale[3] = { 1.0,1.0,1.0 };
	//float rotation[3] = { 0.0,0.0,0.0 };
	//float lightDirection[3] = { -2.2, -4.0, -3.3 };

	//Shader unlit("src/shaders/unlit/unlit.vert", "src/shaders/unlit/unlit.frag");
	//Shader dirLight("src/shaders/DirLight/dirLight.vert", "src/shaders/DirLight/dirLight.frag");
	//Shader visNormals("src/shaders/visualizeNormals/visualizeNormals.vert", "src/shaders/visualizeNormals/visualizeNormals.frag", "src/shaders/visualizeNormals/visualizeNormals.geom");
	//Shader guideGrid("src/shaders/grid/gridGuide.vert", "src/shaders/grid/gridGuide.frag");

	ResourceManager::LoadShader("unlit", "src/shaders/unlit/unlit.vert", "src/shaders/unlit/unlit.frag", nullptr);
	ResourceManager::LoadShader("dirLight", "src/shaders/DirLight/dirLight.vert", "src/shaders/DirLight/dirLight.frag", nullptr);
	ResourceManager::LoadShader("visNormals", "src/shaders/visualizeNormals/visualizeNormals.vert", "src/shaders/visualizeNormals/visualizeNormals.frag", "src/shaders/visualizeNormals/visualizeNormals.geom");
	ResourceManager::LoadShader("guideGrid", "src/shaders/grid/gridGuide.vert", "src/shaders/grid/gridGuide.frag", nullptr);
	ResourceManager::LoadShader("albedo", "src/shaders/DirLight/dirLight.vert", "src/shaders/albedo/albedo.frag", nullptr);
	ResourceManager::LoadShader("outline", "src/shaders/outline/outline.vert", "src/shaders/outline/outline.frag", nullptr);
	std::vector<std::string> shapes{ "PLANE", "CUBE", "SPHERE"};

	//camera = new Camera(glm::vec3(-4.4f, 3.1f, 4.6f), glm::vec3(0.0f, 1.0f, 0.0f), -46.0f, -26.5f);

	//--------------------ASSIMP-------------------
	//Model bpModel("src/assets/backpack/backpack.obj");

	//----------------------------------------------------- CREATE FRAMEBUFFER ----------------------------------------------------------
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// generate texture that our framebuffer can write to
	unsigned int textureColorBuffer;
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_RES_W, SCREEN_RES_H, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach texture to fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	unsigned int selectionColorBuffer;
	glGenTextures(1, &selectionColorBuffer);
	glBindTexture(GL_TEXTURE_2D, selectionColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_RES_W, SCREEN_RES_H,0, GL_RGBA, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	//attach selection texture as color attachment 1
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, selectionColorBuffer, 0);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, attachments);

	// Renderbuffer for depth and stencil
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_RES_W, SCREEN_RES_H);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: framebuffer not complete" << std::endl;
		return -1;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//-----------------------------------------------------------------------------------------------------

	bool bTest = true;

	Shader* unlit = ResourceManager::GetShader("unlit");
	Shader* dirLight = ResourceManager::GetShader("dirLight");
	Shader* visNormals = ResourceManager::GetShader("visNormals");
	Shader* guideGrid = ResourceManager::GetShader("guideGrid");
	Shader* albedo = ResourceManager::GetShader("albedo");
	Shader* outline = ResourceManager::GetShader("outline");
	


	// --------------------------------- LOAD TEXTURE ----------------------------------------------
	ResourceManager::LoadTexture2D("src/assets/images/container.png", true, "albedo");
	Texture2D albedoTexture = ResourceManager::GetTexture2D("albedo");
	// ==================================================================================================

	Renderer::SetCamera(camera);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ----------------------------------------------------------------- RENDER OPENGL----------------------------------------------------------------------

		processInput(window);
		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// try rendering to custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, SCREEN_RES_W, SCREEN_RES_H);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, selectionColorBuffer);

		//--------------------------------------------------------------------------------------------------------------------------------
		Object* currentObject = ResourceManager::GetSelectedObject();
		if (currentObject)
		{
			location = currentObject->GetObjectLocation();
			rotation = currentObject->GetObjectRotation();

			Cube* currentCube = static_cast<Cube*>(currentObject);
			if (currentCube)
				scale = currentCube->GetCubeScale();
		}

		////DIRLIGHT
		//if (bDirLightToggle) {
			//lightshade
			albedo->Use();
			albedo->setVec3("light.direction", glm::vec3(camera->Front));
			albedo->setVec3("viewPos", camera->Position);
			//light properties
			albedo->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
			albedo->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
					
		////bpModel.Draw(unlit);
		Renderer::RenderScene();
		


		// bind Texture
		glActiveTexture(GL_TEXTURE0);
		albedoTexture.Bind();

		Renderer::RenderScene();

		//---------------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------------------------------------------------
		//----------------------------------------------------------- IMGUI -------------------------------------------------------------

		// set back to regular framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glViewport(0, 0, SRC_WIDTH, SRC_HEIGHT);


		// -----------------------------------------------CREATE THE MAIN IMGUI WINDOW THAT ALL OTHER WINDOWS WILL BE DOCKED IN----------------------------------------
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollbar;

		ImGui::Begin("SimpleModelingTool");
		ImGui::End();
		ImGui::PopStyleVar(2);
		// --------------------------------------------------------------------------------------------------------------------------------------------------------------

		// ImGUI window creation
		ImGui::Begin("ToolBar");
		// Text that appears in the window
		ImGui::Text("Hello there adventurer!");
		// Checkbox that appears in the window
		ImGui::Checkbox("DirLight Toggle", &bDirLightToggle);
		// visualize vertex normals
		ImGui::Checkbox("VertexNormals Toggle", &bVertexNormalToggle);
		// Lighd Direction
		//ImGui::DragFloat3("Light Direction", lightDirection);
		// Slider that appears in the window
		ImGui::SliderFloat("Size", &scale[0], 0.5f, 2.0f);
		// Fancy color editor that appears in the window
		ImGui::ColorEdit4("Color", color);
		// Fancy editor Position
		if (ImGui::DragFloat3("Location", &location[0]))
		{
			if (currentObject)
				currentObject->SetObjectLocation(location);
		}
		// Fancy editor Scale
		if (ImGui::DragFloat3("Scale", &scale[0]))
		{
			if (currentObject)
			{
				Cube* currentCube = static_cast<Cube*>(currentObject);
				if (currentCube)
					currentCube->SetCubeScale(scale);
			}
		}
		// Fancy editor Rotation
		if (ImGui::DragFloat3("Rotation", &rotation[0]))
		{
			if (currentObject)
				currentObject->SetObjectRotation(rotation);
		}
		// Ends the window

		ImGui::End();

		{
			ImGui::Begin("AddPrimitiveShapes");
			if (ImGui::TreeNode("Shapes"))
			{
				for (int i = 0; i != 3; i++)
				{
					ImGui::PushID(i);
					const char* selID = shapes[i].c_str();
					if (ImGui::Selectable(selID))
					{
						selectedShape = i;
						std::cout << selectedShape << std::endl;
						ResourceManager::CreateCube();
					}

					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::End();
		}

		{
			ImGui::Begin("SelectObject");
			if (ImGui::TreeNode("Objects"))
			{
				for (std::pair<const unsigned int , Object*>& current : ResourceManager::objectMap)
				{
					ImGui::PushID(current.first);
					const char* selID = current.second->objectName.c_str();
					if (ImGui::Selectable(selID))
					{
						ResourceManager::SelectObject(current.first);
					}

					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::End();
		}

		bool bf = true;


	if (ImGui::Begin("ViewPortTest",&bf , ImGuiWindowFlags_NoScrollbar))
	{
		ImVec2 size = ImGui::GetWindowSize();
		if(colorRender == 0)
		ImGui::Image((ImTextureID)textureColorBuffer,size,ImVec2(0,1),ImVec2(1,0));
		else
			ImGui::Image((ImTextureID)selectionColorBuffer, size, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}

		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
	  // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	  //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete all the objects we've created
	// 
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
// _________________________________________________________________________________
// ------------------------------------- END ---------------------------------------
// _________________________________________________________________________________

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SRC_WIDTH = width;
	SRC_HEIGHT = height;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(static_cast<float>(yoffset));
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		bViewPortActive = !bViewPortActive;

		if (bViewPortActive)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		ResourceManager::SelectNextObject();

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (colorRender == 0)
			colorRender = 1;
		else
			colorRender = 0;
	}
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	//set viewPortActive via TAB, which is a key callback
	if (bViewPortActive)
	{
		if (uState == Normal)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera->ProcessKeyboard(FORWARD, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera->ProcessKeyboard(BACKWARD, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera->ProcessKeyboard(LEFT, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera->ProcessKeyboard(RIGHT, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
			{
				uState = Move;
				interactX = lastX;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}

		if (uState == Move)
		{
			if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				uState = Normal;
			}
		}
	}
	
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	// 1	calc the mouse offset since last frame
	// 2	add the offset values to the camera's yaw and pitch values.
	// 3	add some constraints to the min/max pitch values.
	// 4	calculate the direction vector

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (bFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		bFirstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;		// reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (uState == Normal && bViewPortActive)
	{
		camera->ProcessMouseMovement(xoffset, yoffset);
	}

	if (uState == Move && bViewPortActive)
	{
		xoffset *= 0.1f;
		location[0] += xoffset;
	}
}

void renderPlane()
{
	if (planeVAO == 0)
	{
		float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f
		};
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(planeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = static_cast<unsigned int>(indices.size());

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		unsigned int stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}