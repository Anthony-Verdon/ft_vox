#include "GameRenderer.hpp"
#include "../GameLogic/GameLogic.hpp"
#include "../Time/Time.hpp"
#include "../TextRenderer/TextRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../ChunkClasses/ChunkGenerator/ChunkGenerator.hpp"
#include <iostream>

std::map<std::string, std::unique_ptr<Texture>> GameRenderer::textures;
std::map<std::string, std::unique_ptr<Shader>> GameRenderer::shaders;

void GameRenderer::Init()
{
    textures["tileset"] = std::make_unique<Texture>("assets/textures/tileset.png");
    textures["skybox"] = std::make_unique<Texture>("assets/textures/skybox/");
    shaders["world"] = std::make_unique<Shader>("srcs/shaders/WorldShader/WorldShader.vs", "srcs/shaders/WorldShader/WorldShader.fs");
    shaders["skybox"] = std::make_unique<Shader>("srcs/shaders/SkyboxShader/SkyboxShader.vs", "srcs/shaders/SkyboxShader/SkyboxShader.fs");
    shaders["line"] = std::make_unique<Shader>("srcs/shaders/LineShader/LineShader.vs", "srcs/shaders/LineShader/LineShader.fs");
}

void GameRenderer::Render()
{
    glm::mat4 view = glm::lookAt(GameLogic::camera.getPosition(), GameLogic::camera.getPosition() + GameLogic::camera.getFrontDirection(), GameLogic::camera.getUpDirection());

    float viewDistance = RENDER_DISTANCE_2X * CHUNK_LENGTH;
    if (viewDistance < CHUNK_HEIGHT)
        viewDistance = CHUNK_HEIGHT;
    glm::mat4 projection = glm::perspective(glm::radians(GameLogic::camera.getFOV()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, viewDistance);

    WorldRendering(view, projection);
    SkyboxRendering(view, projection);
    if (GameLogic::data.infoMode)
    {
        ChunkBorderRendering(view, projection);
        DebugInfoRendering();
    }
    ChatRendering();

}

void GameRenderer::WorldRendering(const glm::mat4 &view, const glm::mat4 &projection)
{
    shaders["world"]->use();
    shaders["world"]->setMat4("view", view);
    shaders["world"]->setMat4("projection", projection);
    GameLogic::world.updateWorldData(GameLogic::camera.getPosition().x, GameLogic::camera.getPosition().z);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures["tileset"]->getID());
    for (int i = 0; i < MeshType::MESH_TYPE_COUNT; i++)
    {
        switch (i)
        {
        case MeshType::OPAQUE:
            shaders["world"]->setFloat("opacity", 1.0);
            break;
        case MeshType::TRANSLUCENT:
            shaders["world"]->setFloat("opacity", 0.5);
            break;
        }
        for (size_t x = 0; x < RENDER_DISTANCE_2X; x++)
        {
            for (size_t y = 0; y < RENDER_DISTANCE_2X; y++)
            {
                const std::unique_ptr<ChunkRenderer> &chunk = GameLogic::world.getChunk(x, y);
                if (!chunk)
                    continue;
                glBindVertexArray(chunk->getVAO(static_cast<MeshType>(i)));
                glDrawElements(GL_TRIANGLES, chunk->getFaces(static_cast<MeshType>(i)).size(), GL_UNSIGNED_INT, 0);
            }
        }
    }
}
void GameRenderer::SkyboxRendering(const glm::mat4 &view, const glm::mat4 &projection)
{
    static SkyboxRenderer skybox;
    
    glDepthFunc(GL_LEQUAL);
    shaders["skybox"]->use();
    shaders["skybox"]->setMat4("view", glm::mat4(glm::mat3(view)));
    shaders["skybox"]->setMat4("projection", projection);
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, Time::getTime() / DAY_DURATION_IN_SEC * glm::radians(50.0f),
                            glm::vec3(1.0f, 0.0f, 0.0f));
    shaders["skybox"]->setMat4("rotation", rotation);
    glBindVertexArray(skybox.getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures["skybox"]->getID());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}

