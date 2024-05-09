#pragma once
#include "../ChunkData/ChunkData.hpp"
class ChunkGenerator
{
  private:
    static unsigned long seed;

  public:
    ChunkGenerator() = delete;
    ~ChunkGenerator() = delete;

    static void SetSeed(unsigned long seed);
    static ChunkData GenerateChunkData(int x, int z);
};