#pragma once

#include "../ChunkMesh/ChunkMesh.hpp"

class ChunkRenderer : public ChunkMesh
{
  private:
    unsigned int VAO[MeshType::MESH_TYPE_COUNT];
    unsigned int VBO[MeshType::MESH_TYPE_COUNT];
    unsigned int EBO[MeshType::MESH_TYPE_COUNT];

    void initRenderer();

  public:
    ChunkRenderer() = delete;
    ChunkRenderer(const ChunkMesh &chunkMesh);
    ChunkRenderer(const ChunkRenderer &instance) = delete;
    ChunkRenderer &operator=(const ChunkRenderer &instance) = delete;
    ~ChunkRenderer();

    unsigned int getVAO(MeshType type) const;
};