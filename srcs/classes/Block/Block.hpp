#pragma once

#include <array>
class Block
{
  private:
    Block() = delete;

    unsigned int VAO;

  public:
    Block(int x, int y, int z, std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern);

    unsigned int getVAO() const;
};