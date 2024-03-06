#include "BlockData.hpp"

BlockData::BlockData(int x, int y, int z, std::array<std::pair<unsigned int, unsigned int>, 6> textureCoords)
{
    this->x = x;
    this->y = y;
    this->z = z;
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
        x = instance.getX();
        y = instance.getY();
        z = instance.getZ();
        textureCoords = instance.getTextureCoords();
    }
    return (*this);
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

std::array<std::pair<unsigned int, unsigned int>, 6> BlockData::getTextureCoords() const
{
    return (textureCoords);
}
