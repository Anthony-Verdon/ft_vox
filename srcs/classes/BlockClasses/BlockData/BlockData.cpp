#include "BlockData.hpp"

BlockData::BlockData(int x, int y, int z, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords)
{
    *this = BlockData(glm::vec3(x, y, z), textureCoords);
}

BlockData::BlockData(const glm::vec3 &coords, const std::array<std::pair<unsigned int, unsigned int>, 6> &textureCoords)
{
    this->coords = coords;
    this->textureCoords = textureCoords;
}

BlockData::BlockData(const BlockData &instance)
{
    *this = instance;
}

BlockData &BlockData::operator=(const BlockData &instance)
{
    if (this != &instance)
    {
        coords = instance.getCoords();
        textureCoords = instance.getTextureCoords();
    }
    return (*this);
}

BlockData::~BlockData()
{
}

glm::vec3 BlockData::getCoords() const
{
    return (coords);
}

int BlockData::getX() const
{
    return (coords.x);
}

int BlockData::getY() const
{
    return (coords.y);
}

int BlockData::getZ() const
{
    return (coords.z);
}

std::array<std::pair<unsigned int, unsigned int>, 6> BlockData::getTextureCoords() const
{
    return (textureCoords);
}
