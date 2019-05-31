#include "Camera.h"


Camera::Camera(int screen_width, int screen_height, float field_of_view)
    :screen_width_(screen_width), screen_height_(screen_height), field_of_view_(field_of_view)
{
    position_ = glm::vec3(0.0f, 0.0f, 0.0f);
    yaw_ = -90.0f;
    pitch_ = 0.0f;

    UpdateCameraRotation();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetGlViewMatrix()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::GetGlProjectionMatrix()
{
    return glm::perspective(field_of_view_, screen_width_ / screen_height_, 0.1f, 100.0f);
}

void Camera::SetPosition(glm::vec3 pos)
{
    position_ = pos;
}

void Camera::SetRotation(float pitch, float yaw)
{
    pitch_ = pitch;
    yaw_ = yaw;
    UpdateCameraRotation();
}

void Camera::MovePosition(Directions direction, float step_size_multiplier)
{
    float velocity = movement_speed_ * step_size_multiplier;

    switch (direction)
    {
        case Directions::FORWARD:
            position_ += front_ * velocity;
            break;
        case Directions::BACKWARD:
            position_ -= front_ * velocity;
            break;
        case Directions::RIGHT:
            position_ += right_ * velocity;
            break;
        case Directions::LEFT:
            position_ -= right_ * velocity;
            break;
    }
}

void Camera::UpdateRotation(float delta_pitch, float delta_yaw, bool constrain_pitch)
{
    delta_pitch *= rotation_sensitivity_;
    delta_yaw *= rotation_sensitivity_;

    pitch_ += delta_pitch;
    yaw_ = glm::mod(yaw_ + delta_yaw, 360.0f);

    if (constrain_pitch)
    {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    UpdateCameraRotation();
}

void Camera::Zoom(float delta)
{
    if (field_of_view_ >= 1.0f && field_of_view_ <= default_fov_)
        field_of_view_ -= delta;

    // check boundaries
    if (field_of_view_ <= 1.0f)             field_of_view_ = 1.0f;
    if (field_of_view_ >= default_fov_)      field_of_view_ = default_fov_;
}

void Camera::UpdateCameraRotation()
{
    // New Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);

    // re-calculate the Right and Up vector
    right_ = glm::normalize(glm::cross(front_, world_up_)); 
    up_ = glm::normalize(glm::cross(right_, front_));
}
