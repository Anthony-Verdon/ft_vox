#pragma once

#include <string>
#include <map>
#include "../Texture/Texture.hpp"
#include "../Shader/Shader.hpp"
#include "../SkyboxRenderer/SkyboxRenderer.hpp"
#include "../LineClasses/LineRenderer/LineRenderer.hpp"
#include <memory>

class GameRenderer
{
    private:
        static std::map<std::string, std::unique_ptr<Texture>> textures;
        static std::map<std::string, std::unique_ptr<Shader>> shaders;

        static void WorldRendering(const glm::mat4 &view, const glm::mat4 &projection);
        static void SkyboxRendering(const glm::mat4 &view, const glm::mat4 &projection);
        static void ChunkBorderRendering(const glm::mat4 &view, const glm::mat4 &projection);
        static void DebugInfoRendering();
        static void ChatRendering();

    public:
        static void Init();
        static void Render();
};