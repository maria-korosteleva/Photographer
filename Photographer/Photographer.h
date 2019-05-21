#pragma once
// Class is capable of loading the mesh from the file (.obj, .ply?) and render several views of it 
// from the pre-defined set of the cameras


#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

// Macro for a more convenient shader specification
#define PHOTOGRAPHER_GLSL_TO_STRING(version, shader)  "#version " #version " core \n" #shader  

class Photographer
{
public:
    Photographer();
    ~Photographer();

    int run();

private:
    // useful vars
    unsigned int object_vertex_array_ = 0;
    unsigned int object_vertex_buffer_ = 0;
    unsigned int object_element_buffer_ = 0;
    // useful functions
    void CreateObjectVAO();
    
    // static -> Shaders are created dynamically in run() function only to allocate GPU resources only for the run
    // TODO rethink this behavior
    static unsigned int CompileVertexShader();
    static unsigned int CompileFragmentShader();
    static unsigned int LinkShaders();

    // data from tutorial
    // triangle in Normalized Device Coordinates
    static const std::size_t  kTriangleArrSize = 9;
    float triangle_verts_[kTriangleArrSize] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    // rectangle in Normalized Device Coordinates
    static const std::size_t  kRectangleArrSize = 3*4;
    float rectangle_verts_[kRectangleArrSize] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    static const std::size_t  kRectangleFacesArrSize = 3 * 2;
    unsigned int rectangle_faces_[kRectangleFacesArrSize] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // func from tutorial (working with input)
    void ProcessInput(GLFWwindow *window);
    // callbacks should be static!
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};

