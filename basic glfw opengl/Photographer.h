#pragma once
/* 
Class is capable of loading the mesh from the file (.obj, .ply?) and render several views of it 
from the pre-defined set of the cameras

Requirements: 
OpenGL 3.3 or higher. Only uses the core functionality so far

Dependencies:
glad (https://glad.dav1d.de/): please use the version that suits your settings
glfw (https://github.com/glfw/glfw)
*/

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

// Macro for a more convenient shader specification
#define PHOTOGRAPHER_GLSL_TO_STRING(version, shader)  "#version " #version "\n" #shader

class Photographer
{
public:
    Photographer();
    ~Photographer();

    int run();

private:
    // useful vars
    unsigned int vertex_buffer_object_;
    unsigned int shader_program_id_;
    // useful functions
    void BindVBO();
    unsigned int CompileVertexShader();
    unsigned int CompileFragmentShader();
    void LinkShaders();

    // data from tutorial
    // triangle in Normalized Device Coordinates
    static const std::size_t  kTriangleArrSize = 9;
    float triangle_verts_[kTriangleArrSize] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    // func from tutorial (working with input)
    void ProcessInput(GLFWwindow *window);
    // callbacks should be static!
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};

