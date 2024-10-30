#ifndef QENGINE_C
#define QENGINE_C

#include <chrono>
#include <iostream>

#include "scene/graphics/Viewport.cpp"
#include "scene/graphics/MeshRenderer.cpp"
#include "scene/SceneManager.cpp"


int lastFPS         = 0;
int runTimer        = 0;
auto lastResetTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();    // Current time in ms


int main() {
    try {
        // Initialize Mesh Renderer, Scene Manager and Viewport
        constexpr auto meshRenderer = MeshRenderer();
        const auto sceneManager     = SceneManager(meshRenderer);
        auto viewport               = Viewport("Qengine", 1920, 1080, sceneManager);
        viewport.start();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif // QENGINE_C
