#pragma once

#define GLFW_INCLUDE_GLEXT

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "math/vector/Vector3.h"
#include "math/ray/Ray.h"
#include "Camera.h"
#include "graphics/ui/UI.h"

#include <array>


// #define DRAW_MOUSE_RAY

class Vector2;
class SceneManager;	// Forward declaration for friend

// Constants
constexpr int ANTIALIASING_SAMPLES		= 25;

constexpr float AXES_LENGTH				= 100.0f;
constexpr float MOUSE_RAY_LENGTH		= 1000.0f;

// These really shouldn't be here
inline int  fps		= 0;

class Viewport {
public:
	Viewport(const std::string& title, int width, int height);
	~Viewport();

	void start();
	void render();

	void setCallbacks(GLFWwindow* window);
	void onKeyboardInput(GLFWwindow* cbWindow, int key, int scancode, int action, int mods) const;

private:
	std::shared_ptr<std::array<int, 4>> viewport;
	std::shared_ptr<Camera> activeCamera;

	GLFWwindow* window = nullptr;
	std::string title;
	int width, height;
	float aspect;

	// FPS tracking
	double previousTime = 0.0;
	int frameCount		= 0;

	// Mouse data
	//Vector2 lastMousePos	 = Vector2(0.0, 0.0);
	mutable Vector3 rayStart = Vector3::MINUS_ONE;
	mutable Vector3 rayEnd   = Vector3::ONE;
	std::shared_ptr<Ray> mouseRay;

	// Functions
	void centerWindow() const;
	void windowResize(int newW, int newH);

	void setMouseRay(const Vector2& mousePos) const;
	static void drawRay(const Vector3& rayStart, const Vector3& rayEnd);

	static void drawAxes();
	static void drawGrid();

	void getFPS();
};
