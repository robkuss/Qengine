#ifndef QENGINE_C
#define QENGINE_C

#include <chrono>
#include <iostream>

#include "scene/graphics/Viewport.cpp"

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
