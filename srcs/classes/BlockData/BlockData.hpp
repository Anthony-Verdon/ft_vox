#pragma once

#include <array>
#include <memory>

class BlockData
{
  private:
    int x;
    int y;
    int z;

    std::unique_ptr<float[]> vertices;
    std::unique_ptr<unsigned int[]> faces;

  public:
    BlockData(int x, int y, int z, std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern);
    BlockData(const BlockData &instance);
    ~BlockData();

    int getX() const;
    int getY() const;
    int getZ() const;

    const std::unique_ptr<float[]> &getVertices() const;
    const std::unique_ptr<unsigned int[]> &getFaces() const;
};