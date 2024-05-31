#include "TextRenderer.hpp"
#include "../../globals.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

unsigned int TextRenderer::VAO = -1;
unsigned int TextRenderer::VBO = -1;
std::map<char, t_FreetypeCharacter> TextRenderer::characters;

void TextRenderer::initTextRenderer()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        throw(std::runtime_error("INIT_FREETYPE::INITIALIZATION_FAILED"));

    FT_Face face;
    if (FT_New_Face(ft, "assets/fonts/arial.ttf", 0, &face))
        throw(std::runtime_error("INIT_FREETYPE::FONT_LOADING_FAILED"));

    FT_Set_Pixel_Sizes(face, 0, TEXT_PIXEL_SIZE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYTPE::GLYPH_" << toupper(c) << "_LOADING_FAILED" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        t_FreetypeCharacter freetypeCharacter = {
            texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), (unsigned int)face->glyph->advance.x};
        characters.insert(std::pair<char, t_FreetypeCharacter>(c, freetypeCharacter));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::destructTextRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void TextRenderer::renderText(const std::string &text, float x, float y, float scale, const glm::vec4 &color)
{
    static Shader textShader("srcs/shaders/TextShader/TextShader.vs", "srcs/shaders/TextShader/TextShader.fs");

    textShader.use();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
    textShader.setMat4("projection", projection);
    textShader.setVec4("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (size_t i = 0; i < text.length(); i++)
    {
        unsigned char character = text[i];
        t_FreetypeCharacter freetypeCharacter = characters[character];

        float xpos = x + freetypeCharacter.Bearing.x * scale;
        float ypos = y - (freetypeCharacter.Size.y - freetypeCharacter.Bearing.y) * scale;

        float w = freetypeCharacter.Size.x * scale;
        float h = freetypeCharacter.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f}, {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, freetypeCharacter.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (freetypeCharacter.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}