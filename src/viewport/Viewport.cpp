#include "Viewport.h"

#include <array>
#include <iostream>
#include <ranges>

#include "objects/mesh/cube/Cube.cpp"
#include "objects/mesh/sphere/Sphere.cpp"
#include "objects/mesh/skybox/Skybox.cpp"

#include "graphics/material/texture/Texture.h"
#include "graphics/ui/UI.h"

#include "scene/Scene.h"
#include "scene/SceneManager.h"


Viewport::Viewport(const string& title, const int width, const int height)
	: title(title), width(width), height(height) {
	glfwSetErrorCallback([](int, const char *description) {
		cerr << "GLFW Error: " << description << endl;
	});

	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, ANTIALIASING_SAMPLES);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		throw runtime_error("Failed to open window");
	}
	centerWindow();

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwShowWindow(window);

	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	glfwSwapInterval(0); // Disable v-sync

	// Set callbacks for keyboard and mouse input
	setCallbacks(window);

	// OpenGL setup
	glEnable(GL_MULTISAMPLE);	// Enable multi-sampling (antialiasing)
	glEnable(GL_DEPTH_TEST);	// Enable depth testing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_RESCALE_NORMAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	constexpr float noLight[4] = {0.0, 0.0, 0.0, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	// Other setup
	viewport	 = make_shared<array<int, 4>>();
	activeCamera = make_shared<Camera>();
	mouseRay	 = make_shared<Ray>(rayStart, rayEnd);

	glViewport(0, 0, width, height);	// Initialize viewport
	glGetIntegerv(GL_VIEWPORT, viewport->data());

	aspect = static_cast<float>(width) / static_cast<float>(height);
}

Viewport::~Viewport() {
	// Cleanup
	glfwDestroyWindow(window);
	glfwTerminate();

	SceneManager::cleanupScenes();
}

