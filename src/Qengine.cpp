#ifndef QENGINE_C
#define QENGINE_C

#include <chrono>
#include <iostream>

#include "scene/graphics/Viewport.h"

int main() {
    try {
        // Initialize and start Viewport
        Viewport("Qengine", 1920, 1080, SceneManager()).start();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif // QENGINE_C
