#pragma once
#include "../ChunkData/ChunkData.hpp"
#include <map>
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

    static std::map<std::pair<int, int>, std::vector<std::pair<glm::vec3, BlockType>>> featuresToPlace;
    static std::vector<glm::vec2> chunkGeneratedOnce;

    static void GenerateTerrain(ChunkData &chunkData);
    static void GenerateFeatures(ChunkData &chunkData);

  public:
    ChunkGenerator() = delete;
    ~ChunkGenerator() = delete;

    static void SetSeed(unsigned long seed);

    static unsigned long GetSeed();
    static ChunkData GenerateChunkData(int x, int z, bool firstIteration = true);

    static float GetNoisePosition(int pos, bool xCoord);
    static float getFractalNoise(float x, float z, int octaves, float frequency, float persistence);
#ifdef GENERATE_MAP
    static float getValueFromGraph(float value, eGraphType graph);
    static void GenerateNoiseMap();
    static void generateImage(const std::string &name, int octaves, float frequency, float persistence, int roundFactor,
                              bool drawColor, bool ridgedNoise);
#endif
};