#pragma once

#include <array>
#include <glm/glm.hpp>

enum BlockFace
{
    LEFT,
    RIGHT,
    BOTTOM,
    TOP,
    BACK,
    FRONT
};

class BlockData
{
  protected:
    glm::vec3 coords;
    std::array<std::pair<unsigned int, unsigned int>, 6> textureCoords;
    bool isTranslucent;

  public:
    BlockData() = delete;
    BlockData(int x, int y, int z, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords,
              bool isTranslucent);
    BlockData(const glm::vec3 &coords, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords,
              bool isTranslucent);
    BlockData(const BlockData &instance);
    BlockData &operator=(const BlockData &instance);
    ~BlockData();

    void setCoords(int x, int y, int z);
    void setCoords(const glm::vec3 &coord);

    glm::vec3 getCoords() const;
    int getX() const;
    int getY() const;
    int getZ() const;
    std::array<std::pair<unsigned int, unsigned int>, 6> getTextureCoords() const;
    bool getIsTranslucent() const;
};