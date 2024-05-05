#pragma once

#include <glm/glm.hpp>

class Camera
{
  private:
    glm::vec3 position;
    glm::vec3 frontDirection;
    glm::vec3 rightDirection;
    glm::vec3 upDirection;
    float yaw;
    float pitch;
    float speed;
    float fov;

  public:
    Camera(const Camera &instance) = delete;
    Camera &operator=(const Camera &instance) = delete;

    Camera();
    ~Camera();

    void updatePosition(int frontAxis, int rightAxis, int upAxis);
    void updateDirection();

    void setPosition(const glm::vec3 &position);
    void setYaw(float yaw);
    void setPitch(float pitch);
    void setSpeed(float speed);

    void addToYaw(float yaw);
    void addToPitch(float pitch);

    glm::vec3 getPosition() const;
    glm::vec3 getFrontDirection() const;
    glm::vec3 getUpDirection() const;
    float getYaw() const;
    float getPitch() const;
    float getSpeed() const;
    float getFOV() const;
};