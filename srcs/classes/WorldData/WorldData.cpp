#include "WorldData.hpp"
#include <memory>

WorldData::WorldData()
{
    std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern;
    // bottom left is 0/0 and top right is 1/1
    for (int i = 0; i < 6; i++)
        texturePattern[i] = {0, 1}; // side
    texturePattern[2] = {1, 1};     // top
    texturePattern[3] = {0, 0};     // bottom

    for (int i = 0; i < RENDER_DISTANCE; i++)
    {
        for (int j = 0; j < RENDER_DISTANCE; j++)
        {
            ChunkData chunkData;
            for (int x = i * CHUNK_LENGTH; x < (i + 1) * CHUNK_LENGTH; x++)
            {
                for (int y = 0; y < 1; y++)
                {
                    for (int z = j * CHUNK_LENGTH; z < (j + 1) * CHUNK_LENGTH; z++)
                    {
                        BlockData block(x, y, z, texturePattern);
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

const std::unique_ptr<ChunkMesh> &WorldData::getChunk(int x, int y) const
{
    return (chunks[x * RENDER_DISTANCE + y]);
}

size_t WorldData::getNbChunks() const
{
    return (chunks.size());
}