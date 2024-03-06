#pragma once

#include "../BlockData/BlockData.hpp"
#include <cstddef>
#include <vector>

class BlockMesh : public BlockData
{
  private:
    std::vector<float> vertices;
    std::vector<unsigned int> faces;

  public:
    BlockMesh() = delete;
    BlockMesh(const BlockData &data);
    BlockMesh(const BlockMesh &instance);
    BlockMesh &operator=(const BlockMesh &instance);
    ~BlockMesh();

    void initMesh();
    unsigned int combineVertices(unsigned int vertexIndex, unsigned int textureVertexIndex, int side);
    int vertexIndexInMesh(const std::vector<float> &vertex);

    size_t nbVertices() const;
    size_t nbFaces() const;
    float getVertex(unsigned int index) const;
    unsigned int getFace(unsigned int index) const;
};