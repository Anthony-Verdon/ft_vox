#include "WindowManager.hpp"
#include "../../../libs/glm/glm/gtc/matrix_transform.hpp"
#include "../../globals.hpp"
#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"
#include "../Time/Time.hpp"
#include "../WorldClasses/WorldData/WorldData.hpp"
#include <GLFW/glfw3.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H  

void mouse_callback(GLFWwindow *window, double xPos, double yPos);

WindowManager::WindowManager()
{
    start();
    updateLoop();
}

WindowManager::~WindowManager()
{
}

void WindowManager::start()
{
    if (glfwInit() == GL_FALSE)
        throw(std::runtime_error("INIT_GLFW::INITIALIZATION_FAILED"));

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ft_vox", NULL, NULL);
    if (!window)
        throw(std::runtime_error("INIT_WINDOW::INITIALIZATION_FAILED"));
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw(std::runtime_error("INIT_OPENGL::INITIALIZATION_FAILED"));

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // cull face enabled make openGL draw only on one side
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        throw(std::runtime_error("INIT_FREETYPE::INITIALIZATION_FAILED"));

    FT_Face face;
    if (FT_New_Face(ft, "assets/fonts/arial.ttf", 0, &face))
        throw(std::runtime_error("INIT_FREETYPE::FONT_LOADING_FAILED"));  
    
    FT_Set_Pixel_Sizes(face, 0, 48); 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        t_ftCharacter character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        characters.insert(std::pair<char, t_ftCharacter>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
}

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color, std::map<char, t_ftCharacter> characters, unsigned int VAO, unsigned int VBO);

void WindowManager::updateLoop()
{
    WorldData world;
    // camera.setPosition({0, world.perlinNoise[0] * 256, 0});
    camera.setPosition({8, 64, 8});
    Texture grassTexture("assets/tileset.jpg");
    Shader shader("srcs/shaders/shader.vs", "srcs/shaders/shader.fs");
    
    Shader TextShader("srcs/shaders/TextShader/shader.vs", "srcs/shaders/TextShader/shader.fs");
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);   

    while (!glfwWindowShouldClose(window))
    {
        Time::updateTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* input processing */
        if (isKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        #ifdef DEBUG_MODE
        DebugWriteMap(world);
        DebugWritePlayerCoord();
        #endif
        updateWireframeMode();
        updateSpeed();
        int frontAxis = isKeyPressed(GLFW_KEY_W) - isKeyPressed(GLFW_KEY_S);
        int rightAxis = isKeyPressed(GLFW_KEY_D) - isKeyPressed(GLFW_KEY_A);
        int upAxis = isKeyPressed(GLFW_KEY_SPACE) - isKeyPressed(GLFW_KEY_LEFT_SHIFT);
        camera.updatePosition(frontAxis, rightAxis, upAxis);

        /* shader update */
        shader.use();
        glm::mat4 view = glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getFrontDirection(),
                                     camera.getUpDirection());
        shader.setMat4("view", view);
        glm::mat4 projection =
            glm::perspective(glm::radians(camera.getFOV()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        /* rendering */
        world.updateChunksLoad(camera.getPosition().x, camera.getPosition().z);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture.getID());
        for (size_t x = 0; x < RENDER_DISTANCE_2X; x++)
        {
            for (size_t y = 0; y < RENDER_DISTANCE_2X; y++)
            {
                const std::unique_ptr<ChunkRenderer> &chunk = world.getChunk(x, y);
                if (!chunk)
                    continue;
                glBindVertexArray(chunk->getVAO());
                glDrawElements(GL_TRIANGLES, chunk->getFaces().size(), GL_UNSIGNED_INT, 0);
            }
        }
        RenderText(TextShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), characters, VAO, VBO);
        RenderText(TextShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), characters, VAO, VBO);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool WindowManager::isKeyPressed(int key)
{
    return (glfwGetKey(window, key) == GLFW_PRESS);
}

void WindowManager::updateWireframeMode()
{
    static bool wireFrameMode = false;
    static bool keyEnable = true;

    if (isKeyPressed(GLFW_KEY_F1))
    {
        if (keyEnable == true)
        {
            wireFrameMode = !wireFrameMode;
            if (wireFrameMode)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void WindowManager::updateSpeed()
{

    if (isKeyPressed(GLFW_KEY_LEFT_CONTROL))
        camera.setSpeed(20);
    else
        camera.setSpeed(10);
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    const float sensitivity = 0.1f;

    static float lastX = xPos;
    static float lastY = yPos;

    float xOffset;
    float yOffset;
    Camera *camera = reinterpret_cast<Camera *>(glfwGetWindowUserPointer(window));
    xOffset = (xPos - lastX) * sensitivity;
    yOffset = (lastY - yPos) * sensitivity;
    lastX = xPos;
    lastY = yPos;
    camera->addToYaw(xOffset);
    camera->addToPitch(yOffset);
    if (camera->getPitch() > 89.0f)
        camera->setPitch(89.0f);
    else if (camera->getPitch() < -89.0f)
        camera->setPitch(-89.0f);
    camera->updateDirection();
}

#ifdef DEBUG_MODE
void WindowManager::DebugWriteMap(const WorldData &world)
{
    static bool keyEnable = true;
    if (isKeyPressed(GLFW_KEY_1))
    {
        if (keyEnable)
            world.DebugWriteMap();
        keyEnable = false;
    }
    else
        keyEnable = true;
}

void WindowManager::DebugWritePlayerCoord()
{
    static bool keyEnable = true;
    if (isKeyPressed(GLFW_KEY_2))
    {
        if (keyEnable)
            std::cout  << std::endl 
                << "x: " << camera.getPosition()[0] << " "
                << "y: " << camera.getPosition()[1] << " "
                << "z: " << camera.getPosition()[2] << " " 
                << std::endl << std::endl;
        keyEnable = false;
    }
    else
        keyEnable = true;
}
#endif

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color, std::map<char, t_ftCharacter> characters, unsigned int VAO, unsigned int VBO)
{
    // activate corresponding render state	
    s.use();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    s.setMat4("projection", projection);
    s.setMat4("projection", projection);
    s.setMat4("projection", projection);

    glUniform3f(glGetUniformLocation(s.getID(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        t_ftCharacter ch = characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}