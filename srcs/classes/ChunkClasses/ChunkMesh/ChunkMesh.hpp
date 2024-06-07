#pragma once

#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
#include "../ChunkData/ChunkData.hpp"
#include <optional>
#include <vector>

class ChunkMesh : public ChunkData
{
  private:
    void addBlockMesh(const BlockMesh &blockMesh);

  protected:
    std::vector<float> vertices;
    std::vector<unsigned int> faces;

  public:
    ChunkMesh() = delete;
    ChunkMesh(const ChunkData &data);
    ChunkMesh(const ChunkMesh &instance);
    ChunkMesh &operator=(const ChunkMesh &instance);
    ~ChunkMesh();

    void initMesh();

    std::vector<float> getVertices() const;
    std::vector<unsigned int> getFaces() const;
};
