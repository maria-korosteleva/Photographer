#include "Camera.h"

unsigned int Camera::avalible_camera_id = 1000;

Camera::Camera(int screen_width, int screen_height, float field_of_view)
    :screen_width_(screen_width), screen_height_(screen_height), field_of_view_y_(field_of_view)
{
    mode_ = FREE_MODE;

    ID_ = Camera::avalible_camera_id;
    Camera::avalible_camera_id++;

    position_ = glm::vec3(0.0f, 0.0f, 0.0f);
    yaw_ = -90.0f;
    pitch_ = 0.0f;

    updateVectorsByRotation_();
}

Camera::~Camera()
{
}

glm::mat4 Camera::getGlViewMatrix()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getGlProjectionMatrix()
{
    return glm::perspective(glm::radians(field_of_view_y_), screen_width_ / screen_height_, 0.1f, 100.0f);
}

glm::mat3 Camera::getCVIntrinsicsMatrix()
{
    glm::mat3 intrinsics = glm::mat3(1.0f);  // identity

    //float ratio = screen_width_ / screen_height_;

    // tan(fov_x) = ratio * tan(fov_y)
    // float pix_focal_x = screen_width_ / (2 * ratio * tan(glm::radians(field_of_view_y_) / 2));
    // same for x and y, since we assume that apperture ratio matches the output image aspect ratio
    float pix_focal = screen_height_ / (2 * tan(glm::radians(field_of_view_y_) / 2));

    intrinsics[0][0] = pix_focal;
    intrinsics[1][1] = pix_focal;
    intrinsics[0][2] = screen_width_ / 2;
    intrinsics[1][2] = screen_height_ / 2;

    return intrinsics;
}

void Camera::setPosition(glm::vec3 pos)
{
    position_ = pos;

    if (mode_ == TARGET_MODE)
    {
        updateFrontByTarget_();
    }
}

void Camera::setRotation(float pitch, float yaw)
{
    if (mode_ == TARGET_MODE)
    {
        std::cout << "INFO:: CAMERA " << ID_ << " SWITCHED FROM TARGET MODE TO FREE ROTATION" << std::endl;
        mode_ = FREE_MODE;
    }

    pitch_ = pitch;
    yaw_ = yaw;
    updateVectorsByRotation_();
}

void Camera::setTarget(glm::vec3 target)
{
    std::cout << "INFO:: CAMERA " << ID_ << " SWITCHED FROM FREE ROTATION TO TARGET MODE" << std::endl;

    mode_ = TARGET_MODE;
    target_ = target;
    updateFrontByTarget_();
}

void Camera::movePosition(Directions direction, float step_size_multiplier)
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

    if (mode_ == TARGET_MODE)
    {
        updateFrontByTarget_();
    }
}

void Camera::updateRotation(float delta_pitch, float delta_yaw, bool constrain_pitch)
{
    if (mode_ == TARGET_MODE)
    {
        std::cout << "INFO:: CAMERA " << ID_ << " SWITCHED FROM TARGET MODE TO FREE ROTATION" << std::endl;
        mode_ = FREE_MODE;
    }

    delta_pitch *= rotation_sensitivity_;
    delta_yaw *= rotation_sensitivity_;

    pitch_ += delta_pitch;
    yaw_ = glm::mod(yaw_ + delta_yaw, 360.0f);

    if (constrain_pitch)
    {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    updateVectorsByRotation_();
}

void Camera::zoom(float delta)
{
    if (field_of_view_y_ >= 1.0f && field_of_view_y_ <= default_fov_)
        field_of_view_y_ -= zoom_sensitivity_ * delta;

    // check boundaries
    if (field_of_view_y_ <= 1.0f)             field_of_view_y_ = 1.0f;
    if (field_of_view_y_ >= default_fov_)      field_of_view_y_ = default_fov_;
}

void Camera::saveParamsForOpenCV(const std::string path)
{
    glm::mat3 intrinsics = getCVIntrinsicsMatrix();

    glm::mat4 extrinsic = glm::transpose(getGlViewMatrix()); // transposed because of the colomn-wise storage
    //glm::mat4 turn_y_180 = glm::mat4(1.0);
    //turn_y_180[0][0] = -1.0f;
    //turn_y_180[2][2] = -1.0f;
    //extrinsic = turn_y_180 * extrinsic; // yes, right, the whole resulting scene is rotated!

    // Save
    // very hardcore approach. But no new dependencies!
    std::ofstream xml_file;
    xml_file.open(path + "/" + std::to_string(ID_) + ".xml");

    xml_file << "<?xml version=\"1.0\"?>" << std::endl
        << "<opencv_storage>" << std::endl;

    // extrinsic
    xml_file << "<CameraMatrix type_id=\"opencv-matrix\">" << std::endl
        << "\t<rows>3</rows>" << std::endl
        << "\t<cols>4</cols>" << std::endl
        << "\t<dt>d</dt>" << std::endl
        << "\t<data>" << std::endl;
    for (int i = 0; i < 3; ++i)
    {
        xml_file << "\t\t";
        for (int j = 0; j < 4; ++j)
        {
            xml_file << extrinsic[i][j] << " ";     
        }
        xml_file << std::endl;
    }
    xml_file << "\t</data>\n</CameraMatrix>" << std::endl;
    
    xml_file << "<Intrinsics type_id=\"opencv-matrix\">" << std::endl
        << "\t<rows>3</rows>" << std::endl
        << "\t<cols>3</cols>" << std::endl
        << "\t<dt>d</dt>" << std::endl
        << "\t<data>" << std::endl;

    for (int i = 0; i < 3; ++i)
    {
        xml_file << "\t\t";
        for (int j = 0; j < 3; ++j)
        {
            xml_file << intrinsics[i][j] << " ";
        }
        xml_file << std::endl;
    }
    
    xml_file << "\t</data>\n</Intrinsics>" << std::endl;

    // No distortion
    xml_file << "<Distortion type_id=\"opencv-matrix\">" << std::endl
        << "\t<rows>8</rows>" << std::endl
        << "\t<cols>1</cols>" << std::endl
        << "\t<dt>d</dt>" << std::endl
        << "\t<data> 0. 0. 0. 0. 0. 0. 0. 0.</data>" << std::endl
        << "</Distortion>" << std::endl;

    xml_file << "</opencv_storage>" << std::endl;
    xml_file.close();
}

void Camera::updateVectorsByRotation_()
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

void Camera::updateFrontByTarget_()
{
    front_ = glm::normalize(target_ - position_);

    // re-calculate the Right and Up vector
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
