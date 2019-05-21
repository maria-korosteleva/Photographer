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

    // Link shaders: here to allocate resources only for the drawing loop
    unsigned int shader_program_id = Photographer::LinkShaders();

    // create VAOs
    // TODO: move to constructor? or object setter? In this case, add checks that the pbject is created
    // TODO: Share the location ids somehow!!
    this->CreateTriangleVAO();

    // TUTORIAL: Forever loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        this->ProcessInput(window);

        // render commands
        // clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // state-setting function
        glClear(GL_COLOR_BUFFER_BIT);       // state-using function

        // draw
        glUseProgram(shader_program_id);
        //glUseProgram(shaderProgram);
        glBindVertexArray(this->triangle_VAO_); // No need to do this every time
        glDrawArrays(GL_TRIANGLES, 0, 3);   // last is the number of vertices

        // swap the buffers and check all call events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // TODO!  optional: de-allocate all resources once they've outlived their purpose:
    // To destructor? Store the VBO than!
    // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program_id);

    glfwTerminate();
    return 0;
}

void Photographer::CreateTriangleVAO()
{
    // VAO allows storing the vertex attribures as an object for easy loading after
    glGenVertexArrays(1, &this->triangle_VAO_);
    glBindVertexArray(this->triangle_VAO_);

    // setup vertex buffer object
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // last parameter indicated that data won't change througout the process
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->triangle_verts_), this->triangle_verts_, GL_STATIC_DRAW);

    // Vertex data interpretation guide
    // location, size of the attribute, attribute type, data normalization bool, data stride, data offset
    // takes data from the current binded VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Cleaning. Unbinding is not necessary, but I'm doing this for completeness
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

unsigned int Photographer::CompileVertexShader()
{
    // vertex shader code as string :D
    const char *vertex_shader_source = PHOTOGRAPHER_GLSL_TO_STRING(330, 
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
    const char *fragment_shader_source = PHOTOGRAPHER_GLSL_TO_STRING(460,
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

unsigned int Photographer::LinkShaders()
{
    // get shaders
    unsigned int vertex_shader_id = Photographer::CompileVertexShader();
    unsigned int fragment_shader_id = Photographer::CompileFragmentShader();

    // link shaders
    unsigned int shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vertex_shader_id);
    glAttachShader(shader_program_id, fragment_shader_id);
    glLinkProgram(shader_program_id);

    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return shader_program_id;
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
