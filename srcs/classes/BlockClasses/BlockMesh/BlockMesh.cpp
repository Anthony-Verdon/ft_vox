#include "BlockMesh.hpp"

#define TILESET_WIDTH 2
#define TILESET_HEIGHT 2

// possibility to reduce size of initCoords
float BlockMesh::initCoords[NB_VERTICES * 5] = {
    // top
    0.0, 1.0, 0.0, 0.0, 0.0, //
    1.0, 1.0, 0.0, 0.5, 0.0, //
    0.0, 1.0, 1.0, 0.0, 0.5, //
    1.0, 1.0, 1.0, 0.5, 0.5, //

    // sides
    0.0, 0.0, 0.0, 0.0, 0.0, //
    1.0, 0.0, 0.0, 0.5, 0.0, //
    0.0, 1.0, 0.0, 0.0, 0.5, //
    1.0, 1.0, 0.0, 0.5, 0.5, //

    0.0, 0.0, 0.0, 0.0, 0.0, //
    0.0, 1.0, 0.0, 0.0, 0.5, //
    0.0, 0.0, 1.0, 0.5, 0.0, //
    0.0, 1.0, 1.0, 0.5, 0.5, //

    0.0, 0.0, 1.0, 0.0, 0.0, //
    1.0, 0.0, 1.0, 0.5, 0.0, //
    0.0, 1.0, 1.0, 0.0, 0.5, //
    1.0, 1.0, 1.0, 0.5, 0.5, //

    1.0, 0.0, 0.0, 0.0, 0.0, //
    1.0, 1.0, 0.0, 0.0, 0.5, //
    1.0, 0.0, 1.0, 0.5, 0.0, //
    1.0, 1.0, 1.0, 0.5, 0.5, //

    // bot
    0.0, 0.0, 0.0, 0.0, 0.0, //
    1.0, 0.0, 0.0, 0.5, 0.0, //
    0.0, 0.0, 1.0, 0.0, 0.5, //
    1.0, 0.0, 1.0, 0.5, 0.5, //
};

unsigned int BlockMesh::initFaces[12 * 3] = {
    0,  1,  2, //
    1,  2,  3, //

    4,  5,  6, //
    5,  6,  7, //

    8,  9,  10, //
    9,  10, 11, //

    12, 13, 14, //
    13, 14, 15, //

    16, 17, 18, //
    17, 18, 19, //

    20, 21, 22, //
    21, 22, 23  //
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
    for (int i = 0; i < NB_VERTICES * 5; i += 5)
    {
        vertices[i + 0] = initCoords[i + 0] + x;
        vertices[i + 1] = initCoords[i + 1] + y;
        vertices[i + 2] = initCoords[i + 2] + z;
        std::pair<unsigned int, unsigned int> texture = textureCoords[i / (4 * 5)];
        vertices[i + 3] = initCoords[i + 3] + static_cast<float>(texture.first) / TILESET_WIDTH;
        vertices[i + 4] = initCoords[i + 4] + static_cast<float>(texture.second) / TILESET_HEIGHT;
    }
    for (int i = 0; i < NB_FACES * 2 * 3; i++)
        faces[i] = initFaces[i];
}

float BlockMesh::getVertex(unsigned int index) const
{
    return (vertices[index]);
}

unsigned int BlockMesh::getFace(unsigned int index) const
{
    return (faces[index]);
}