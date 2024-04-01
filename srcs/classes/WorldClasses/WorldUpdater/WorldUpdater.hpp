#pragma once

#include "../../ChunkClasses/ChunkMesh/ChunkMesh.hpp"
#include <mutex>
#include <stack>

class WorldUpdater
{
    private:
        std::mutex chunksToLoadMutex;
        std::stack<std::array<int, 4>> chunksToLoad;

        std::mutex chunksLoadedMutex;
        std::stack<ChunkData> chunksLoaded;

        std::mutex stopThreadMutex;
        bool stopThread;

        void loadNewChunks();
        ChunkData initChunkData(int modifierX, int modifierZ);
    
    public:
        WorldUpdater();
        ~WorldUpdater();
        void addChunkToLoad(int x, int z, int arrayX, int arrayZ);
        std::optional<ChunkData> getChunkLoaded();
};