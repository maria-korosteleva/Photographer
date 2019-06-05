#pragma once
// Class is capable of loading the mesh from the file (.obj, .ply?) and render several views of it 
// from the pre-defined set of the cameras
// Should know the locations of the shader files
// Should know the location of the input of vertex shader
// TODO get out the locations

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
// math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// From other Project 
#include <GK-Undressing-People-Ceres/GeneralMesh.h>

// Local
#include "Shader.h"
#include "Camera.h"


// #define __APPLE__    // uncomment this statement to fix compilation on Mac OS X

class Photographer
{
public:
    // TUTORIAL keep track of the mouse
    static float yaw_, pitch_;
    static float lastX_, lastY_;
    static bool first_mouse_;
    static float fov_;

    Photographer();
    ~Photographer();

    int run();

    void setObject(GeneralMesh* object);

private:
    static constexpr const char* const vertex_shader_path_ = "./Shaders/VertexShader.glsl";
    static constexpr const char const * fragment_shader_path_ = "./Shaders/FragmentShader.glsl";

    // Scene preparation
    void createObjectVAO_();
    void createShaders_();
    void setUpObjectColor_();
    void setUpLight_();

    // called every frame
    void drawMainObjects_();

    // context set-up
    GLFWwindow* initWindowContext_();
    void registerCallbacks_(GLFWwindow* window);
    void cleanAndCloseContext_();
    
    // TUTORIAL
    // View Control
    void processInput_(GLFWwindow *window);
    // callbacks should be static!
    static void framebufferSizeCallback_(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // target
    GeneralMesh* object_;
    
    // running ids
    unsigned int object_vertex_array_ = 0;
    unsigned int object_vertex_buffer_ = 0;
    unsigned int object_element_buffer_ = 0;
    
    // tools
    Shader* shader_ = nullptr;
    static Camera* camera_;

    // appearence control
    float win_width_ = 800;
    float win_height_ = 600;

    // lighting
    glm::vec3 dir_light_ = glm::vec3(-0.2f, -1.0f, -0.3f);
    static const std::size_t kPointLights = 4;
    glm::vec3 point_light_positions_[kPointLights] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    
    // TUTORIAL keep track of rendering speed for camera speed adjustment
    float delta_time_ = 0.0f;	// Time between current frame and last frame
    float last_frame_time_ = 0.0f; // Time of last frame

    // data from tutorial
    static const std::size_t num_cubes_ = 10;
    glm::vec3 cube_positions_[num_cubes_] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

};