void GameRenderer::ChunkBorderRendering(const glm::mat4 &view, const glm::mat4 &projection)
{
    static LineRenderer line(LineData(glm::vec3(0, 0, 0), glm::vec3(0, 256, 0), glm::vec3(255, 0, 0)));

    shaders["line"]->use();
    shaders["line"]->setMat4("view", view);
    shaders["line"]->setMat4("projection", projection);
    shaders["line"]->setVec3("color", glm::vec3(255, 255, 0));
    for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE + 1; x++)
    {
        for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE + 1; z++)
        {
            glm::vec3 camPosition = GameLogic::camera.getPosition();
            glm::vec2 linePosition = {(int)camPosition.x - (int)camPosition.x % CHUNK_LENGTH + x * CHUNK_LENGTH,
                                        (int)camPosition.z - (int)camPosition.z % CHUNK_LENGTH +
                                            z * CHUNK_LENGTH};
            if (camPosition.x < 0)
                linePosition.x -= CHUNK_LENGTH;
            if (camPosition.z < 0)
                linePosition.y -= CHUNK_LENGTH;
            line.setStartPoint({linePosition.x, 0, linePosition.y});
            line.setEndPoint({linePosition.x, CHUNK_HEIGHT, linePosition.y});
            line.draw(); // @todo create draw function on all renderer

            for (int y = 0; y < CHUNK_HEIGHT; y += MINICHUNK_HEIGHT)
            {
                if (x != RENDER_DISTANCE + 1)
                {
                    line.setStartPoint({linePosition.x, y, linePosition.y});
                    line.setEndPoint({linePosition.x + CHUNK_LENGTH, y, linePosition.y});
                    line.draw();
                }
                if (z != RENDER_DISTANCE + 1)
                {
                    line.setStartPoint({linePosition.x, y, linePosition.y});
                    line.setEndPoint({linePosition.x, y, linePosition.y + CHUNK_LENGTH});
                    line.draw();
                }
            }
        }
    }
}
void GameRenderer::DebugInfoRendering()
{
    static glm::vec3 cameraOldPosition = GameLogic::camera.getPosition();
    glm::vec3 cameraNewPosition = GameLogic::camera.getPosition();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    const float scaling = 0.5f;
    TextRenderer::renderText("X: " + std::to_string(GameLogic::camera.getPosition().x), 0.0f,
                             WINDOW_HEIGHT - 1 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText("Y: " + std::to_string(GameLogic::camera.getPosition().y), 0.0f,
                             WINDOW_HEIGHT - 2 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText("Z: " + std::to_string(GameLogic::camera.getPosition().z), 0.0f,
                             WINDOW_HEIGHT - 3 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText("FPS: " + std::to_string(static_cast<int>(std::round(1.0f / Time::getDeltaTime()))), 0.0f,
                             WINDOW_HEIGHT - 4 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    float distanceMade =
        sqrt(pow(cameraNewPosition.x - cameraOldPosition.x, 2) + pow(cameraNewPosition.y - cameraOldPosition.y, 2) +
             pow(cameraNewPosition.z - cameraOldPosition.z, 2));
    TextRenderer::renderText("speed : " + std::to_string(distanceMade / Time::getDeltaTime()) + " blocks per second",
                             0.0f, WINDOW_HEIGHT - 5 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));

    TextRenderer::renderText("continentalness value : " +
                                 std::to_string((ChunkGenerator::getFractalNoise(
                                     ChunkGenerator::GetNoisePosition(cameraNewPosition.x, true),
                                     ChunkGenerator::GetNoisePosition(cameraNewPosition.z, false),
                                     CONTINENTALNESS_OCTAVES, CONTINENTALNESS_FREQUENCY, CONTINENTALNESS_PERSISTENCE))),
                             0.0f, WINDOW_HEIGHT - 6 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                             glm::vec4(1, 1, 1, 1));
    TextRenderer::renderText(
        "peak and valleys value : " +
            std::to_string(ChunkGenerator::getFractalNoise(ChunkGenerator::GetNoisePosition(cameraNewPosition.x, true),
                                                           ChunkGenerator::GetNoisePosition(cameraNewPosition.z, false),
                                                           PV_OCTAVES, PV_FREQUENCY, PV_PERSISTENCE)),
        0.0f, WINDOW_HEIGHT - 7 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling, glm::vec4(1, 1, 1, 1));
    if (GameLogic::data.wireframeMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    cameraOldPosition = cameraNewPosition;
}

void GameRenderer::ChatRendering()
{
    const float scaling = 0.5f;
    if (GameLogic::data.inputMode == CHAT)
    {
        TextRenderer::renderText("message : " + GameLogic::data.message, 0.0f,
                                 WINDOW_HEIGHT - 10 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                                 glm::vec4(1, 1, 1, 1));
    }
    else if (GameLogic::data.inputMode == GAME)
    {
        if (Time::getTime() - GameLogic::data.lastMessageTimeStamp < CHAT_DISPLAY_TIME + CHAT_FADE_TIME)
        {
            float fading;
            if (Time::getTime() - GameLogic::data.lastMessageTimeStamp < CHAT_DISPLAY_TIME)
                fading = 1;
            else
                fading = 1 - (((Time::getTime() - GameLogic::data.lastMessageTimeStamp) - CHAT_DISPLAY_TIME) / CHAT_FADE_TIME);
            TextRenderer::renderText("last message : " + GameLogic::data.lastMessage, 0.0f,
                                     WINDOW_HEIGHT - 10 * static_cast<float>(TEXT_PIXEL_SIZE) * scaling, scaling,
                                     glm::vec4(1, 1, 1, fading));
        }
    }
}
