
#include "BlockMesh.hpp"
#include <algorithm>

constexpr int TILESET_WIDTH = 2;
constexpr int TILESET_HEIGHT = 2;

// possibility to reduce size of initCoords
constexpr float initCoords[8 * 3] = {
    0, 0, 0, //
    1, 0, 0, //
    0, 0, 1, //
    1, 0, 1, //
    0, 1, 0, //
    1, 1, 0, //
    0, 1, 1, //
    1, 1, 1, //
};

constexpr float initTexturesCoords[4 * 2] = {
    0.0 / TILESET_WIDTH, 0.0 / TILESET_HEIGHT, //
    1.0 / TILESET_WIDTH, 0.0 / TILESET_HEIGHT, //
    0.0 / TILESET_WIDTH, 1.0 / TILESET_HEIGHT, //
    1.0 / TILESET_WIDTH, 1.0 / TILESET_HEIGHT, //
};

constexpr unsigned int initFaces[12 * 6] = {
    // y = 1 | 4, 5, 6, 7,
    4, 5, 6, /**/ 0, 1, 2, //
    5, 6, 7, /**/ 1, 2, 3, // top
    // x = 0 | 0, 2, 4, 6,
    0, 2, 4, /**/ 0, 1, 2, //
    2, 4, 6, /**/ 1, 2, 3, // left
    // x = 1 | 1, 3, 5, 7,
    1, 3, 5, /**/ 0, 1, 2, //
    3, 5, 7, /**/ 1, 2, 3, // right
    // z = 0 | 0, 1, 4, 5,
    0, 1, 4, /**/ 0, 1, 2, //
    1, 4, 5, /**/ 1, 2, 3, // back
    // z = 1 | 2, 3, 6, 7,
    2, 3, 6, /**/ 0, 1, 2, //
    3, 6, 7, /**/ 1, 2, 3, // front
    // y = 0 | 0, 1, 2, 3,
    0, 1, 2, /**/ 0, 1, 2, //
    1, 2, 3, /**/ 1, 2, 3, // bottom
};

BlockMesh::BlockMesh(const BlockData &data) : BlockData(data)
{
    initMesh();
}

BlockMesh::~BlockMesh()
{
}

BlockMesh::BlockMesh(const BlockMesh &instance)
    : BlockData(instance.getX(), instance.getY(), instance.getZ(), instance.getTextureCoords())
{
    initMesh();
}

BlockMesh &BlockMesh::operator=(const BlockMesh &instance)
{
    if (this != &instance)
    {
        x = instance.getX();
        y = instance.getY();
        z = instance.getZ();
        textureCoords = instance.getTextureCoords();
        initMesh();
    }
    return (*this);
}

void BlockMesh::initMesh()
{
    for (unsigned int i = 0; i < 12 * 6; i += 6)
    {
        for (unsigned int j = 0; j < 3; j++)
            faces.push_back(combineVertices(initFaces[i + j], initFaces[i + j + 3], i / 12));
    }
}

unsigned int BlockMesh::combineVertices(unsigned int vertexIndex, unsigned int textureVertexIndex, int side)
{
    std::vector<float> vertexCombined(5);
    vertexCombined[0] = initCoords[vertexIndex * 3 + 0] + x;
    vertexCombined[1] = initCoords[vertexIndex * 3 + 1] + y;
    vertexCombined[2] = initCoords[vertexIndex * 3 + 2] + z;

    std::pair<unsigned int, unsigned int> texture = textureCoords[side];
    vertexCombined[3] =
        initTexturesCoords[textureVertexIndex * 2 + 0] + static_cast<float>(texture.first) / TILESET_WIDTH;
    vertexCombined[4] =
        initTexturesCoords[textureVertexIndex * 2 + 1] + static_cast<float>(texture.second) / TILESET_HEIGHT;

    int vertexCombinedIndex = vertexIndexInMesh(vertexCombined);
    if (vertexCombinedIndex == -1)
    {
        for (unsigned int i = 0; i < 5; i++)
            vertices.push_back(vertexCombined[i]);
        return (vertices.size() / 5 - 1);
    }
    else
        return (vertexCombinedIndex / 5);
}

int BlockMesh::vertexIndexInMesh(const std::vector<float> &vertex)
{
    auto it = vertices.begin();
    while (it != vertices.end())
    {
        it = std::search(it, vertices.end(), vertex.begin(), vertex.end());
        if (it != vertices.end())
        {
            unsigned int distance = std::distance(vertices.begin(), it++);
            if (distance % 5 == 0)
                return (distance);
        }
    }
    return -1;
}

size_t BlockMesh::nbVertices() const
{
    return (vertices.size());
}

size_t BlockMesh::nbFaces() const
{
    return (faces.size());
}

float BlockMesh::getVertex(unsigned int index) const
{
    return (vertices[index]);
}

unsigned int BlockMesh::getFace(unsigned int index) const
{
    return (faces[index]);
}