void Viewport::start() {
	// Initialize Viewport pointers for SceneManager
	SceneManager::viewport		= viewport;
	SceneManager::activeCamera	= activeCamera;
	SceneManager::mouseRay		= mouseRay;

	// Create Scenes
	const auto foreground = make_shared<Scene>("Foreground");
	const auto background = make_shared<Scene>("Background");

	// Add Scenes to the SceneManager
	SceneManager::addScene(background);
	SceneManager::addScene(foreground);

	// Load Textures
	const auto noTexture	= shared_ptr<Texture>{};
	const auto thmTexture	= make_shared<Texture>("../resources/textures/thm2k.png");
	const auto earthTexture = make_shared<Texture>("../resources/textures/earth_diffuse.jpg");
	const auto starsTexture = make_shared<Texture>("../resources/textures/cubemap8k.jpg");

	// Add Default Cube to Scene
	const auto cube = make_shared<Cube>(
		"THM Cube",
		Vector3(0.0f, 1.5f, 0.0f),
		1.0f,
		Colors::WHITE,
		thmTexture
	);
	foreground->addObject(cube);

	// Add Earth to Scene
	const auto earth = make_shared<Sphere>(
		"Earth",
		Vector3(0.0f, -2.0f, 0.0f),
		1.0f,
		64,
		32,
		Colors::WHITE,
		earthTexture
	);
	foreground->addObject(earth);

	const auto skybox = make_shared<Skybox>(
		"Skybox",
		Colors::WHITE,
		starsTexture
	);
	background->addObject(skybox);

	background->enableDepthIsolation();
	background->enableFixedPosition();


	// Set up UI afterwards
	UI::setup(&SceneManager::viewport->at(2), &SceneManager::viewport->at(3));

	// Set up graphics
	clearColor(Colors::BG_COLOR);	// Background color

	// Initialize lighting
	array lightPos = {2.0f, 3.0f, 6.0f, 0.0f};
	foreground->addLight(
		make_shared<Light>("Sun", GL_LIGHT1, lightPos),
		Colors::LIGHT_SUN,
		Colors::LIGHT_AMBIENT,
		Colors::WHITE
	);

	background->addLight(
		make_shared<Light>("Sky Light", GL_LIGHT1, lightPos),
		Colors::BLACK,
		Colors::SKY,
		Colors::BLACK
	);

	// Get matrices
	activeCamera->loadProjectionMatrix(aspect);
	activeCamera->loadViewMatrix();


	// Start rendering the Viewport
	while (!glfwWindowShouldClose(window)) {
		render();

		getFPS();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Viewport::render() {
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT, viewport->data());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render Scenes
	SceneManager::renderScenes();

	// Draw the coordinate system
	if (drawCoordinateSystem) {
		drawAxes();
		drawGrid();
	}

	#ifdef DRAW_MOUSE_RAY
		drawRay(rayStart, rayEnd);
	#endif

	// Render UI last
	UI::render();
}


void Viewport::getFPS() {
	if (const double currentTime = glfwGetTime(); currentTime - previousTime >= 1.0) {
		fps = frameCount;
		frameCount = 0;
		previousTime = currentTime;
	}
	frameCount++;
}

void Viewport::windowResize(const int newW, const int newH) {
	glViewport(0, 0, width = newW, height = newH);
	aspect = static_cast<float>(width) / static_cast<float>(height);
	activeCamera->loadProjectionMatrix(aspect);
}

/** Centers the application's window to the middle of the screen. */
void Viewport::centerWindow() const {
	const auto monitor = glfwGetPrimaryMonitor();
	const auto vidMode = glfwGetVideoMode(monitor);
	const int windowPosX	 = (vidMode->width - width) / 2;
	const int windowPosY	 = (vidMode->height - height) / 2;
	glfwSetWindowPos(window, windowPosX, windowPosY);
}

void Viewport::setMouseRay(const Vector2& mousePos) const {
	mouseRay->direction = unproject(mousePos, viewport.get(), activeCamera->viewMatrix, activeCamera->projMatrix).normalize();
	const auto directionScaled = mouseRay->direction * MOUSE_RAY_LENGTH;
	rayStart = mouseRay->origin = activeCamera->camPos;
	rayEnd   = mouseRay->origin + directionScaled;
}


// Drawing functions

/** Draw the coordinate system axes. */
void Viewport::drawAxes() {
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	// X-axis in red
	color3f(Colors::RED);
	glVertex3f(-AXES_LENGTH, 0, 0);    // Start point
	glVertex3f( AXES_LENGTH, 0, 0);    // End point

	// Y-axis in green
	color3f(Colors::GREEN);
	glVertex3f(0, -AXES_LENGTH, 0);
	glVertex3f(0,  AXES_LENGTH, 0);

	// Z-axis in blue
	color3f(Colors::BLUE);
	glVertex3f(0, 0, -AXES_LENGTH);
	glVertex3f(0, 0,  AXES_LENGTH);

	glEnd();
}

/** Draw a grid in the xy-plane to visualize the coordinate system. */
void Viewport::drawGrid() {
	glLineWidth(1.0f);
	glBegin(GL_LINES);

	// Grid lines in the x direction (front-back lines)
	color3f(Colors::GRID_COLOR);
	for (int x = static_cast<int>(-AXES_LENGTH); x <= static_cast<int>(AXES_LENGTH); x++) {
		// Horizontal line at y = constant, spanning x-axis
		glVertex3f(static_cast<float>(x), -AXES_LENGTH, 0);
		glVertex3f(static_cast<float>(x),  AXES_LENGTH, 0);
	}

	// Grid lines in the y direction (left-right lines)
	for (int y = static_cast<int>(-AXES_LENGTH); y <= static_cast<int>(AXES_LENGTH); y++) {
		// Vertical line at x = constant, spanning y-axis
		glVertex3f(-AXES_LENGTH, static_cast<float>(y), 0);
		glVertex3f( AXES_LENGTH, static_cast<float>(y), 0);
	}

	glEnd();
}

void Viewport::drawRay(const Vector3& rayStart, const Vector3& rayEnd) {
	glPointSize(5);
	glBegin(GL_POINTS);
	color3f(Colors::RED);
	glVertex3f(rayStart.x, rayStart.y, rayStart.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	color3f(Colors::RAY_COLOR);
	glVertex3f(rayStart.x, rayStart.y, rayStart.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
	glEnd();
}
