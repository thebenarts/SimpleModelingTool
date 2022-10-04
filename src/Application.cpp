#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"			// includes glad/glad.h
#include "stb_image.h"

#include <GLFW/glfw3.h>
// RUN THIS ON X64
// This is a test change
//hello there
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void renderShape();
void renderPlane(); unsigned int planeVBO = 0, planeVAO = 0;
void renderCube();	unsigned int cubeVAO = 0, cubeVBO = 0;
void renderSphere();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
int SRC_WIDTH = 1600;
int SRC_HEIGHT = 900;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Camera
//Camera camera(glm::vec3(0.0, 0.0, 3.0));
Camera camera(glm::vec3(-4.4f, 3.1f, 4.6f), glm::vec3(0.0f, 1.0f, 0.0f), -46.f, -26.5f);
float lastX = (float)SRC_WIDTH / 2.f, lastY = (float)SRC_HEIGHT / 2.f;
bool bFirstMouse = true;
bool bViewPortActive = true;
int selectedShape = 1;

float location[3] = { 0.0,0.0,0.0 };

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

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Variables to be changed in the ImGUI window
	bool bDirLightToggle = true;
	bool bVertexNormalToggle = true;
	float size = 1.0f;
	float color[4] = { 0.8f, 0.3f, 0.02f, 1.0f };
	//TODO : encapsualte geometry data
	float scale[3] = { 1.0,1.0,1.0 };
	float rotation[3] = { 0.0,0.0,0.0 };
	//float lightDirection[3] = { -2.2, -4.0, -3.3 };

	Shader unlit("src/shaders/unlit/unlit.vert", "src/shaders/unlit/unlit.frag");
	// Directional light shader
	Shader dirLight("src/shaders/DirLight/dirLight.vert", "src/shaders/DirLight/dirLight.frag");
	Shader visNormals("src/shaders/visualizeNormals/visualizeNormals.vert", "src/shaders/visualizeNormals/visualizeNormals.frag", "src/shaders/visualizeNormals/visualizeNormals.geom");
	Shader guideGrid("src/shaders/grid/gridGuide.vert", "src/shaders/grid/gridGuide.frag");

	std::vector<std::string> shapes{ "PLANE", "CUBE", "SPHERE" };

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(window);
		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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
		ImGui::SliderFloat("Size", &size, 0.5f, 2.0f);
		// Fancy color editor that appears in the window
		ImGui::ColorEdit4("Color", color);
		// Fancy editor Position
		ImGui::DragFloat3("Location", location);
		// Fancy editor Scale
		ImGui::DragFloat3("Scale", scale);
		// Fancy editor Rotation
		ImGui::DragFloat3("Rotation", rotation);
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
					}

					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::End();
		}

		glm::mat4 projection = glm::perspective(glm::radians(camera.FieldOfView), (float)SRC_WIDTH / (float)SRC_HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(location[0], location[1], location[2]));
		model = glm::scale(model, glm::vec3(scale[0], scale[1], scale[2]));
		model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0, 0, 1));

		// use shader
		unlit.use();
		unlit.setMat4("projection", projection);
		unlit.setMat4("view", view);
		unlit.setMat4("model", model);

		//DIRLIGHT
		// --------
		if(bDirLightToggle){
			//lightshade
			dirLight.use();
			//dirLight.setVec3("light.direction", glm::vec3(lightDirection[0], lightDirection[1], lightDirection[2]));
			// I AM THE DIRLIGHT
			dirLight.setVec3("light.direction", glm::vec3(camera.Front));
			dirLight.setVec3("viewPos", camera.Position);
		
			//light properties
			dirLight.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
			dirLight.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		
			dirLight.setMat4("projection", projection);
			dirLight.setMat4("view", view);
			dirLight.setMat4("model", model);
		}


		// set matrice information in shader

		renderShape();

		//--------------------------	vis Normals
		if (bVertexNormalToggle)
		{
			visNormals.use();
			visNormals.setMat4("view", view);
			visNormals.setMat4("model", model);
			visNormals.setMat4("projection", projection);
			renderShape();
		}

		guideGrid.use();
		guideGrid.setMat4("projection", projection);
		guideGrid.setMat4("view", view);
		renderPlane();
	

		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SRC_WIDTH = width;
	SRC_HEIGHT = height;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		bViewPortActive = !bViewPortActive;

		if(bViewPortActive)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	}
	if (bViewPortActive)
	{
		if (uState == Normal)
		{
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProcessKeyboard(FORWARD, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProcessKeyboard(BACKWARD, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProcessKeyboard(LEFT, deltaTime);

			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProcessKeyboard(RIGHT, deltaTime);

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
		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	if (uState == Move && bViewPortActive)
	{
		xoffset *= 0.1f;
		location[0] += xoffset;

	}
}

void renderShape()
{
	switch (selectedShape)
	{
	case 0:
		renderPlane();
		break;
	case 1: 
		renderCube();
		break;
	case 2:
		renderSphere();
		break;
	default:
		renderCube();
		break;
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