#include "Block.hpp"
#include "../../../libs/glad/include/glad/glad.h"
#include <iostream>

#define TILESET_WIDTH 2
#define TILESET_HEIGHT 2
Block::Block(int x, int y, int z, std::array<std::pair<unsigned int, unsigned int>, 6> texturePattern)
{
    unsigned int VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    float vertices[] = {
        // top texture 0/0
        0.0, 1.0, 0.0, 0.0, 0.0, //
        1.0, 1.0, 0.0, 0.5, 0.0, //
        0.0, 1.0, 1.0, 0.0, 0.5, //
        1.0, 1.0, 1.0, 0.5, 0.5, //

        // sides texture 0/1
        0.0, 0.0, 0.0, 0.0, 0.0, //
        1.0, 0.0, 0.0, 0.5, 0.0, //
        0.0, 1.0, 0.0, 0.0, 0.5, //
        1.0, 1.0, 0.0, 0.5, 0.5, //

        0.0, 0.0, 0.0, 0.0, 0.0, //
        0.0, 1.0, 0.0, 0.0, 0.5, //
        0.0, 0.0, 1.0, 0.5, 0.0, //
        0.0, 1.0, 1.0, 0.5, 0.5, //

        0.0, 0.0, 1.0, 0.0, 0.0, //
        1.0, 0.0, 1.0, 0.5, 0.0, //
        0.0, 1.0, 1.0, 0.0, 0.5, //
        1.0, 1.0, 1.0, 0.5, 0.5, //

        1.0, 0.0, 0.0, 0.0, 0.0, //
        1.0, 1.0, 0.0, 0.0, 0.5, //
        1.0, 0.0, 1.0, 0.5, 0.0, //
        1.0, 1.0, 1.0, 0.5, 0.5, //

        // bot texture 1/0
        0.0, 0.0, 0.0, 0.0, 0.0, //
        1.0, 0.0, 0.0, 0.5, 0.0, //
        0.0, 0.0, 1.0, 0.0, 0.5, //
        1.0, 0.0, 1.0, 0.5, 0.5, //
    };

    for (unsigned int i = 0; i < 24 * 5; i += 5)
    {
        vertices[i + 0] += x;
        vertices[i + 1] += y;
        vertices[i + 2] += z;
        vertices[i + 3] += static_cast<float>(texturePattern[i / (4 * 5)].first) / TILESET_WIDTH;
        vertices[i + 4] += static_cast<float>(texturePattern[i / (4 * 5)].second) / TILESET_HEIGHT;
    }

    unsigned int faces[] = {
        0,  1,  2, //
        1,  2,  3, //

        4,  5,  6, //
        5,  6,  7, //

        8,  9,  10, //
        9,  10, 11, //

        12, 13, 14, //
        13, 14, 15, //

        16, 17, 18, //
        17, 18, 19, //

        20, 21, 22, //
        21, 22, 23  //
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), &faces[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

unsigned int Block::getVAO() const
{
    return (VAO);
}