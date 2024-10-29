#include "LineRenderer.hpp"

LineRenderer::LineRenderer(const LineData &instance) : LineData(instance)
{
    std::vector<float> vertices = {
        start.x, start.y, start.z, end.x, end.y, end.z, // create a LineMesh just for this ?
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LineRenderer::draw() const
{
    std::vector<float> vertices = {
        start.x, start.y, start.z, end.x, end.y, end.z,
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
}

LineRenderer::~LineRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}