#include "Shader.h"


Shader::Shader(const GLchar * vertex_path, const GLchar * fragment_path)
{
    unsigned int vertex_shader;
    unsigned int fragment_shader;
    // read & compile
    if (vertex_path != nullptr)
    {
        std::string vertex_code = Shader::ReadCodeFile(vertex_path);
        vertex_shader = Shader::CompileVertexShader(vertex_code.c_str());
    }
    else
    {
        vertex_shader = Shader::CompileVertexShader(default_vertex_shader_source_);
    }
    
    if (fragment_path != nullptr)
    {
        std::string fragment_code = Shader::ReadCodeFile(fragment_path);
        fragment_shader = Shader::CompileFragmentShader(fragment_code.c_str());
    }
    else
    {
        fragment_shader = Shader::CompileFragmentShader(default_fragment_shader_source_);
    }
    
    // create program
    CreateProgram(vertex_shader, fragment_shader);

    // cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader()
{
    glDeleteProgram(ID_);
}

void Shader::use()
{
    glUseProgram(ID_);
}

void Shader::SetUniform(const std::string & name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID_, name.c_str()), (int)value);
}

void Shader::SetUniform(const std::string & name, int value) const
{
    glUniform1i(glGetUniformLocation(ID_, name.c_str()), value);
}

void Shader::SetUniform(const std::string & name, float value) const
{
    glUniform1f(glGetUniformLocation(ID_, name.c_str()), value);
}

void Shader::SetUniform(const std::string & name, glm::mat4 value) const
{
    unsigned int location = glGetUniformLocation(ID_, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniform(const std::string & name, glm::vec3 value) const
{
    unsigned int location = glGetUniformLocation(ID_, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::CreateProgram(unsigned int vertex_shader, unsigned int fragment_shader)
{
    // link shaders
    ID_ = glCreateProgram();
    glAttachShader(ID_, vertex_shader);
    glAttachShader(ID_, fragment_shader);
    glLinkProgram(ID_);

    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(ID_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID_, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

std::string Shader::ReadCodeFile(const GLchar * path)
{
    std::string code;
    std::ifstream file;

    // allow throuwing exeptions
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        code = stream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE NOT SUCCESFULLY READ" << std::endl;
    }
    return code;
}

unsigned int Shader::CompileVertexShader(const char * shader_code)
{
    unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader_id, 1, &shader_code, NULL);
    glCompileShader(vertex_shader_id);

    // check if compiled ok
    int  success;
    char info_log[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        // info before dying young 
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
        return 0;
    }

    return vertex_shader_id;
}

unsigned int Shader::CompileFragmentShader(const char * shader_code)
{
    unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader_id, 1, &shader_code, NULL);
    glCompileShader(fragment_shader_id);

    // check if compiled ok
    int  success;
    char info_log[512];
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        // info before dying young 
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
        return 0;
    }

    return fragment_shader_id;
}
