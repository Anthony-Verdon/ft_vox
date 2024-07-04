#pragma once

#include "../BlockData/BlockData.hpp"
#include <cstddef>
#include <vector>

class BlockMesh : public BlockData
{
  private:
    std::vector<float> vertices;
    std::vector<unsigned int> faces;

    unsigned int combineVertices(unsigned int vertexIndex, unsigned int textureVertexIndex, int side);
    int vertexIndexInMesh(const std::vector<float> &vertex);

  public:
    BlockMesh() = delete;
    BlockMesh(int x, int y, int z, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords,
              const std::array<bool, 6> neighborsExist);
    BlockMesh(const glm::vec3 &blockCoords, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords,
              const std::array<bool, 6> neighborsExist);
    BlockMesh(const BlockData &data, const std::array<bool, 6> neighborsExist);
    BlockMesh(const BlockMesh &instance);
    BlockMesh &operator=(const BlockMesh &instance);
    ~BlockMesh();

    void initMesh(const std::array<bool, 6> &neighborsExist);

    size_t nbVertices() const;
    size_t nbFaces() const;
    float getVertex(unsigned int index) const;
    unsigned int getFace(unsigned int index) const;
    std::vector<float> getVertices() const;
    std::vector<unsigned int> getFaces() const;
};