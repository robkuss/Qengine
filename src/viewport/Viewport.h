#pragma once

#define GLFW_INCLUDE_GLEXT

#include <array>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <math/vector/Vector3.h>
#include <math/ray/Ray.h>

#include "scene/Camera.h"


// Options
#define TEXT	// For on-screen debug text
#ifdef TEXT
	#include "scene/graphics/ui/text/Text.cpp"
#endif

// #define DRAW_MOUSE_RAY

class Vector2;
class SceneManager;	// Forward declaration for friend

// Constants
constexpr int ANTIALIASING_SAMPLES		= 10;

constexpr float SELECT_TOLERANCE		= 20.0f;			// Tolerance in pixel distance for mouse picking
constexpr float AXES_LENGTH				= 100.0f;
constexpr float MOUSE_RAY_LENGTH		= 1000.0f;


class Viewport {
public:
	std::array<int, 4> viewport {};

	explicit Viewport(const std::string& title, int width, int height);
	~Viewport();

	void start();

	void setCallbacks(GLFWwindow* window);
	void onKeyboardInput(GLFWwindow* cbWindow, int key, int scancode, int action, int mods);
	void drawOnScreenText() const;

private:
	GLFWwindow* window = nullptr;
	std::string title;
	int width, height;
	float aspect;
	SceneManager* sceneManager{};

	#ifdef TEXT
		Text* text{};	// For on-screen debug text
	#endif

	// FPS tracking
	double previousTime = 0.0;
	int frameCount		= 0;
	int fps				= 0;

	// Lighting
	GLfloat light1Pos[4] = {2, 3, 6, 0};
	GLfloat light2Pos[4] = {-2, -3, -6, 0};

	// Mouse Ray
	mutable Vector3 rayStart = Vector3::MINUS_ONE;
	mutable Vector3 rayEnd   = Vector3::ONE;
	Ray mouseRay = Ray(rayStart, rayEnd);

	// Camera
	Camera activeCamera = Camera();

	// Functions
	void render();

	void centerWindow() const;
	void windowResize(int newW, int newH);

	void setMouseRay(const Vector2 &mousePos);
	static void drawRay(const Vector3 &rayStart, const Vector3 &rayEnd);

	static void drawAxes();
	static void drawGrid();

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

	void getFPS();
};
