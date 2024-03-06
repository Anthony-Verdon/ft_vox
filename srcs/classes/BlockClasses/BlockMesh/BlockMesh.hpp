#pragma once

#include "../BlockData/BlockData.hpp"

#define NB_VERTICES 24
#define NB_FACES 6

class BlockMesh : public BlockData
{
  private:
    float vertices[NB_VERTICES * 5];
    unsigned int faces[NB_FACES * 2 * 3];

    static float initCoords[NB_VERTICES * 5];
    static unsigned int initFaces[NB_FACES * 2 * 3];

  public:
    BlockMesh() = delete;
    BlockMesh(const BlockData &data);
    BlockMesh(const BlockMesh &instance);
    BlockMesh &operator=(const BlockMesh &instance);
    ~BlockMesh();

    void initMesh();
    float getVertex(unsigned int index) const;
    unsigned int getFace(unsigned int index) const;
};