/*
The code template was taken from https://learnopengl.com/
*/

#pragma once

#include "ShaderManager.h"
#include "CameraManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

class GLCore
{
public:
	static CameraManager cameraManager;

	static float last_x;
	static float last_y;
	static bool first_mouse;

	static const unsigned int SCR_WIDTH = 1400;
	static const unsigned int SCR_HEIGHT = 900;

	GLCore();
	void RenderLoop();

private:
	ShaderManager platform_shader_manager;
	ShaderManager skybox_shader_manager;

	Terrain terrain = Terrain(1,1);
    
	float delta_time = 0.0f;
	float last_frame = 0.0f;
	
	GLFWwindow* window;
	unsigned int platform_VAO;
    unsigned int platform_VBO;
    unsigned int platform_EBO;

	unsigned int skybox_VAO;
    unsigned int skybox_VBO;

	void ProcessInput(bool start = false);

	void BindObjects();

	void static FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	void static MouseCallback(GLFWwindow* window, double xpos_in, double ypos_in);
    void static ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    void DrawPlatforms();
	void DrawSkybox();
};

