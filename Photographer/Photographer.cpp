//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

#include "Photographer.h"

float Photographer::lastX_ = 400;
float Photographer::lastY_ = 300;
bool Photographer::first_mouse_ = true;
Camera* Photographer::view_camera_ = nullptr;

Photographer::Photographer(GeneralMesh* target_object)
    : object_(target_object)
{
}

Photographer::~Photographer()
{
}

void Photographer::viewScene()
{
    GLFWwindow* window = initWindowContext_(true);
    registerCallbacks_(window);
    
    setUpScene_();

    view_camera_ = new Camera(win_width_, win_height_);
    view_camera_->setPosition(default_camera_position_);

    // operating the view
    Photographer::lastX_ = 400;
    Photographer::lastY_ = 300;
    first_mouse_ = true;
    last_frame_time_ = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        delta_time_ = currentFrame - last_frame_time_;
        last_frame_time_ = currentFrame;

        processInput_(window);
        
        clearBackground_();
        cameraParamsToShader_(*shader_, *view_camera_);
        drawMainObjects_(*shader_);

        // ----- finish
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanAndCloseContext_();
}

void Photographer::renderToImages(const std::string path)
{
    bool default_camera = false;
    if (image_cameras_.size() == 0)
    {
        std::cout << 
            "WARNING::RENDER TO FILE:: No Cameras Set; using default camera. Use addCameraToPosition() to set up cameras" 
            << std::endl;
        Camera camera(win_width_, win_height_);
        camera.setPosition(default_camera_position_);
        image_cameras_.push_back(camera);

        default_camera = true;
    }

    GLFWwindow* window = initWindowContext_(false);
    initCustomBuffer_();

    setUpScene_();

    for (auto &&camera: image_cameras_)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

        // render
        clearBackground_();
        cameraParamsToShader_(*shader_, camera);
        drawMainObjects_(*shader_);

        // Switch to default & save 
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        saveRGBTexToFile(path + "/view_" + std::to_string(camera.getID()) + ".png",
            texture_color_buffer_);
    }

    cleanAndCloseContext_();

    if (default_camera)
    {
        image_cameras_.pop_back();
    }
}

void Photographer::setObject(GeneralMesh * object)
{
    object_ = object;
}

void Photographer::addCameraToPosition(float x, float y, float z)
{
    Camera camera(win_width_, win_height_);
    camera.setPosition(glm::vec3(x, y, z));
    // set target mode

    image_cameras_.push_back(camera);
}

void Photographer::setUpScene_()
{
    createObjectVAO_();
    createShaders_();
    setUpObjectColor_();
    setUpLight_();
}

void Photographer::createObjectVAO_()
{
    // id avalibility check
    if (object_vertex_array_ > 0
        || object_vertex_buffer_ > 0
        || object_element_buffer_ > 0)
    {
        // catastrofY! Want to overwrite the existing buffers!
        std::cout << "ERROR::CREATE OBJECT BUFFERS::OBJECT BUFFERS WERE ALREADY ALLOCATED. DATA IS LOST\n" << std::endl;
    }

    // ------ Main (cube) object
    // VAO allows storing the vertex attribures as an object for easy loading after
    glGenVertexArrays(1, &object_vertex_array_);
    glBindVertexArray(object_vertex_array_);

    // setup vertex buffer object
    glGenBuffers(1, &object_vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, object_vertex_buffer_);
    // last parameter indicated that data won't change througout the process
    const std::vector<GeneralMesh::GLMVertex>* verts = &object_->getGLNormalizedVertices();

    glBufferData(GL_ARRAY_BUFFER, 
        object_->getGLNormalizedVertices().size() * sizeof(GeneralMesh::GLMVertex),
        &object_->getGLNormalizedVertices()[0], GL_STATIC_DRAW);

    // setup element buffer object -- for faces!
    glGenBuffers(1, &object_element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_element_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        object_->getGLMFaces().size() * sizeof(unsigned int), 
        &object_->getGLMFaces()[0], GL_STATIC_DRAW);

    // Vertex data interpretation guide
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GeneralMesh::GLMVertex), (void*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GeneralMesh::GLMVertex), 
        (void*)offsetof(GeneralMesh::GLMVertex, GeneralMesh::GLMVertex::normal));
    glEnableVertexAttribArray(1);

    // Cleaning. Unbinding is not necessary, but I'm doing this for completeness
    // Important to unbind GL_ELEMENT_ARRAY_BUFFER after Vertex Array
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Photographer::createShaders_()
{
    if (shader_ != nullptr) delete shader_;
    shader_ = new Shader(vertex_shader_path_, fragment_shader_path_);
}

void Photographer::setUpObjectColor_()
{
    shader_->use();

    //glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.31f);  coral
    glm::vec3 color = glm::vec3(0.6f, 0.6f, 0.6f);

    shader_->setUniform("material.diffuse", color);
    shader_->setUniform("material.specular", 0.3f * color);
    shader_->setUniform("material.shininess", 64.0f);
}

