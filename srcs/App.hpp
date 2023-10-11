#ifndef APP_HPP
#define APP_HPP

#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class App
{
    private:
        void initWindow();
        void initVulkan();
        void mainLoop();
        void cleanup();

        GLFWwindow *window;
    public:
        App();
        App(const App &instance);
        App &operator=(const App &instance);
        ~App();
        void run();
};

#endif