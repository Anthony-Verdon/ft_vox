#pragma once

#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
#include "../ChunkData/ChunkData.hpp"
#include <optional>
#include <vector>

enum MeshType
{
    OPAQUE, // ex: grass
    // TRANSPARENT, // ex: glass
    TRANSLUCENT, // ex: water
    MESH_TYPE_COUNT
};

class ChunkMesh : public ChunkData
{
  private:
    void addBlockMesh(const BlockMesh &blockMesh);

  protected:
    std::array<std::vector<float>, MeshType::MESH_TYPE_COUNT> vertices;
    std::array<std::vector<unsigned int>, MeshType::MESH_TYPE_COUNT> faces;

  public:
    ChunkMesh() = delete;
    ChunkMesh(const ChunkData &data);
    ChunkMesh(const ChunkMesh &instance);
    ChunkMesh &operator=(const ChunkMesh &instance);
    ~ChunkMesh();

    void initMesh();

    std::vector<float> getVertices(MeshType type) const;
    std::vector<unsigned int> getFaces(MeshType type) const;
};
