#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shaders/VertexShader.h"
#include "Shaders/FragmentShader.h"

// Macro for a more convenient shader specification
#define SHADER_CODE_GLSL_TO_STRING(version, shader)  "#version " #version " core \n" #shader  

class Shader
{
public:
    // Constructor builds the shader
    enum ShaderTypes
    {
        FULL_SHADER, 
        SIMPLE_SHADER
    };
    Shader(ShaderTypes vertex_shader_type, ShaderTypes fragment_shader_type);
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    ~Shader();
    // Activate the shader
    void use();
    // working with uniforms
    void setUniform(const std::string &name, bool value) const;
    void setUniform(const std::string &name, int value) const;
    void setUniform(const std::string &name, float value) const;
    void setUniform(const std::string &name, glm::mat4 value) const;
    void setUniform(const std::string &name, glm::vec3 value) const;
    void setUniform(const std::string &name, glm::vec4 value) const;
    // Program ID
    unsigned int getID() { return this->ID_; }
  
private:
    void createProgram_(unsigned int vertex_shader, unsigned int fragment_shader);

    static std::string readCodeFile_(const GLchar* path);

    // compilers
    static unsigned int compileVertexShader_(const char* shader_code);
    static unsigned int compileFragmentShader_(const char* shader_code);
    
    unsigned int ID_;

    // default shaders
    const char *default_vertex_shader_source_ = SHADER_CODE_GLSL_TO_STRING(330,

        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            ourColor = aColor;
        }
    );

    const char *default_fragment_shader_source_ = SHADER_CODE_GLSL_TO_STRING(460,
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0, 0.9, 0.9, 1.0);
        }
    );
};

