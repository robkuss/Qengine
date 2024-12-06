#pragma once

#define GLFW_INCLUDE_GLEXT

#include <array>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <math/vector/Vector3.h>
#include <math/ray/Ray.h>


// Options
#define TEXT	// For on-screen debug text
#ifdef TEXT
	#include "ui/text/Text.cpp"
#endif

class Vector2;
class SceneManager;	// Forward declaration for friend

// Constants
const auto CAMERA_POSITION_INIT			= Vector3(10.0f, 0, 0);		// Default camera position
const auto LOOK_AT_POINT_INIT			= Vector3(0, 0, 0);			// Default: Looking at origin
const auto UP_VECTOR_INIT				= Vector3(0, 0, 1);			// Default: Up direction is positive Z
const float CAMERA_DISTANCE_INIT		= CAMERA_POSITION_INIT.length();	// Camera distance from the origin

constexpr float CAMERA_DISTANCE_MIN		= 0.02f;
constexpr float CAMERA_DISTANCE_MAX		= 10000.0f;
constexpr float Z_NEAR					= CAMERA_DISTANCE_MIN / 2.0f;		// Near Clipping Plane: The distance from the camera to the near clipping plane. Objects closer than this distance are not rendered.
constexpr float Z_FAR					= CAMERA_DISTANCE_MAX * 2.0f;		//  Far Clipping Plane: The distance from the camera to the far clipping plane. Objects further than this distance are not rendered.
constexpr float FOV_Y					= 45.0f;							// Field of View in Y dimension
constexpr float ZOOM_SENSITIVITY		= 2.0f;

constexpr float AXES_LENGTH				= 100.0f;
constexpr float MOUSE_RAY_LENGTH		= 1000.0f;

constexpr int ANTIALIASING_SAMPLES		= 10;


class Viewport {
public:
	explicit Viewport(const std::string& title, int width, int height);
	~Viewport();

	void start();

	void setCallbacks(GLFWwindow* window);
	void onKeyboardInput(GLFWwindow* cbWindow, int key, int scancode, int action, int mods);
	void drawOnScreenText() const;

private:
	GLFWwindow* window;
	std::string title;
	int width, height;
	SceneManager* sceneManager{};

	#ifdef TEXT
		Text* text{};	// For on-screen debug text
	#endif

	// Initial values
	Vector3 camPos		= CAMERA_POSITION_INIT;
	Vector3 lookAt		= LOOK_AT_POINT_INIT;
	Vector3 up			= UP_VECTOR_INIT;
	float camDist		= CAMERA_DISTANCE_INIT;
	float zoomSpeed		= 1.0f;

	// Variables for camera rotation
	bool rotating		= false;
	double rotSens		= 0.5;
	double rotH			= 0.0;
	double rotV			= 0.0;
	double lastH		= 0.0;
	double lastV		= 0.0;

	// Mouse Ray
	Vector3 rayStart    = Vector3::MINUS_ONE;
	Vector3 rayEnd      = Vector3::ONE;

	// FPS tracking
	double previousTime = 0.0;
	int frameCount		= 0;
	int fps				= 0;

	// OpenGL Pointers
	GLint* viewport		= new GLint[4];
	std::array<GLfloat, 16> projMatrix;
	std::array<GLfloat, 16> viewMatrix;

	GLdouble* mouseX	= new double[1];
	GLdouble* mouseY	= new double[1];

	// Lighting
	GLfloat light1Pos[4] = {2, 3, 6, 0};
	GLfloat light2Pos[4] = {-2, -3, -6, 0};

	// Functions
	void render() const;

	void centerWindow() const;
	void windowResize(int newW, int newH);

	void initRotation(bool isRotating);
	void rotate(double mouseX, double mouseY);
	void zoom(double yoffset);
	void setPerspective(float h, float v);

	void gluPerspective(float aspect);
	void gluLookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
	void updateCameraPosition();

	[[nodiscard]] Ray getMouseRay(const Vector2& mousePos) const;
	void drawMouseRay() const;

	static void drawAxes();
	static void drawGrid();

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

	void getFPS();
};
