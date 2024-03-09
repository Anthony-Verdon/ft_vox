#include "WorldData.hpp"
#include <iostream>
#include <memory>

WorldData::WorldData()
{
    updateChunksLoad(0, 0);
}

WorldData::WorldData(const WorldData &instance)
{
    *this = instance;
}
WorldData &WorldData::operator=(const WorldData &instance)
{
    if (this != &instance)
    {
        for (int x = 0; x < RENDER_DISTANCE; x++)
        {
            for (int y = 0; y < RENDER_DISTANCE; y++)
            {
                ChunkMesh chunkMesh(*(instance.getChunk(x, y).get()));
                chunks.push_back(std::make_unique<ChunkMesh>(chunkMesh));
            }
        }
    }
    return (*this);
}

WorldData::~WorldData()
{
}

void WorldData::updateChunksLoad(int xOrigin, int zOrigin)
{
    chunks.clear();
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    for (int i = -RENDER_DISTANCE / 2; i < RENDER_DISTANCE / 2; i++)
    {
        for (int j = -RENDER_DISTANCE / 2; j < RENDER_DISTANCE / 2; j++)
        {
            ChunkData chunkData;
            for (int x = 0; x < CHUNK_LENGTH; x++)
            {
                for (int y = 0; y < 1; y++)
                {
                    for (int z = 0; z < CHUNK_LENGTH; z++)
                    {
                        int posX = xOrigin * CHUNK_LENGTH + x + i * CHUNK_LENGTH;
                        int posZ = zOrigin * CHUNK_LENGTH + z + j * CHUNK_LENGTH;
                        BlockData block(posX, y, posZ, texturePattern);
                        chunkData.addBlock(block);
                    }
                }
            }
            ChunkMesh chunkMesh(chunkData);
            chunkMesh.initMesh();
            chunks.push_back(std::make_unique<ChunkMesh>(chunkMesh));
        }
    }
}

const std::unique_ptr<ChunkMesh> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE + y]);
}

size_t WorldData::getNbChunks() const
{
    return (chunks.size());
}