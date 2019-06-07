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
    Shader* shader_ = nullptr;
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

    static constexpr size_t camera_model_verts_num_ = 43;
    float camera_model_vertices_[camera_model_verts_num_ * 3] = {
        0.0420324, 0.0856893, -0.0754958,
        -0.178786, -0.0810147, -0.0754958,
        -0.178786, 0.0856893, -0.0754958,
        0.0420324, 0.0856893, -0.0754958,
        0.0420324, -0.0810147, -0.0754958,
        -0.178786, -0.0810147, -0.0754958,
        -0.178786, 0.0856893, 0.0802652,
        0.0420324, -0.0810147, 0.0802652,
        0.0420324, 0.0856893, 0.0802652,
        -0.178786, -0.0810147, 0.0802652,
        0.0420324, 0.0856893, 0.0802652,
        0.0420324, -0.0810147, -0.0754958,
        0.0420324, 0.0856893, -0.0754958,
        0.0420324, -0.0810147, 0.0802652,
        -0.178786, 0.0856893, -0.0754958,
        -0.178786, -0.0810147, 0.0802652,
        -0.178786, 0.0856893, 0.0802652,
        -0.178786, -0.0810147, -0.0754958,
        -0.178786, 0.0856893, -0.0754958,
        0.0420324, 0.0856893, 0.0802652,
        0.0420324, 0.0856893, -0.0754958,
        -0.178786, 0.0856893, 0.0802652,
        -0.178786, -0.0810147, 0.0802652,
        0.0420324, -0.0810147, -0.0754958,
        0.0420324, -0.0810147, 0.0802652,
        -0.178786, -0.0810147, -0.0754958,
        0.155052, -0.0035747, 0.00551521,
        0.155052, -0.0794227, -0.0703328,
        0.155052, 0.0722733, -0.0703328,
        0.155052, -0.0794227, -0.0703328,
        -0.0594726, -0.0035747, 0.00551521,
        0.155052, 0.0722733, -0.0703328,
        0.155052, -0.0794227, 0.0813632,
        0.155052, -0.0794227, 0.0813632,
        -0.0594726, -0.0035747, 0.00551521,
        0.155052, -0.0794227, -0.0703328,
        0.155052, 0.0722733, 0.0813632,
        0.155052, 0.0722733, 0.0813632,
        -0.0594726, -0.0035747, 0.00551521,
        0.155052, -0.0794227, 0.0813632,
        0.155052, 0.0722733, -0.0703328,
        -0.0594726, -0.0035747, 0.00551521,
        0.155052, 0.0722733, 0.0813632
    };

    static constexpr size_t camera_model_faces_num_ = 20;
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
    };
};
