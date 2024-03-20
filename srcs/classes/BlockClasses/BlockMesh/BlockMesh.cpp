
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

    /*
    if {x,y,z} is 0, first triangle is clock-wise, second triangle is counter clock-wise
    if it's 1, first triangle is counter clock-wise, second triangle is clock-wise
    */
    // x = 0 | 0, 2, 4, 6,
    0, 2, 4, /**/ 0, 1, 2, // left
    6, 4, 2, /**/ 3, 2, 1, //
    // x = 1 | 1, 3, 5, 7,
    5, 3, 1, /**/ 2, 1, 0, // right
    3, 5, 7, /**/ 1, 2, 3, //
    // y = 0 | 0, 1, 2, 3,
    0, 1, 2, /**/ 0, 1, 2, // bottom
    3, 2, 1, /**/ 3, 2, 1, //
    // y = 1 | 4, 5, 6, 7,
    6, 5, 4, /**/ 2, 1, 0, // top
    5, 6, 7, /**/ 1, 2, 3, //
    // z = 0 | 0, 1, 4, 5,
    4, 1, 0, /**/ 2, 1, 0, // back
    1, 4, 5, /**/ 1, 2, 3, //
    // z = 1 | 2, 3, 6, 7,
    2, 3, 6, /**/ 0, 1, 2, // front
    7, 6, 3, /**/ 3, 2, 1, //
};

BlockMesh::BlockMesh(const BlockData &data, const std::array<bool, 6> neighborsExist) : BlockData(data)
{
    initMesh(neighborsExist);
}

BlockMesh::~BlockMesh()
{
}

BlockMesh::BlockMesh(const BlockMesh &instance)
    : BlockData(instance.getX(), instance.getY(), instance.getZ(), instance.getTextureCoords())
{
    *this = instance;
}

BlockMesh &BlockMesh::operator=(const BlockMesh &instance)
{
    if (this != &instance)
    {
        x = instance.getX();
        y = instance.getY();
        z = instance.getZ();
        textureCoords = instance.getTextureCoords();
        vertices = instance.getVertices();
        faces = instance.getFaces();
    }
    return (*this);
}

void BlockMesh::initMesh(const std::array<bool, 6> neighborsExist)
{
    for (unsigned int i = 0; i < 6; i++)
    {
        if (neighborsExist[i])
            continue;
        for (unsigned int j = 0; j < 2; j++)
        {
            for (unsigned int k = 0; k < 3; k++)
            {
                int vertexIndex = i * 12 + j * 6 + k;
                faces.push_back(combineVertices(initFaces[vertexIndex], initFaces[vertexIndex + 3], i));
            }
        }
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

std::vector<float> BlockMesh::getVertices() const
{
    return (vertices);
}

std::vector<unsigned int> BlockMesh::getFaces() const
{
    return (faces);
}