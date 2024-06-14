#pragma once

#include <array>
#include <glm/glm.hpp>

enum DIRECTIONS
{
    TOP,
    FRONT,
    RIGHT,
    BACK,
    LEFT,
    BOTTOM
};

class BlockData
{
  protected:
    glm::vec3 coords;
    std::array<std::pair<unsigned int, unsigned int>, 6> textureCoords;

  public:
    BlockData() = delete;
    BlockData(int x, int y, int z, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords);
    BlockData(const glm::vec3 &coords, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords);
    BlockData(const BlockData &instance);
    BlockData &operator=(const BlockData &instance);
    ~BlockData();

    glm::vec3 getCoords() const;
    int getX() const;
    int getY() const;
    int getZ() const;
    std::array<std::pair<unsigned int, unsigned int>, 6> getTextureCoords() const;
};