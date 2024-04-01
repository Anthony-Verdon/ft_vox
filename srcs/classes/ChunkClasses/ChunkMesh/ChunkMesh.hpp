#pragma once

#include "../../BlockClasses/BlockMesh/BlockMesh.hpp"
#include "../ChunkData/ChunkData.hpp"
#include <vector>
#include <optional>

class ChunkMesh : public ChunkData
{
  private:
    std::vector<float> vertices;
    std::vector<unsigned int> faces;

    bool init;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void convertChunkDataIntoMesh(const std::array<std::optional<ChunkData>, 4> &neighborsChunks);
    void addBlockMesh(const BlockMesh &blockMesh);

  public:
    ChunkMesh() = delete;
    ChunkMesh(const ChunkData &data);
    ChunkMesh(const ChunkMesh &instance);
    ChunkMesh &operator=(const ChunkMesh &instance);
    ~ChunkMesh();

    void initMesh(const std::array<std::optional<ChunkData>, 4> &neighborsChunks);

    unsigned int getVAO() const;
    std::vector<float> getVertices() const;
    std::vector<unsigned int> getFaces() const;
    bool isInit() const;
};
