#pragma once

#include <string>

class SkyboxRenderer
{
  private:
    unsigned int VAO;
    unsigned int VBO;

  public:
    SkyboxRenderer();
    ~SkyboxRenderer();

    unsigned int getVAO();
};