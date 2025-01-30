#include <iostream>

#include "viewport/Viewport.h"

using namespace std;

constexpr auto TITLE         = "Qengine";
constexpr int DEFAULT_WIDTH  = 1920;
constexpr int DEFAULT_HEIGHT = 1080;


int main() {
    try {
        Viewport(TITLE, DEFAULT_WIDTH, DEFAULT_HEIGHT).start();
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
