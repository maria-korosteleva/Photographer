#pragma once

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
// It also holds the projection matrix

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

    Camera(int screen_width, int screen_height, float field_of_view = default_fov_);
    ~Camera();

    glm::mat4 GetGlViewMatrix();
    glm::mat4 GetGlProjectionMatrix();
    //glm::mat4 GetCVProjectionMatrix();

    void SetPosition(glm::vec3 pos);
    void SetRotation(float pitch, float yaw);
    //void SetTarget(glm::vec3 target);
    //void SetFrontVector(glm::vec3 front);

    // TUTORIAL
    void MovePosition(Directions direction, float step_size_multiplier = 1.0f);
    void UpdateRotation(float delta_pitch, float delta_yaw, bool constrain_pitch = true);
    void Zoom(float delta);

    glm::vec3 position() { return position_; }
    glm::vec3 front_vector() { return front_; }

private:
    static constexpr float default_fov_ = 45.0f;
    // TUTORIAL
    static constexpr float rotation_sensitivity_ = 0.05f;
    static constexpr float movement_speed_ = 2.5f;

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraRotation();

    // extrinsic
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    const glm::vec3 world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    // extra -- rotation parameters
    float yaw_, pitch_;

    // intrinsic 
    float field_of_view_;
    float screen_width_;
    float screen_height_;

};

