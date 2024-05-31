#pragma once

#include "../Shader/Shader.hpp"
#include <glm/glm.hpp>
#include <map>
#include <string>

typedef struct s_FreetypeCharacter
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
} t_FreetypeCharacter;

class TextRenderer
{
  private:
    static unsigned int VAO;
    static unsigned int VBO;

    static std::map<char, t_FreetypeCharacter> characters;

    TextRenderer() = delete;
    ~TextRenderer() = delete;

  public:
    static void initTextRenderer();
    static void destructTextRenderer();
    static void renderText(const std::string &text, float x, float y, float scale, const glm::vec4 &color);
};