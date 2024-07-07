#pragma once

#include "../LineData/LineData.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class LineRenderer : public LineData
{
  private:
    unsigned int VAO;
    unsigned int VBO;
    std::vector<float> vertices;

  public:
    LineRenderer() = delete;
    LineRenderer(const LineData &instance);
    ~LineRenderer();

    void draw() const;
};