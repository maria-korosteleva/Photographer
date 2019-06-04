//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>

#include "Photographer.h"

float Photographer::lastX_ = 400;
float Photographer::lastY_ = 300;
bool Photographer::first_mouse_ = true;
Camera* Photographer::camera_ = nullptr;

Photographer::Photographer()
{
}

Photographer::~Photographer()
{
}

int Photographer::run()
{
    GLFWwindow* window = initWindowContext_();
    registerCallbacks_(window);
    
    // The scene
    createObjectVAO_();
    createShaders_();
    setUpObjectColor_();
    setUpLight_();

    // Camera
    camera_ = new Camera(win_width_, win_height_);
    camera_->setPosition(glm::vec3(0.0f, 1.0f, 5.0f));

    // operating the view
    Photographer::lastX_ = 400;
    Photographer::lastY_ = 300;
    first_mouse_ = true;
    last_frame_time_ = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // TUTORIAL input
        this->processInput_(window);

        // Backgournd render commands
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);   // state-setting function
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // state-using function

        // move light 
        //float radius = 2;
        //dir_light_.x = radius * sin(glfwGetTime());
        //dir_light_.z = radius * cos(glfwGetTime());

        // Draw
        drawLightCubes_();
        drawMainObjects_();

        // ----- finish
        // swap the buffers and check all call events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // rendering time
        float currentFrame = glfwGetTime();
        delta_time_ = currentFrame - last_frame_time_;
        last_frame_time_ = currentFrame;
    }

    cleanAndCloseContext_();
    return 0;
}

void Photographer::setObject(GeneralMesh * object)
{
    object_ = object;
}

void Photographer::createObjectVAO_()
{
    // id avalibility check
    if (object_vertex_array_ > 0
        || light_vertex_array_ >0 
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts_), cube_verts_, GL_STATIC_DRAW);

    // setup element buffer object -- for faces!
    //glGenBuffers(1, &object_element_buffer_);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_element_buffer_);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->rectangle_faces_), rectangle_faces_, GL_STATIC_DRAW);

    // Vertex data interpretation guide
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ------ Light object
    glGenVertexArrays(1, &light_vertex_array_);
    glBindVertexArray(light_vertex_array_);

    glBindBuffer(GL_ARRAY_BUFFER, object_vertex_buffer_);
    // interptrtation guide -- vertices only
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Cleaning. Unbinding is not necessary, but I'm doing this for completeness
    // Important to unbind GL_ELEMENT_ARRAY_BUFFER after Vertex Array
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int Photographer::loadTexture_(const char * filename, bool alpha)
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

void Photographer::createShaders_()
{
    if (shader_ != nullptr) delete shader_;
    shader_ = new Shader(vertex_shader_path_, fragment_shader_path_);
    if (light_shader_ != nullptr) delete light_shader_;
    light_shader_ = new Shader(vertex_shader_path_, light_fragment_shader_path_);
}

void Photographer::setUpObjectColor_()
{
    shader_->use();

    tex_container_ = loadTexture_(tex_container_path_);
    tex_steel_border_ = loadTexture_(tex_steel_border_path_, true);
    tex_face_ = loadTexture_(tex_smiley_path_, true);

    shader_->setUniform("material.diffuse", 0);   // container
    shader_->setUniform("material.specular", 2);   // face
    shader_->setUniform("material.shininess", 128.0f);
}

void Photographer::setUpLight_()
{
    // directional
    shader_->setUniform("directional_light.direction", dir_light_);
    shader_->setUniform("directional_light.ambient", glm::vec3(0.2f));
    shader_->setUniform("directional_light.diffuse", glm::vec3(0.0f, 0.0f, 0.7f));
    shader_->setUniform("directional_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    // point lights
    for (int i = 0; i < kPointLights; ++i)
    {
        std::string name = "point_lights[";
        name += std::to_string(i) + ']';

        shader_->setUniform(name + ".position", point_light_positions_[i]);

        shader_->setUniform(name + ".ambient", glm::vec3(0.2f));
        shader_->setUniform(name + ".diffuse", glm::vec3(0.5f, 0.0f, 0.0f));
        shader_->setUniform(name + ".specular", glm::vec3(1.0f, 1.0f, 1.0f));

        shader_->setUniform(name + ".attenuation_constant", 1.0f);
        shader_->setUniform(name + ".attenuation_linear", 0.09f);
        shader_->setUniform(name + ".attenuation_quadratic", 0.032f);
    }

    // spotlight properties
    // positions and directions are defined on the go
    shader_->setUniform("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
    shader_->setUniform("spot_light.outer_cut_off", glm::cos(glm::radians(18.0f)));
    
    // color components
    shader_->setUniform("spot_light.ambient", glm::vec3(0.2f));
    shader_->setUniform("spot_light.diffuse", glm::vec3(0.0f, 0.5f, 0.5f));
    shader_->setUniform("spot_light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

void Photographer::drawLightCubes_()
{
    light_shader_->use();

    // camera -- could change
    light_shader_->setUniform("view", camera_->getGlViewMatrix());
    light_shader_->setUniform("projection", camera_->getGlProjectionMatrix());

    glBindVertexArray(light_vertex_array_);
    for (int i = 0; i < kPointLights; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, point_light_positions_[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        light_shader_->setUniform("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void Photographer::drawMainObjects_()
{
    shader_->use();

    // TUTORIAL bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_container_);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_face_);

    // camera
    shader_->setUniform("view", camera_->getGlViewMatrix());
    shader_->setUniform("projection", camera_->getGlProjectionMatrix());
    shader_->setUniform("eye_pos", camera_->getPosition());

    // light
    shader_->setUniform("spot_light.direction", camera_->getFrontVector());
    shader_->setUniform("spot_light.position", camera_->getPosition());

    // draw cubes
    glBindVertexArray(this->object_vertex_array_);

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
        shader_->setUniform("model", model);
        shader_->setUniform("normal_matrix", glm::transpose(glm::inverse(model)));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    //glDrawElements(GL_TRIANGLES, this->kRectangleFacesArrSize, GL_UNSIGNED_INT, 0); // second argument is the tot number of vertices to draw

}

GLFWwindow* Photographer::initWindowContext_()
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

    // TUTORIAL for a mouse control
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}

void Photographer::registerCallbacks_(GLFWwindow * window)
{
    glfwSetFramebufferSizeCallback(window, Photographer::framebufferSizeCallback_);
    glfwSetCursorPosCallback(window, Photographer::mouseCallback);
    glfwSetScrollCallback(window, Photographer::scrollCallback);
}

void Photographer::cleanAndCloseContext_()
{
    glDeleteVertexArrays(1, &object_vertex_array_);
    glDeleteBuffers(1, &object_vertex_buffer_);
    glDeleteBuffers(1, &object_element_buffer_);
    glDeleteTextures(1, &tex_container_);
    glDeleteTextures(1, &tex_face_);
    delete shader_;
    delete camera_;
    glfwTerminate();

    shader_ = nullptr;
    camera_ = nullptr;
    object_vertex_array_ = object_element_buffer_ = object_vertex_buffer_ = tex_container_ = tex_face_ = 0;
}

void Photographer::processInput_(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // camera control
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_->movePosition(camera_->FORWARD, delta_time_);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_->movePosition(camera_->BACKWARD, delta_time_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_->movePosition(camera_->LEFT, delta_time_);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_->movePosition(camera_->RIGHT, delta_time_);
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

    camera_->updateRotation(yoffset, xoffset);
}

void Photographer::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    camera_->zoom(yoffset);
}