void Photographer::setUpLight_()
{
    // directional
    shader_->setUniform("directional_light.direction", glm::vec3(-0.2f, -1.0f, -0.5f));
    shader_->setUniform("directional_light.ambient", glm::vec3(0.2f));
    shader_->setUniform("directional_light.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
    shader_->setUniform("directional_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    static const std::size_t kPointLights = 2;
    glm::vec3 point_light_positions[kPointLights] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(0.0f,  0.0f, -2.0f)
    };

    // point lights
    for (int i = 0; i < kPointLights; ++i)
    {
        std::string name = "point_lights[";
        name += std::to_string(i) + ']';

        shader_->setUniform(name + ".position", point_light_positions[i]);

        shader_->setUniform(name + ".ambient", glm::vec3(0.2f));
        shader_->setUniform(name + ".diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_->setUniform(name + ".specular", glm::vec3(1.0f, 1.0f, 1.0f));

        shader_->setUniform(name + ".attenuation_constant", 1.0f);
        shader_->setUniform(name + ".attenuation_linear", 0.09f);
        shader_->setUniform(name + ".attenuation_quadratic", 0.032f);
    }
}

void Photographer::clearBackground_()
{
    //glClearColor(0.85f, 0.8f, 0.8f, 1.0f);   // state-setting function
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);   // state-setting function
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // state-using function
}

void Photographer::cameraParamsToShader_(Shader& shader, Camera& camera)
{
    shader.use();
    shader.setUniform("view", camera.getGlViewMatrix());
    shader.setUniform("projection", camera.getGlProjectionMatrix());
    shader.setUniform("eye_pos", camera.getPosition());
}

void Photographer::drawMainObjects_(Shader& shader)
{
    shader.use();
    glBindVertexArray(this->object_vertex_array_);
   
    for (int i = 0; i < num_cubes_; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube_positions_[i]);
        if (i % 3 == 1)
        {
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
        }
        else
        {
            float angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
        }
        shader.setUniform("model", model);
        shader.setUniform("normal_matrix", glm::transpose(glm::inverse(model)));
        glDrawElements(GL_TRIANGLES, object_->getFaces().size(), GL_UNSIGNED_INT, 0); // second argument is the tot number of vertices to draw
    }
}

GLFWwindow* Photographer::initWindowContext_(bool visible)
{
    glfwInit();

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!visible)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

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

    // TUTORIAL for a mouse control
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void Photographer::initCustomBuffer_()
{
    // create framebuffer
    if (!framebuffer_)
    {
        glGenFramebuffers(1, &framebuffer_);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    }   
    else // or just reuse previous framebuffer
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    }

    if (!texture_color_buffer_)
    {
        glGenTextures(1, &texture_color_buffer_);
        glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
        // dimention should match current Viewport dimentions (call glViewport if the change is needed)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, win_width_, win_height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // attach to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer_, 0);
    } // or reuse
    
    if (!depth_render_buffer_)
    {
        glGenRenderbuffers(1, &depth_render_buffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_render_buffer_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, win_width_, win_height_);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer_);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::RenderToImage:: Framebuffer is not complete!" << std::endl;
    }
}

void Photographer::cleanAndCloseContext_()
{
    // object-related. Should always be there
    glDeleteVertexArrays(1, &object_vertex_array_);
    glDeleteBuffers(1, &object_vertex_buffer_);
    glDeleteBuffers(1, &object_element_buffer_);
    object_vertex_array_ = object_element_buffer_ = object_vertex_buffer_ = 0;

    if (framebuffer_)
    {
        glDeleteFramebuffers(1, &framebuffer_);
        framebuffer_ = 0;
    }
    if (texture_color_buffer_)
    {
        glDeleteTextures(1, &texture_color_buffer_);
        texture_color_buffer_ = 0;
    }

    if (shader_ != nullptr)
    {
        delete shader_;
        shader_ = nullptr;
    }
    
    if (view_camera_ != nullptr)
    {
        delete view_camera_;
        view_camera_ = nullptr;
    }

    glfwTerminate();
}

void Photographer::saveRGBTexToFile(const std::string filename, unsigned int texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    int width, height, n_channels, internal_format;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);

    if (internal_format == GL_RGB) n_channels = 3;
    else if (internal_format == GL_RGBA) n_channels = 4;
    else
    {
        std::cout << "ERROR::WRITING TEXTURE TO FILE::Unknown texture format " << internal_format << std::endl;
    }

    std::vector<unsigned char> image(width *  height * n_channels);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

    stbi_flip_vertically_on_write(true);    // Gl texture coord system is upside down
    int success = stbi_write_png(filename.c_str(), width, height, n_channels, image.data(), 0);

    if (!success)
    {
        // don't know how to get the failure reason info
        std::cout << "ERROR::WRITING TEXTURE TO FILE::Failed to save png." 
            << "Check that the path specified exists:" << std::endl
            << filename << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void Photographer::registerCallbacks_(GLFWwindow * window)
{
    glfwSetFramebufferSizeCallback(window, Photographer::framebufferSizeCallback_);
    glfwSetCursorPosCallback(window, Photographer::mouseCallback);
    glfwSetScrollCallback(window, Photographer::scrollCallback);
}

void Photographer::processInput_(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // camera control
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        view_camera_->movePosition(view_camera_->FORWARD, delta_time_);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        view_camera_->movePosition(view_camera_->BACKWARD, delta_time_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        view_camera_->movePosition(view_camera_->LEFT, delta_time_);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        view_camera_->movePosition(view_camera_->RIGHT, delta_time_);
}

void Photographer::framebufferSizeCallback_(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Photographer::mouseCallback(GLFWwindow * window, double xpos, double ypos)
{
    if (first_mouse_)
    {
        lastX_ = xpos;
        lastY_ = ypos;
        first_mouse_ = false;
    }

    float xoffset = xpos - lastX_;
    float yoffset = lastY_ - ypos; // reversed since y-coordinates range from bottom to top
    lastX_ = xpos;
    lastY_ = ypos;

    view_camera_->updateRotation(yoffset, xoffset);
}

void Photographer::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    view_camera_->zoom(yoffset);
}
