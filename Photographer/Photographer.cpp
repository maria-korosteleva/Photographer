//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

#include "Photographer.h"

Photographer::Photographer()
{
}

Photographer::~Photographer()
{
}

int Photographer::run()
{
    GLFWwindow* window = InitWindowContext();
    RegisterCallbacks(window);
    
    CreateObjectVAO();

    tex_container_ = LoadTexture(tex_container_path_);
    tex_face_ = LoadTexture(tex_smiley_path_, true);

    if (shader_program_ != nullptr) delete shader_program_;
    shader_program_ = new Shader(vertex_shader_path_, fragment_shader_path_);
    shader_program_->use();
    shader_program_->SetUniform("texture2", 1);   // bind the second texture location manually

    // going 3D
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), win_width_ / win_height_, 0.1f, 100.0f);

    // camera
    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    
    glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_reverse_direction = glm::normalize(camera_pos - camera_target); // reverse direction
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // up guess -- results in relatively up camera
    glm::vec3 camera_right = glm::normalize(glm::cross(up, camera_reverse_direction));
    //glm::vec3 camera_up = glm::normalize(glm::cross(camera_reverse_direction, camera_right));

    glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up = up;

    //glm::mat4 view;
    //view = glm::lookAt(camera_pos, camera_target, up);

    glBindVertexArray(this->object_vertex_array_);

    while (!glfwWindowShouldClose(window))
    {
        // TUTORIAL input
        this->ProcessInput(window);

        // render commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);   // state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // state-using function
        
        // TUTORIAL bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_container_);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_face_);
        shader_program_->SetUniform("mix_value", texture_mix_rate_);

        // projection
        shader_program_->SetUniform("projection", projection);

        float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        camera_pos = glm::vec3(camX, 0.0, camZ);
        glm::mat4 view;
        view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
        shader_program_->SetUniform("view", view);

        // draw cubes
        for (int i = 0; i < num_cubes_; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions_[i]);
            if (i % 3 == 0)
            {
                model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            }
            else
            {
                float angle = 20.0f * i;
                model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            }
            shader_program_->SetUniform("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //glDrawElements(GL_TRIANGLES, this->kRectangleFacesArrSize, GL_UNSIGNED_INT, 0); // second argument is the tot number of vertices to draw
        
        // swap the buffers and check all call events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    CleanAndCloseContext();
    return 0;
}

void Photographer::CreateObjectVAO()
{
    // id avalibility check
    if (object_vertex_array_ > 0
        || object_vertex_buffer_ > 0
        || object_element_buffer_ > 0)
    {
        // catastrofY! Want to overwrite the existing buffers!
        std::cout << "ERROR::CREATE OBJECT BUFFERS::OBJECT BUFFERS WERE ALREADY ALLOCATED. DATA IS LOST\n" << std::endl;
    }

    // VAO allows storing the vertex attribures as an object for easy loading after
    glGenVertexArrays(1, &object_vertex_array_);
    glBindVertexArray(object_vertex_array_);

    // setup vertex buffer object
    glGenBuffers(1, &object_vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, object_vertex_buffer_);
    // last parameter indicated that data won't change througout the process
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts_), cube_verts_, GL_STATIC_DRAW);

    // setup element buffer object -- for faces!
    //glGenBuffers(1, &object_element_buffer_);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_element_buffer_);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->rectangle_faces_), rectangle_faces_, GL_STATIC_DRAW);

    // Vertex data interpretation guide
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Cleaning. Unbinding is not necessary, but I'm doing this for completeness
    // Important to unbind GL_ELEMENT_ARRAY_BUFFER after Vertex Array
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int Photographer::LoadTexture(const char * filename, bool alpha)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load
    int width, height, n_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char * tex_data = stbi_load(filename, &width, &height, &n_channels, 0);
    if (!tex_data)
    {
        std::cout << "Failed to load texture" << std::endl;
        return 0;
    }

    // generate texture & mipmaps
    if (alpha)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);

    // it's ok to free the tex_container_data_ here.
    stbi_image_free(tex_data);

    return texture;
}

GLFWwindow* Photographer::InitWindowContext()
{
    glfwInit();

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // create a window
    GLFWwindow* window = glfwCreateWindow(win_width_, win_height_, "Photographer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

    // load glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 0;
    }

    // set viewport size -- viewport is needed to calc screen coordinates from normalized range
    glViewport(0, 0, win_width_, win_height_);

    glEnable(GL_DEPTH_TEST);

    return window;
}

void Photographer::RegisterCallbacks(GLFWwindow * window)
{
    glfwSetFramebufferSizeCallback(window, Photographer::FramebufferSizeCallback);
}

void Photographer::CleanAndCloseContext()
{
    glDeleteVertexArrays(1, &object_vertex_array_);
    glDeleteBuffers(1, &object_vertex_buffer_);
    glDeleteBuffers(1, &object_element_buffer_);
    glDeleteTextures(1, &tex_container_);
    glDeleteTextures(1, &tex_face_);
    delete shader_program_;
    glfwTerminate();

    shader_program_ = nullptr;
    object_vertex_array_ = object_element_buffer_ = object_vertex_buffer_ = tex_container_ = tex_face_ = 0;
}

void Photographer::ProcessInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        texture_mix_rate_ += 0.02;
        if (texture_mix_rate_ > 1.0f) texture_mix_rate_ = 1.0;
        shader_program_->SetUniform("mix_rate", texture_mix_rate_);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        texture_mix_rate_ -= 0.02;
        if (texture_mix_rate_ < 0.0f) texture_mix_rate_ = 0.0;
        shader_program_->SetUniform("mix_rate", texture_mix_rate_);
    }
}

void Photographer::FramebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}
