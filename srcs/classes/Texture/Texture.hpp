#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <string>

class Texture
{
  private:
    void loadTexture(const std::string &path);
    void loadSkybox(const std::string &path);

    unsigned int ID;

  public:
    Texture() = delete;
    Texture(const Texture &instance) = delete;
    Texture &operator=(const Texture &instance) = delete;

    Texture(const std::string &path);
    ~Texture();

    unsigned int getID() const;
};