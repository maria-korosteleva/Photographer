#include "Photographer.h"



Photographer::Photographer()
{
}


Photographer::~Photographer()
{
}

int Photographer::run()
{
    glfwInit();
    // Configure GLFW
    // TODO change to appropriate version afterwards
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MAC

    // create a window
    int win_width = 800;
    int win_height = 600;
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set viewport size -- viewport is needed to calc screen coordinates from normalized range
    glViewport(0, 0, win_width, win_height);

    // TUTORIAL: set viewport size change callback
    glfwSetFramebufferSizeCallback(window, Photographer::FramebufferSizeCallback);

    // TUTORIAL: Forever loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        this->ProcessInput(window);

        // render commands
        // clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // state-setting function
        glClear(GL_COLOR_BUFFER_BIT);       // state-using function

        // swap the buffers and check all call events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void Photographer::BindVBO()
{
    glGenBuffers(1, &this->vertex_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertex_buffer_object_);

    // last parameter indicated that data won't change througout the process
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->triangle_verts_), this->triangle_verts_, GL_STATIC_DRAW);
}

unsigned int Photographer::CompileVertexShader()
{
    // vertex shader code as string :D
    const char *vertex_shader_source = PHOTOGRAPHER_GLSL_TO_STRING(330, core
        layout (location = 0) in vec3 aPos;
        void main()\n
        {
           gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n
        }
    );

    unsigned int vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader_id, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader_id);

    // check if compiled ok
    int  success;
    char info_log[512];
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader_id, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    return vertex_shader_id;
}

unsigned int Photographer::CompileFragmentShader()
{
    // shader code as string :D
    const char *fragment_shader_source = PHOTOGRAPHER_GLSL_TO_STRING(330, core
        out vec4 FragColor;
    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
    );

    unsigned int fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader_id, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader_id);

    // check if compiled ok
    int  success;
    char info_log[512];
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment_shader_id, 512, NULL, info_log);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
    }

    return fragment_shader_id;
}

void Photographer::LinkShaders()
{
    // get shaders
    unsigned int vertex_shader_id = this->CompileVertexShader();
    unsigned int fragment_shader_id = this->CompileFragmentShader();

    // link
    this->shader_program_id_ = glCreateProgram();
    glAttachShader(this->shader_program_id_, vertex_shader_id);
    glAttachShader(this->shader_program_id_, fragment_shader_id);
    glLinkProgram(this->shader_program_id_);

    // check if linked ok
    int  success;
    char info_log[512];
    glGetShaderiv(this->shader_program_id_, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(this->shader_program_id_, 512, NULL, info_log);
        std::cout << "ERROR::SHADER PROGRAM::LINKING FAILED\n" << info_log << std::endl;
    }
    
    // cleaning
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
}

void Photographer::ProcessInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Photographer::FramebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}
