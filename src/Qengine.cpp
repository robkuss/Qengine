#ifndef QENGINE_C
#define QENGINE_C

#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

// Constants
constexpr int WIDTH_INIT  = 1920;
constexpr int HEIGHT_INIT = 1080;

constexpr int ANTIALIASING_SAMPLES = 10;

// Initial values
static GLFWwindow* window = nullptr;

int windowWidth    = WIDTH_INIT;
int windowHeight   = HEIGHT_INIT;
float aspect       = static_cast<float>(WIDTH_INIT) / static_cast<float>(HEIGHT_INIT);

int lastFPS        = 0;
int runTimer       = 0;
auto lastResetTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();    // Current time in ms


int main() {
    std::cout << "Hello World!" << std::endl;

    return 0;
}

#endif // QENGINE_C
