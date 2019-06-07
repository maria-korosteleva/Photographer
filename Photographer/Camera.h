#pragma once

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
// It also holds the projection matrix
// Move rotations to something better

#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    enum Directions {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };
    enum Mode
    {
        TARGET_MODE, 
        FREE_MODE
    };


    Camera(int screen_width, int screen_height, float field_of_view = default_fov_);
    ~Camera();

    glm::mat4 getGlViewMatrix();
    glm::mat4 getGlProjectionMatrix();
    //glm::mat4 GetCVProjectionMatrix();

    void setPosition(glm::vec3 pos);
    void setRotation(float pitch, float yaw);
    void setTarget(glm::vec3 target);
    //void SetFrontVector(glm::vec3 front);

    // TUTORIAL
    void movePosition(Directions direction, float step_size_multiplier = 1.0f);
    void updateRotation(float delta_pitch, float delta_yaw, bool constrain_pitch = true);
    void zoom(float delta);

    glm::vec3 getPosition() { return position_; }
    glm::vec3 getFrontVector() { return front_; }

    unsigned int getID() { return ID_; }

private:
    static unsigned int avalible_camera_id;
    static constexpr float default_fov_ = 45.0f;
    // manipulation
    static constexpr float rotation_sensitivity_ = 0.05f;
    static constexpr float zoom_sensitivity_ = 0.01f;
    static constexpr float movement_speed_ = 2.5f;

    // Calculates the front vector 
    // from the Camera's (updated) Euler Angles
    void updateVectorsByRotation_();
    // based on the target
    void updateFrontByTarget_();

    // ID
    unsigned int ID_;

    // mode
    Mode mode_;

    // extrinsic
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    const glm::vec3 world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    // extra -- rotation parameters
    float yaw_, pitch_;
    glm::vec3 target_;

    // intrinsic 
    float field_of_view_;
    float screen_width_;
    float screen_height_;

};

