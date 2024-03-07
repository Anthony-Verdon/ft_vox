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

    void convertBlocks();
    int faceIndexInMesh(const std::vector<float> &newFaceCoords);
    void addFaceToMesh(const std::vector<float> &newFaceCoords);
    int vertexIndexInMesh(const std::vector<float>::const_iterator &start,
                          const std::vector<float>::const_iterator &end);
    std::vector<float> findFaceCoords(unsigned int faceStartIndex) const;

  public:
    ChunkMesh() = delete;
    ChunkMesh(const ChunkData &data);
    ChunkMesh(const ChunkMesh &instance);
    ChunkMesh &operator=(const ChunkMesh &instance);
    ~ChunkMesh();

    void initMesh();

    unsigned int getVAO() const;
    std::vector<float> getVertices() const;
    std::vector<unsigned int> getFaces() const;
};