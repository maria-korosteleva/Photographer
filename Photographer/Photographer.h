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
#include <stb/stb_image_write.h>
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
    Photographer(GeneralMesh* target_object);
    ~Photographer();

    void viewScene();
    void renderToImages(const std::string path = "./");

    void setObject(GeneralMesh* object);

private:
    static constexpr const char* const vertex_shader_path_ = "./Shaders/VertexShader.glsl";
    static constexpr const char const * fragment_shader_path_ = "./Shaders/FragmentShader.glsl";

    // Scene preparation
    void setUpScene_();
    void createObjectVAO_();
    void createShaders_();
    void setUpObjectColor_();
    void setUpLight_();

    // called every frame
    void clearBackground_();
    void cameraParamsToShader_(Shader& shader, Camera& camera);
    void drawMainObjects_(Shader& shader);

    // context set-up
    GLFWwindow* initWindowContext_(bool visible);
    void initCustomBuffer_();
    void registerCallbacks_(GLFWwindow* window);
    void cleanAndCloseContext_();

    // saver!
    void saveRGBTexToFile(const std::string filename, unsigned int texture_id);
    
    // View Control
    void processInput_(GLFWwindow *window);
    // callbacks should be static!
    static void framebufferSizeCallback_(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    // Vars ------------------------------
     
    // tools
    Shader* shader_ = nullptr;
    glm::vec3 default_camera_position_ = glm::vec3(0.0f, 0.0f, 4.0f);
    std::vector<Camera> image_cameras;
    static Camera* view_camera_;

    // appearence control
    float win_width_ = 800;
    float win_height_ = 600;

    // target
    GeneralMesh* object_;
    unsigned int object_vertex_array_ = 0;
    unsigned int object_vertex_buffer_ = 0;
    unsigned int object_element_buffer_ = 0;

    // custom buffers
    unsigned int framebuffer_ = 0;
    unsigned int texture_color_buffer_ = 0;
    unsigned int depth_render_buffer_ = 0;

    // keep track of the mouse
    static float yaw_, pitch_;
    static float lastX_, lastY_;
    static bool first_mouse_;
    static float fov_;
    
    // keep track of rendering speed for camera speed adjustment
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
