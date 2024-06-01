#pragma once
#include "../ChunkData/ChunkData.hpp"
class ChunkGenerator
{
  private:
    static unsigned long seed;
    static float modifierX;
    static float modifierZ;

    static std::unique_ptr<unsigned char[]> Zoom(int scale, const std::unique_ptr<unsigned char[]> &mapBeforeZoom);
    static void AddIsland();

  public:
    ChunkGenerator() = delete;
    ~ChunkGenerator() = delete;

    static void SetSeed(unsigned long seed);
    static ChunkData GenerateChunkData(int x, int z);

    static void GenerateMap();

    static float getNoiseValue(int x, int z);
};