#include "BlockData.hpp"
#include <memory>

#define TILESET_WIDTH 2
#define TILESET_HEIGHT 2

BlockData::BlockData(int x, int y, int z, std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern)
{
    this->x = x;
    this->y = y;
    this->z = z;
    float baseCoord[] = {
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

    vertices = std::make_unique<float[]>(4 * 5 * 6);
    for (unsigned int i = 0; i < 4 * 5 * 6; i += 5)
    {
        vertices[i + 0] = baseCoord[i + 0] + x;
        vertices[i + 1] = baseCoord[i + 1] + y;
        vertices[i + 2] = baseCoord[i + 2] + z;
        vertices[i + 3] = baseCoord[i + 3] + static_cast<float>(texturePattern[i / (4 * 5)].first) / TILESET_WIDTH;
        vertices[i + 4] = baseCoord[i + 4] + static_cast<float>(texturePattern[i / (4 * 5)].second) / TILESET_HEIGHT;
    }

    unsigned int baseFaces[] = {
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

    faces = std::make_unique<unsigned int[]>(3 * 2 * 6);
    for (unsigned int i = 0; i < 3 * 2 * 6; i++)
        faces[i] = baseFaces[i];
}

BlockData::BlockData(const BlockData &instance)
{
    x = instance.getX();
    y = instance.getY();
    z = instance.getZ();
    vertices = std::make_unique<float[]>(4 * 5 * 6);
    const std::unique_ptr<float[]> &instanceVertices = instance.getVertices();
    for (unsigned int i = 0; i < 4 * 5 * 6; i++)
        vertices[i] = instanceVertices[i];
    const std::unique_ptr<unsigned int[]> &instanceFaces = instance.getFaces();
    faces = std::make_unique<unsigned int[]>(3 * 2 * 6);
    for (unsigned int i = 0; i < 3 * 2 * 6; i++)
        faces[i] = instanceFaces[i];
}

BlockData::~BlockData()
{
}

int BlockData::getX() const
{
    return (x);
}

int BlockData::getY() const
{
    return (y);
}

int BlockData::getZ() const
{
    return (z);
}

const std::unique_ptr<float[]> &BlockData::getVertices() const
{
    return (vertices);
}

const std::unique_ptr<unsigned int[]> &BlockData::getFaces() const
{
    return (faces);
}