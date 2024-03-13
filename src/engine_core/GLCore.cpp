#include "../../include/engine_core/GLCore.h"
#include "../../include/engine_core/ShaderManager.h"
#include "../../include/engine_core/Terrain.h"
#include "../../include/engine_core/Skybox.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

CameraManager GLCore::cameraManager = CameraManager();

float GLCore::last_x = GLCore::SCR_WIDTH / 2.0;
float GLCore::last_y = GLCore::SCR_HEIGHT / 2.0;
bool GLCore::first_mouse = true;

GLCore::GLCore()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    this->window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shell texturing", NULL, NULL);
    if (this->window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return;
    }
    
    glfwMakeContextCurrent(this->window);
    glfwSetFramebufferSizeCallback(this->window, this->FramebufferSizeCallback);
    glfwSetCursorPosCallback(this->window, this->MouseCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->platform_shader_manager = ShaderManager();
    this->platform_shader_manager.BuildShaders(
        (char*)"shaders/PlatformVertexShader.vs",
        (char*)"shaders/PlatformFragmentShader.fs",
        (char*)"shaders/PlatformGeometryShader.gs"
    );

    this->skybox_shader_manager = ShaderManager();
    this->skybox_shader_manager.BuildShaders(
        (char*)"shaders/SkyboxVertexShader.vs",
        (char*)"shaders/SkyboxFragmentShader.fs"
    );

    this->BindObjects();

    this->platform_shader_manager.Use();
}

void GLCore::BindObjects()
{    
    terrain.GenerateAndBindTerrain(this->platform_VAO, this->platform_VBO, this->platform_EBO);

    Skybox skybox;
    skybox.BindSkybox(this->skybox_VAO, this->skybox_VBO);
}

void GLCore::DrawPlatforms()
{
    glm::mat4 projection = glm::perspective(glm::radians(this->cameraManager.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = this->cameraManager.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);

    this->platform_shader_manager.Use();
    this->platform_shader_manager.SetMat4((char*)"projection", projection);
    this->platform_shader_manager.SetMat4((char*)"view", view);
    this->platform_shader_manager.SetFloat((char*)"time", static_cast<float>(glfwGetTime()));

    glBindVertexArray(platform_VAO);

    model = glm::mat4(1.0f);
    platform_shader_manager.SetMat4((char*)"model", model);

    glDrawElements(GL_TRIANGLES, 6*(128-1)*(128-1), GL_UNSIGNED_INT, 0); //TODO optimize

    glBindVertexArray(0);
}

void GLCore::DrawSkybox()
{
    glm::mat4 projection = glm::perspective(glm::radians(this->cameraManager.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = this->cameraManager.GetViewMatrixForSkybox();

    glDepthFunc(GL_LEQUAL);
    this->skybox_shader_manager.Use();
    this->skybox_shader_manager.SetMat4((char*)"view", view);
    this->skybox_shader_manager.SetMat4((char*)"projection", projection);

    glBindVertexArray(skybox_VAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void GLCore::RenderLoop()
{
    int prev_sec = 0;
    int counter = 0;

    ProcessInput(true);

    while (!glfwWindowShouldClose(this->window)) {
        float current_frame = static_cast<float>(glfwGetTime());
        this->delta_time = current_frame - this->last_frame;
        this->last_frame = current_frame;

        if (current_frame > prev_sec) {
            prev_sec += 1;
            std::cout << "FPS: " << counter << std::endl;
            counter = 0;
        }
        
        counter++;

        this->ProcessInput();

        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->DrawPlatforms();
        this->DrawSkybox();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &this->platform_VAO);
    glDeleteBuffers(1, &this->platform_VBO);

    this->platform_shader_manager.DeleteShader();

    glfwTerminate();
}

void GLCore::ProcessInput(bool start)
{
    if (start) {
        this->cameraManager.ProcessKeyboard(CAM_FORWARD, delta_time, this->terrain);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        this->cameraManager.ProcessKeyboard(CAM_FORWARD, delta_time, this->terrain);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        this->cameraManager.ProcessKeyboard(CAM_BACKWARD, delta_time, this->terrain);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        this->cameraManager.ProcessKeyboard(CAM_LEFT, delta_time, this->terrain);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        this->cameraManager.ProcessKeyboard(CAM_RIGHT, delta_time, this->terrain);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        this->cameraManager.ProcessKeyboard(CAM_DOWN, delta_time, this->terrain);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        this->cameraManager.ProcessKeyboard(CAM_UP, delta_time, this->terrain);
    }

    this->cameraManager.UpdateCameraVectors();
}

void GLCore::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLCore::MouseCallback(GLFWwindow* window, double xpos_in, double ypos_in)
{
    float xpos = static_cast<float>(xpos_in);
    float ypos = static_cast<float>(ypos_in);

    if (GLCore::first_mouse)
    {
        GLCore::last_x = xpos;
        GLCore::last_y = ypos;
        GLCore::first_mouse = false;
    }

    float xoffset = xpos - GLCore::last_x;
    float yoffset = GLCore::last_y - ypos;

    GLCore::last_x = xpos;
    GLCore::last_y = ypos;

    cameraManager.ProcessMouseMovement(xoffset, yoffset);
}