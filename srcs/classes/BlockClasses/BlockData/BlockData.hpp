#pragma once

#include <array>
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
    int x;
    int y;
    int z;

    std::array<std::pair<unsigned int, unsigned int>, 6> textureCoords;

  public:
    BlockData() = delete;
    BlockData(int x, int y, int z, std::array<std::pair<unsigned int, unsigned int>, 6>);
    BlockData(const BlockData &instance);
    BlockData &operator=(const BlockData &instance);
    ~BlockData();

    int getX() const;
    int getY() const;
    int getZ() const;
    std::array<std::pair<unsigned int, unsigned int>, 6> getTextureCoords() const;
};