#pragma once

#define GLFW_INCLUDE_GLEXT

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <scene/SceneManager.h>

// Options
#define TEXT	// For on-screen debug text
#ifdef TEXT
	#include "ui/text/Text.cpp"
#endif


// Constants
const auto CAMERA_POSITION_INIT			= Vector3(10, 0, 0);			// Default camera position
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
	explicit Viewport(const std::string& title, int width, int height, const SceneManager& sceneManager);
	~Viewport();

	void setCallbacks(GLFWwindow* window);

	void start();
	void render() const;

	void centerWindow() const;
	void windowResize(int newW, int newH);

	void initRotation(bool isRotating);
	void rotate(double mouseX, double mouseY);
	void zoom(double yoffset);
	void setPerspective(float h, float v);

	void onKeyboardInput(GLFWwindow *cbWindow, int key, int scancode, int action, int mods);

private:
	GLFWwindow* window;
	std::string title;
	int width, height;
	float aspect;
	SceneManager sceneManager;

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
	GLint* viewport		= new int[4];
	GLfloat* projMatrix = new float[16];
	GLfloat* viewMatrix = new float[16];
	GLdouble* mouseX	= new double[1];
	GLdouble* mouseY	= new double[1];

	// Lighting
	GLfloat light1Pos[4] = {2, 3, 6, 0};
	GLfloat light2Pos[4] = {-2, -3, -6, 0};

	// Functions
	void gluPerspective() const;
	static void gluLookAt(const Vector3& eye, const Vector3& center, const Vector3& up) ;
	void updateCameraPosition();

	[[nodiscard]] Vector3 unproject(double mouseX, double mouseY, float depth) const;
	Ray getMouseRay(double mouseX, double mouseY);

	static void drawAxes();
	static void drawGrid();
	void drawMouseRay() const;
	void drawOnScreenText() const;

	static void setLight(const Color& diffuse, const Color& ambient, const Color& specular);

	void getFPS();
};
