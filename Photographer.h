#pragma once
// Class is capable of taking pictures of the 3D object from multiple cameras =)
// Accepts GeneralMesh object as an input. 
// from the pre-defined set of the cameras
// 
// Note: the set-up of the Buffer objects is tightly coupled with the variable location settings in the shaders loaded by the Shader class

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
// math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// From other Project 
#include <GeneralMesh/GeneralMesh.h>

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
    void saveImageCamerasParamsCV(const std::string path = "./");

    void setObject(GeneralMesh* object);
    // allows to specify either absolute position
    // or direction + distance to target (if dist is set)
    void addCameraToPosition(float x, float y, float z, float dist = -1.0);

private:
    static constexpr const char* const vertex_shader_path_ = "./Shaders/VertexShader.glsl";
    static constexpr const char const * fragment_shader_path_ = "./Shaders/FragmentShader.glsl";

    // Scene preparation
    void setUpScene_();
    void createTargetObjectVAO_();
    void createCameraObjectVAO_();
    void createShaders_();
    void setUpTargetObjectColor_();
    void setUpLight_();
    Camera createDefaultTargetCamera_();

    // called every frame
    void clearBackground_();
    void cameraParamsToShader_(Shader& shader, Camera& camera);
    void drawMainObject_(Shader& shader);
    void drawImageCameraObjects_(Shader& shader);

    // context set-up
    GLFWwindow* initWindowContext_(bool visible);
    void initCustomBuffer_();
    void registerCallbacks_(GLFWwindow* window);
    void cleanAndCloseContext_();

    // saver!
    void saveRGBTexToFile_(const std::string filename, unsigned int texture_id);
    
    // View Control
    void processInput_(GLFWwindow *window);
    // callbacks should be static!
    static void framebufferSizeCallback_(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    // Vars ------------------------------
    // tools
    // pointers are used to init shader later than in constructor
    Shader* shader_ = nullptr;
    Shader* simple_shader_ = nullptr;
    glm::vec3 default_camera_position_ = glm::vec3(0.0f, 0.0f, 4.0f);
    std::vector<Camera> image_cameras_;
    static Camera* view_camera_;

    // appearence control
    float win_width_ = 800;
    float win_height_ = 600;

    // target
    GeneralMesh* object_;
    glm::vec3 default_camera_target_;
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
    
    // keep track of rendering speed for camera speed adjustment
    float delta_time_ = 0.0f;	// Time between current frame and last frame
    float last_frame_time_ = 0.0f; // Time of last frame

    // Camera Model data
    unsigned int cam_obj_vertex_array_ = 0;
    unsigned int cam_obj_vertex_buffer_ = 0;
    unsigned int cam_obj_element_buffer_ = 0;

    static constexpr size_t camera_model_verts_num_ = 67;
    float camera_model_vertices_[camera_model_verts_num_ * 3] = {
        0.0652003f, 0.0366214f, -0.0759451f,
        -0.155618f, -0.130083f, -0.0759451f,
        -0.155618f, 0.0366214f, -0.0759451f,
        0.0652003f, 0.0366214f, -0.0759451f,
        0.0652003f, -0.130083f, -0.0759451f,
        -0.155618f, -0.130083f, -0.0759451f,
        -0.155618f, 0.0366214f, 0.0798159f,
        0.0652003f, -0.130083f, 0.0798159f,
        0.0652003f, 0.0366214f, 0.0798159f,
        -0.155618f, -0.130083f, 0.0798159f,
        0.0652003f, 0.0366214f, 0.0798159f,
        0.0652003f, -0.130083f, -0.0759451f,
        0.0652003f, 0.0366214f, -0.0759451f,
        0.0652003f, -0.130083f, 0.0798159f,
        -0.155618f, 0.0366214f, -0.0759451f,
        -0.155618f, -0.130083f, 0.0798159f,
        -0.155618f, 0.0366214f, 0.0798159f,
        -0.155618f, -0.130083f, -0.0759451f,
        -0.155618f, 0.0366214f, -0.0759451f,
        0.0652003f, 0.0366214f, 0.0798159f,
        0.0652003f, 0.0366214f, -0.0759451f,
        -0.155618f, 0.0366214f, 0.0798159f,
        -0.155618f, -0.130083f, 0.0798159f,
        0.0652003f, -0.130083f, -0.0759451f,
        0.0652003f, -0.130083f, 0.0798159f,
        -0.155618f, -0.130083f, -0.0759451f,
        0.17822f, -0.0526426f, 0.00506594f,
        0.17822f, -0.128491f, -0.0707821f,
        0.17822f, 0.0232054f, -0.0707821f,
        0.17822f, -0.128491f, -0.0707821f,
        -0.0363047f, -0.0526426f, 0.00506594f,
        0.17822f, 0.0232054f, -0.0707821f,
        0.17822f, -0.128491f, 0.0809139f,
        0.17822f, -0.128491f, 0.0809139f,
        -0.0363047f, -0.0526426f, 0.00506594f,
        0.17822f, -0.128491f, -0.0707821f,
        0.17822f, 0.0232054f, 0.0809139f,
        0.17822f, 0.0232054f, 0.0809139f,
        -0.0363047f, -0.0526426f, 0.00506594f,
        0.17822f, -0.128491f, 0.0809139f,
        0.17822f, 0.0232054f, -0.0707821f,
        -0.0363047f, -0.0526426f, 0.00506594f,
        0.17822f, 0.0232054f, 0.0809139f,
        -0.0201357f, 0.139205f, -0.0205681f,
        -0.0628827f, 0.0366214f, -0.0205681f,
        -0.0628827f, 0.139205f, -0.0205681f,
        -0.0201357f, 0.0366214f, -0.0205681f,
        -0.0628827f, 0.139205f, 0.0221779f,
        -0.0201357f, 0.0366214f, 0.0221779f,
        -0.0201357f, 0.139205f, 0.0221779f,
        -0.0628827f, 0.0366214f, 0.0221779f,
        -0.0201357f, 0.139205f, 0.0221779f,
        -0.0201357f, 0.0366214f, -0.0205681f,
        -0.0201357f, 0.139205f, -0.0205681f,
        -0.0201357f, 0.0366214f, 0.0221779f,
        -0.0628827f, 0.139205f, -0.0205681f,
        -0.0628827f, 0.0366214f, 0.0221779f,
        -0.0628827f, 0.139205f, 0.0221779f,
        -0.0628827f, 0.0366214f, -0.0205681f,
        -0.0628827f, 0.139205f, -0.0205681f,
        -0.0201357f, 0.139205f, 0.0221779f,
        -0.0201357f, 0.139205f, -0.0205681f,
        -0.0628827f, 0.139205f, 0.0221779f,
        -0.0628827f, 0.0366214f, 0.0221779f,
        -0.0201357f, 0.0366214f, -0.0205681f,
        -0.0201357f, 0.0366214f, 0.0221779f,
        -0.0628827f, 0.0366214f, -0.0205681f,
    };

    static constexpr size_t camera_model_faces_num_ = 32;
    unsigned int camera_model_faces_[camera_model_faces_num_ * 3] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        6, 9, 7,
        10, 11, 12,
        10, 13, 11,
        14, 15, 16,
        14, 17, 15,
        18, 19, 20,
        18, 21, 19,
        22, 23, 24,
        22, 25, 23,
        26, 27, 28,
        29, 30, 31,
        26, 32, 27,
        33, 34, 35,
        26, 36, 32,
        37, 38, 39,
        26, 28, 36,
        40, 41, 42,
        43, 44, 45,
        43, 46, 44,
        47, 48, 49,
        47, 50, 48,
        51, 52, 53,
        51, 54, 52,
        55, 56, 57,
        55, 58, 56,
        59, 60, 61,
        59, 62, 60,
        63, 64, 65,
        63, 66, 64,
    };
};
