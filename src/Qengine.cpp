#include <chrono>
#include <iostream>

#include "Qstatus.h"
#include "scene/graphics/Viewport.h"

constexpr auto TITLE         = "Qengine";
constexpr int DEFAULT_WIDTH  = 1920;
constexpr int DEFAULT_HEIGHT = 1080;

int main() {
	std::cout << SPECIFIC_ERROR << std::endl;

    try {
        // Initialize and start Viewport
        Viewport(TITLE, DEFAULT_WIDTH, DEFAULT_HEIGHT, SceneManager()).start();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
