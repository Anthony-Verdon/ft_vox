#pragma once
#include "../ChunkData/ChunkData.hpp"

enum eGraphType
{
    CONTINENTALNESS_GRAPH,
    EROSION_GRAPH,
    PV_GRAPH
};

class ChunkGenerator
{
  private:
    static unsigned long seed;
    static float modifierX;
    static float modifierY;
    static float modifierZ;

  public:
    ChunkGenerator() = delete;
    ~ChunkGenerator() = delete;

    static void SetSeed(unsigned long seed);
    static ChunkData GenerateChunkData(int x, int z);
};