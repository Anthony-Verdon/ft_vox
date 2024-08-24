#pragma once
#include "../ChunkData/ChunkData.hpp"
#include <map>
enum eGraphType
{
    CONTINENTALNESS_GRAPH,
    EROSION_GRAPH,
    PV_GRAPH
};

// Custom comparator for glm::vec2 to use it as a key in a map
struct Vec2Compare
{
    bool operator()(const glm::vec<2, float, glm::packed_highp> &lhs,
                    const glm::vec<2, float, glm::packed_highp> &rhs) const
    {
        if (lhs.x < rhs.x)
            return true;
        if (lhs.x > rhs.x)
            return false;
        return lhs.y < rhs.y;
    }
};

class ChunkGenerator
{
  private:
    static unsigned long seed;
    static float modifierX;
    static float modifierY;
    static float modifierZ;

    static std::map<glm::vec2, std::vector<std::pair<glm::vec3, BlockType>>, Vec2Compare> featuresToPlace;
    static std::vector<glm::vec2> chunkGeneratedOnce;

    static void GenerateTerrain(ChunkData &chunkData);
    static void GenerateFeatures(ChunkData &chunkData);
    static void GenerateTree(const glm::vec2 &chunkCoord, const glm::vec3 &treeCoord);
    static void AddFeatures(ChunkData &chunkData);

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