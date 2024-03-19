#include "Camera.hpp"
#include "../Time/Time.hpp"

Camera::Camera()
{
    position = glm::vec3(0.0f, 200.0f, 0.0f);
    frontDirection = glm::vec3(1.0f, 1.0f, 1.0f);
    upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
    rightDirection = glm::normalize(glm::cross(frontDirection, upDirection));
    yaw = -90.0f;
    pitch = 0.0f;
    speed = 10.00;
    fov = 80.0f;
}

Camera::~Camera()
{
}

void Camera::updatePosition(int frontAxis, int rightAxis, int upAxis)
{
    glm::vec3 update = static_cast<float>(frontAxis) * frontDirection + static_cast<float>(rightAxis) * rightDirection +
                       static_cast<float>(upAxis) * upDirection;
    if (update != glm::vec3(0, 0, 0))
        position += glm::normalize(update) * speed * Time::getDeltaTime();
}

void Camera::updateDirection()
{
    glm::vec3 newDirection = glm::vec3(cosf(glm::radians(yaw)) * cosf(glm::radians(pitch)), sinf(glm::radians(pitch)),
                                       sinf(glm::radians(yaw)) * cosf(glm::radians(pitch)));
    frontDirection = glm::normalize(newDirection);
    rightDirection = glm::normalize(glm::cross(frontDirection, upDirection));
}

void Camera::setPosition(const glm::vec3 &position)
{
    this->position = position;
}

void Camera::setYaw(float yaw)
{
    this->yaw = yaw;
}

void Camera::setPitch(float pitch)
{
    this->pitch = pitch;
}

void Camera::setSpeed(float speed)
{
    this->speed = speed;
}

void Camera::addToYaw(float yaw)
{
    this->yaw += yaw;
}

void Camera::addToPitch(float pitch)
{
    this->pitch += pitch;
}

glm::vec3 Camera::getPosition() const
{
    return (position);
}

glm::vec3 Camera::getFrontDirection() const
{
    return (frontDirection);
}

glm::vec3 Camera::getUpDirection() const
{
    return (upDirection);
}

float Camera::getYaw() const
{
    return (yaw);
}

float Camera::getPitch() const
{
    return (pitch);
}

float Camera::getSpeed() const
{
    return (speed);
}

float Camera::getFOV() const
{
    return (fov);
}