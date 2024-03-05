#pragma once

#include "../BlockData/BlockData.hpp"
#include <vector>

class Chunk
{
  private:
    std::vector<BlockData> blocks;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    bool init;

    std::unique_ptr<float[]> vertices;
    std::unique_ptr<unsigned int[]> faces;
    void convertBlocks();

  public:
    unsigned int getVAO() const;
    bool isInit() const;

    Chunk();
    ~Chunk();

    void loadChunk();
    void updateChunk();
    void unloadChunk();

    std::vector<BlockData> getBlocks() const;
    void addBlock(const BlockData &block);
};