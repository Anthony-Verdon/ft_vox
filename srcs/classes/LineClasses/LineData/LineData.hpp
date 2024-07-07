#pragma once

#include <glm/glm.hpp>

class LineData
{
  protected:
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 color;

  public:
    LineData() = delete;
    LineData(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color);
    ~LineData();
    LineData(const LineData &instance);
    LineData &operator=(const LineData &instance);

    void setStartPoint(const glm::vec3 &start);
    void setEndPoint(const glm::vec3 &end);
    void setColor(const glm::vec3 &color);

    glm::vec3 getStartPoint() const;
    glm::vec3 getEndPoint() const;
    glm::vec3 getColor() const;
};