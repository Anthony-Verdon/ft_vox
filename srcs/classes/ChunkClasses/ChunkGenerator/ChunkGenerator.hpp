#pragma once
#include "../ChunkData/ChunkData.hpp"
class ChunkGenerator
{
  private:
    static unsigned long seed;
    static float modifierX;
    static float modifierZ;

  public:
    ChunkGenerator() = delete;
    ~ChunkGenerator() = delete;

    static void SetSeed(unsigned long seed);
    static ChunkData GenerateChunkData(int x, int z);
    static float getNoiseValue(int x, int z);
    static float getFractalNoise(float x, float z, int octaves, float frequency, float persistence);

#ifdef GENERATE_MAP
#endif
    static void GenerateNoiseMap();
    static void generateImage(const std::string &name, int octaves, float frequency, float persistence,
                              int roundFactor);
};