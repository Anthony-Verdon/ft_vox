#pragma once
#include "../ChunkData/ChunkData.hpp"

#ifndef GENERATE_MAP
#define GENERATE_MAP //@todo suppress, just use it for visual code
#endif

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
    static unsigned long GetSeed();
    static ChunkData GenerateChunkData(int x, int z);

#ifdef GENERATE_MAP
    static float getValueFromGraph(float value, eGraphType graph);
    static float getFractalNoise(float x, float z, int octaves, float frequency, float persistence);
    static void GenerateNoiseMap();
    static void generateImage(const std::string &name, int octaves, float frequency, float persistence, int roundFactor,
                              bool drawColor, bool ridgedNoise);
#endif
};