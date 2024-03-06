#pragma once

#include "../ChunkData/ChunkData.hpp"
#include <vector>

class ChunkMesh : public ChunkData
{
  private:
    std::vector<float> vertices;
    std::vector<unsigned int> faces;

    bool isInit;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

  public:
    ChunkMesh() = delete;
    ChunkMesh(const ChunkData &data);
    ChunkMesh(const ChunkMesh &instance);
    ChunkMesh &operator=(const ChunkMesh &instance);
    ~ChunkMesh();

    void initMesh();
    void convertBlocks();

    unsigned int getVAO() const;
    std::vector<float> getVertices() const;
    std::vector<unsigned int> getFaces() const;
};