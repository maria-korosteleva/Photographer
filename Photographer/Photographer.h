#pragma once
// Class is capable of loading the mesh from the file (.obj, .ply?) and render several views of it 
// from the pre-defined set of the cameras
// Should know the locations of the shader files
// Should know the location of the input of vertex shader

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include "Shader.h"


// #define __APPLE__    // uncomment this statement to fix compilation on Mac OS X

class Photographer
{
public:
    Photographer();
    ~Photographer();

    int run();

private:
    static constexpr const char* const vertex_shader_path_ = "C:/Users/Maria/MyDocs/opengl_edu/Photographer/Photographer/Shaders/VertexShader.glsl";
    static constexpr const char const * fragment_shader_path_ = "C:/Users/Maria/MyDocs/opengl_edu/Photographer/Photographer/Shaders/FragmentShader.glsl";
    // functions
    void CreateObjectVAO();
    unsigned int LoadTexture(const char * filename, bool alpha = false);

    GLFWwindow* InitWindowContext();
    void RegisterCallbacks(GLFWwindow* window);
    void CleanAndCloseContext();
    
    // func from tutorial
    void ProcessInput(GLFWwindow *window);
    // callbacks should be static!
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    // vars
    unsigned int object_vertex_array_ = 0;
    unsigned int object_vertex_buffer_ = 0;
    unsigned int object_element_buffer_ = 0;
    unsigned int tex_container_ = 0;
    unsigned int tex_face_ = 0;
    Shader* shader_program_ = nullptr;
    float mix_rate_ = 0.2;
    
    // data from tutorial
    // triangle in Normalized Device Coordinates
    static constexpr const char* const tex_container_path_ = "C:/Users/Maria/MyDocs/opengl_edu/Photographer/Photographer/Textures/container.jpg";
    static constexpr const char* const tex_smiley_path_ = "C:/Users/Maria/MyDocs/opengl_edu/Photographer/Photographer/Textures/awesomeface.png";

    static const std::size_t  kTriangleArrSize = 9;
    float triangle_verts_[kTriangleArrSize] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // rectangle in Normalized Device Coordinates
    static const std::size_t  kRectangleArrSize = (3 + 3 + 2)*4;
    float rectangle_verts_[kRectangleArrSize] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    static const std::size_t  kRectangleFacesArrSize = 3 * 2;
    unsigned int rectangle_faces_[kRectangleFacesArrSize] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

};

