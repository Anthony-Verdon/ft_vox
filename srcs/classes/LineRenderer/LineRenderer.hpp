#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Line
{
    int shaderProgram;
    unsigned int VBO, VAO;
    std::vector<float> vertices;
    glm::vec3 startPoint;
    glm::vec3 endPoint;
    glm::mat4 MVP = glm::mat4(1.0);

  public:
    glm::vec3 lineColor;
    Line(glm::vec3 start, glm::vec3 end)
    {

        startPoint = start;
        endPoint = end;
        lineColor = glm::vec3(1, 1, 1);

        vertices = {
            start.x, start.y, start.z, end.x, end.y, end.z,

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

    int setMVP(glm::mat4 mvp)
    {
        MVP = mvp;
        return 1;
    }

    int setColor(glm::vec3 color)
    {
        lineColor = color;
        return 1;
    }

    int draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        return 1;
    }

    ~Line()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
};