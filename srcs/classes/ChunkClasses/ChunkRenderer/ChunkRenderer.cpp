#include "ChunkRenderer.hpp"
#include <cstring>
#include <glad/glad.h>
#include <iostream>
ChunkRenderer::ChunkRenderer(const ChunkMesh &chunkMesh) : ChunkMesh(chunkMesh)
{
    glGenVertexArrays(MeshType::MESH_TYPE_COUNT, VAO);
    glGenBuffers(MeshType::MESH_TYPE_COUNT, VBO);
    glGenBuffers(MeshType::MESH_TYPE_COUNT, EBO);
    initRenderer();
}

/*
ChunkRenderer::ChunkRenderer(const ChunkRenderer &instance) : ChunkMesh(instance)
{
    *this = instance;
}

ChunkRenderer &ChunkRenderer::operator=(const ChunkRenderer &instance)
{
    if (this != &instance)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        initRenderer();
        // @todo: copy
    }
    return (*this);
}
*/
ChunkRenderer::~ChunkRenderer()
{
    glDeleteVertexArrays(MeshType::MESH_TYPE_COUNT, VAO);
    glDeleteBuffers(MeshType::MESH_TYPE_COUNT, VBO);
    glDeleteBuffers(MeshType::MESH_TYPE_COUNT, EBO);
}

void ChunkRenderer::initRenderer()
{
    for (int i = 0; i < MeshType::MESH_TYPE_COUNT; i++)
    {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices[i].size(), vertices[i].data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces[i].size(), faces[i].data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
}

unsigned int ChunkRenderer::getVAO(MeshType type) const
{
    return (VAO[type]);
}

void ChunkRenderer::updateRenderer()
{
    for (int i = 0; i < MeshType::MESH_TYPE_COUNT; i++)
    {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices[i].size(), vertices[i].data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces[i].size(), faces[i].data(), GL_DYNAMIC_DRAW);
    }
}
