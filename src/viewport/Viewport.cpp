#include "Viewport.h"

#include <cmath>
#include <sstream>
#include <iomanip>

#include <viewport/scene/Scene.h>
#include <viewport/scene/graphics/color/Colors.h>
#include <math/vector/Vector2.h>

#include <objects/Object.h>
#include <objects/mesh/cube/Cube.cpp>
#include <objects/mesh/sphere/Sphere.cpp>
#include <objects/mesh/skybox/Skybox.cpp>

#include "material/texture/Texture.h"


Viewport::Viewport(const std::string& title, const int width, const int height)
		: title(title), width(width), height(height), scene(new Scene()) {
	glfwSetErrorCallback([](int, const char *description) {
		std::cerr << "GLFW Error: " << description << std::endl;
	});

	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, ANTIALIASING_SAMPLES);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Failed to open window");
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
	glEnable(GL_MULTISAMPLE); // Enable multi-sampling (antialiasing)
	glEnable(GL_DEPTH_TEST);  // Enable depth testing
	glEnable(GL_RESCALE_NORMAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	constexpr float noLight[4] = {0.0, 0.0, 0.0, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, noLight);

	// Other setup
	#ifdef TEXT
		text = new Text();	// Initialize FreeType for on-screen debug text
	#endif

	aspect = static_cast<float>(width) / static_cast<float>(height);

	scene->context->viewport = &viewport;
	scene->context->activeCamera = &activeCamera;
}

Viewport::~Viewport() {
	// Cleanup
	#ifdef TEXT
		text->~Text();
	#endif

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Viewport::start() {
	auto noTexture	  = std::shared_ptr<Texture>{};
	auto thmTexture   = std::make_shared<Texture>("../resources/textures/thm2k.png");
	auto earthTexture = std::make_shared<Texture>("../resources/textures/earth_diffuse.jpg");
	auto starsTexture = std::make_shared<Texture>("../resources/textures/cubemap8k.jpg");

	// Add Default Cube to Scene
	const auto cube = std::make_shared<Cube>(
		"Cube",
		Vector3(0.0f, 1.0f, 0.0f),
		1.0f,
		Colors::WHITE,
		thmTexture
	);
	scene->addObject(cube);

	// Add Earth to Scene
	const auto earth = std::make_shared<Sphere>(
		"Earth",
		Vector3(0.0f, -1.0f, 0.0f),
		0.5f,
		64,
		32,
		Colors::WHITE,
		earthTexture
	);
	scene->addObject(earth);

	const auto skybox = std::make_shared<Skybox>(
		"Skybox",
		Colors::WHITE,
		starsTexture
	);
	skybox->applyTransformation(OBJECT, SCALE, Matrix4::scale(Vector3(5.0f, 5.0f, 5.0f)));
	//sceneManager->addObject(skybox);


	clearColor(Colors::BG_COLOR);	// Background color
	Scene::setLight(Colors::LIGHT_SUN, Colors::LIGHT_AMBIENT, Colors::WHITE);

	// Get matrices
	activeCamera.gluPerspective(aspect); // projection matrix
	activeCamera.gluLookAt();			 // view matrix

	// Start rendering the Viewport
	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &width, &height);

		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

/** Viewport rendering loop called every frame */
void Viewport::render() {
	glViewport(0, 0, width, height);
	glGetIntegerv(GL_VIEWPORT, viewport.data());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the coordinate system
	drawAxes();
	drawGrid();

	// Render scene objects
	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightfv(GL_LIGHT2, GL_POSITION, light2Pos);

	scene->render();

	#ifdef DRAW_MOUSE_RAY
		drawRay(rayStart, rayEnd);
	#endif

	// Render UI
	#ifdef TEXT
		drawOnScreenText();
		text->drawErrorText(width, height);
	#endif

	getFPS();
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
	activeCamera.gluPerspective(aspect);
}

/** Centers the application's window to the middle of the screen. */
void Viewport::centerWindow() const {
	const auto monitor = glfwGetPrimaryMonitor();
	const auto vidMode = glfwGetVideoMode(monitor);
	const int windowPosX	 = (vidMode->width - width) / 2;
	const int windowPosY	 = (vidMode->height - height) / 2;
	glfwSetWindowPos(window, windowPosX, windowPosY);
}

void Viewport::drawOnScreenText() const {
	const auto cube = *scene->sceneObjects[0];
	const auto mouseWorld = unproject(Vector2(*activeCamera.mouseX, *activeCamera.mouseY), &viewport, activeCamera.viewMatrix, activeCamera.projMatrix);
	size_t vertexCount = 0;
	for (const auto& obj : scene->sceneObjects) {
		vertexCount += dynamic_cast<Mesh*>(obj.get())->vertices.size();
	}
	for (int i = 0; i <= 11; i++) {
		std::ostringstream out;
		switch (i) {
			case 0:  out << "FPS: " << fps; break;
			case 1:  out << "Camera Pos: " << activeCamera.camPos.toString(); break;
			case 2:  out << "Camera Rot: " << std::fixed << std::setprecision(1) << activeCamera.rotH << " / " << activeCamera.rotV; break;
			case 3:  out << "Zoom: " << std::fixed << std::setprecision(3) << activeCamera.camDist; break;
			case 4:  out << "Mouse Screen: " << activeCamera.mouseX[0]  << " / " << activeCamera.mouseY[0]; break;
			case 5:  out << "Mouse World: "  << mouseWorld.toString(); break;
			case 6:	 out << "Mode: " << scene->selectionMode.modeToString();
				if (scene->transformMode.mode    != Mode::NONE)	out << " " << scene->transformMode.modeToString();
				if (scene->transformMode.subMode != SubMode::NONE) out << " " << scene->transformMode.subModeToString(); break;
			case 7:  out << "Cube:"; break;
			case 8:  out << "    Pos: "   << cube.position.toString();  break;
			case 9:  out << "    Scale: " << cube.scale.toString();     break;
			case 10: out << "    Rot: "   << cube.rotationEuler.toString(); break;
			default: out << "Vertex Count: " << vertexCount; break;
		}
		#ifdef TEXT
			text->renderText(out.str().c_str(), Text::firstLineX, Text::line(i), width, height, Colors::TEXT_COLOR);
		#endif
	}
}

void Viewport::setMouseRay(const Vector2& mousePos) {
	mouseRay.direction = unproject(mousePos, &viewport, activeCamera.viewMatrix, activeCamera.projMatrix).normalize();
	const auto directionScaled = mouseRay.direction * MOUSE_RAY_LENGTH;
	rayStart = mouseRay.origin = activeCamera.camPos;
	rayEnd   = mouseRay.origin + directionScaled;
}


// Drawing functions

/** Draw the coordinate system axes. */
void Viewport::drawAxes() {
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

	glBegin(GL_LINES);
	color3f(Colors::RAY_COLOR);
	glVertex3f(rayStart.x, rayStart.y, rayStart.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
	glEnd();
}
