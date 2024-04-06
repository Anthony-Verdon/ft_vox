#pragma once

#include "../ChunkMesh/ChunkMesh.hpp"

class ChunkRenderer : public ChunkMesh
{
  private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void initRenderer();

  public:
    ChunkRenderer() = delete;
    ChunkRenderer(const ChunkMesh &chunkMesh);
    ChunkRenderer(const ChunkRenderer &instance) = delete;
    ChunkRenderer &operator=(const ChunkRenderer &instance) = delete;
    ~ChunkRenderer();

    unsigned int getVAO() const;
};