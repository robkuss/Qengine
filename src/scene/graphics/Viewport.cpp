#include "Viewport.h"

#include "math/Util.h"
#include "math/vector/Vector2.h"
#include "color/Colors.h"

#include <cmath>
#include <sstream>
#include <iomanip>


Viewport::Viewport(const std::string &title, const int width, const int height, const SceneManager& sceneManager)
		: title(title), width(width), height(height), sceneManager(sceneManager) {
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

	// Set callbacks for keyboard and mouse input
	setCallbacks(window);

	// OpenGL setup
	glEnable(GL_DEPTH_TEST); // Enable depth testing
	glEnable(GL_MULTISAMPLE); // Enable multi-sampling (antialiasing)

	glfwSwapInterval(0); // Disable v-sync

	aspect = static_cast<float>(width) / static_cast<float>(height);
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
	// Init matrices
	gluPerspective();						  // Initialize projection matrix
	gluLookAt(camPos, lookAt, up);   // Initialize view matrix

	#ifdef TEXT
		text = new Text();	// Initialize FreeType for on-screen debug text
	#endif

	// Start rendering the Viewport
	while (!glfwWindowShouldClose(window)) {
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		getFPS();
	}
}

/** Viewport rendering loop called every frame */
void Viewport::render() {
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	clearColor(BG_COLOR);										// Background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the framebuffer

	// Draw the coordinate system
	drawAxes();
	drawGrid();

	// Render scene objects
	sceneManager.render(camPos);

	#ifdef DRAW_MOUSE_RAY
		drawMouseRay();
	#endif

	#ifdef TEXT
		drawOnScreenText();
		text->drawErrorText(width, height);
	#endif
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
	width = newW;
	height = newH;
	aspect = static_cast<float>(width) / static_cast<float>(height);

	// Update viewport
	glViewport(0, 0, width, height);
	gluPerspective();
	render();
}

/** Centers the application's window to the middle of the screen. */
void Viewport::centerWindow() const {
	const auto monitor = glfwGetPrimaryMonitor();
	const auto vidMode = glfwGetVideoMode(monitor);
	const int monitorWidth	 = vidMode->width;
	const int monitorHeight  = vidMode->height;
	const int windowPosX	 = (monitorWidth - width) / 2;
	const int windowPosY	 = (monitorHeight - height) / 2;
	glfwSetWindowPos(window, windowPosX, windowPosY);
}

void Viewport::drawOnScreenText() const {
	const auto cube = *sceneManager.sceneObjects[0];
	const auto mouseWorld = unproject(mouseX[0], mouseY[0], 0.0f);
	for (int i = 0; i <= 10; i++) {
		std::ostringstream out;
		switch (i) {
			case 0:  out << "FPS: " << fps; break;
			case 1:  out << "Camera Pos: " << std::fixed << std::setprecision(3) << camPos.x << " " << camPos.y << " " << camPos.z; break;
			case 2:  out << "Camera Rot: " << std::fixed << std::setprecision(1) << rotH << " / " << rotV; break;
			case 3:  out << "Zoom: " << std::fixed << std::setprecision(3) << camDist; break;
			case 4:  out << "Mouse Screen: " << mouseX[0]  << " / " << mouseY[0]; break;
			case 5:  out << "Mouse World: "  << std::fixed << std::setprecision(3) << mouseWorld.x << " " << mouseWorld.y << " " << mouseWorld.z; break;
			case 6:	 out << "Mode: " << modeToString(sceneManager.viewportMode.mode);
				if (sceneManager.transformMode.mode    != Mode::NONE)	out << " " << modeToString(sceneManager.transformMode.mode);
				if (sceneManager.transformMode.subMode != SubMode::NONE) out << " " << subModeToString(sceneManager.transformMode.subMode); break;
			case 7:  out << "Cube:"; break;
			case 8:  out << "    Pos: "   << std::fixed << std::setprecision(3) << cube.position.x  << " " << cube.position.y  << " " << cube.position.z;  break;
			case 9:  out << "    Scale: " << std::fixed << std::setprecision(3) << cube.scale.x     << " " << cube.scale.y     << " " << cube.scale.z;     break;
			default: out << "    Rot: "   << std::fixed << std::setprecision(3) << cube.rotation.x  << " " << cube.rotation.y  << " " << cube.rotation.z;  break;
		}
		text->renderText(out.str().c_str(), Text::firstLineX, Text::line(i), width, height, TEXT_COLOR);
	}
}

/**
 * Function to set up a perspective projection matrix, which is essential for rendering 3D scenes
 * in a way that simulates human vision, where objects further away appear smaller than those closer.
 */
void Viewport::gluPerspective() const {
	glMatrixMode(GL_PROJECTION);	// Subsequent matrix operations will affect the projection matrix

	const double fh = tan(FOV_Y * PI / 360.0) * Z_NEAR;	// Height of the Near Clipping Plane
	const double fw = fh * aspect;							//  Width of the Near Clipping Plane

	// Update the perspective projection matrix based on the calculated dimensions
	glLoadIdentity();
	glFrustum(-fw, fw, -fh, fh, Z_NEAR, Z_FAR);
}

/**
 * Defines a viewing transformation by specifying an eye point, a reference point indicating.
 * the center of the scene, and an up vector.
 *
 * @param eye    Coordinates of the eye point (camera position).
 * @param center Coordinates of the reference point (look-at point).
 * @param up     Coordinates of the up vector, which defines the upwards direction relative to the camera.
 *               It typically points upwards in the world coordinate system but can be adjusted to tilt the camera.
 */
void Viewport::gluLookAt(const Vector3 eye, const Vector3 center, const Vector3 up) {
	glMatrixMode(GL_MODELVIEW);      // Subsequent matrix operations will affect the modelview matrix

	const Vector3 forward = (center - eye).normalize();	 // Calculate the forward vector (direction from eye to center)
	const Vector3 side = forward.cross(up).normalize();	 // Calculate the side vector (perpendicular to both forward and up vectors)
	const Vector3 zUp = side.cross(forward);			 // Recalculate the actual up vector to ensure orthogonality

	// Construct the view matrix, which is used to transform coordinates from world space to camera space
	const GLfloat viewMatrix[] = {
		 side.x,		 zUp.x,		  -forward.x,		 0.0f,
		 side.y,		 zUp.y,		  -forward.y,		 0.0f,
		 side.z,		 zUp.z,		  -forward.z,		 0.0f,
		-side.dot(eye), -zUp.dot(eye), forward.dot(eye), 1.0f
	};

	// Update the view matrix, which sets up the camera's orientation and position
	glLoadIdentity();
	glMultMatrixf(viewMatrix);
}

/** Update camera position based on spherical coordinates. */
void Viewport::updateCameraPosition() {
	const double radH = rotH * PI / 180.0;
	const double radV = rotV * PI / 180.0;

	const auto sinH = static_cast<float>(sin(radH));
	const auto cosH = static_cast<float>(cos(radH));
	const auto sinV = static_cast<float>(sin(radV));
	const auto cosV = static_cast<float>(cos(radV));

	camPos = Vector3(
		camDist * cosV * cosH,
		camDist * cosV * -sinH,
		camDist * sinV
	);
}

void Viewport::initRotation(const bool isRotating) {
	rotating = isRotating;
	if (rotating) {
		lastH = mouseX[0];
		lastV = mouseY[0];
	}
}

void Viewport::rotate(const double mouseX, const double mouseY) {
	const double dx = mouseX - lastH;
	const double dy = mouseY - lastV;
	rotH = fmod(rotH + dx * rotSens, 360.0);  // Adjust horizontal rotation
	rotV = fmod(rotV + dy * rotSens, 360.0);  // Adjust vertical rotation
	lastH = mouseX;
	lastV = mouseY;

	// Apply camera rotation
	updateCameraPosition();
	gluLookAt(camPos, lookAt, up);
}

/** Handle the mouse scroll event to zoom in and out. */
void Viewport::zoom(const double yoffset) {
	// Calculate the zoom speed scalar based on the current camera distance
	zoomSpeed = camDist / CAMERA_DISTANCE_INIT * ZOOM_SENSITIVITY;

	// Update the camera distance with the scroll input
	camDist -= static_cast<float>(yoffset) * zoomSpeed;
	camDist = std::clamp(camDist, CAMERA_DISTANCE_MIN, CAMERA_DISTANCE_MAX);

	// Apply zoom
	updateCameraPosition();
	gluLookAt(camPos, lookAt, up);
}

void Viewport::togglePerspective(const float h, const float v) {
	rotH = h;
	rotV = v;
	updateCameraPosition();	 // Apply camera rotation
	gluLookAt(camPos, lookAt, up);
}

/**
 * This function maps screen space (2D mouse coordinates) to 3D world space.
 *
 * @param mouseX The x-coordinate of the mouse position in screen space.
 * @param mouseY The y-coordinate of the mouse position in screen space.
 * @param depth A Z-value in normalized device coordinates.
 *              Use 1.0f for a ray (far plane),
 *              and 0.0f for the near plane or exact position grabbing.
 *
 * @return the world space coordinates for the given mouse position as a Vector3
 */
Vector3 Viewport::unproject(const double mouseX, const double mouseY, const float depth) const {
	// Lambda function to convert GLfloat* to std::array<float, 16>
	auto toArray = [](const GLfloat* matrix) {
		std::array<float, 16> arr{};
		std::copy_n(matrix, 16, arr.begin());
		return arr;
	};

	// Get viewport, projection, and modelview matrices
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);

	// Convert mouse coordinates to normalized device coordinates (NDC)
	const auto x = static_cast<float>(2.0f * mouseX / viewport[2] - 1.0f);
	const auto y = static_cast<float>(1.0f - 2.0f * mouseY / viewport[3]);

	const auto viewSpace = Vector4(x, y, depth, 1.0f);											// Create a vector in clip space
	const auto clipSpace = Matrix4(toArray(projMatrix)).invert() * viewSpace;				// Transform from clip space to view space by applying the inverse of the projection matrix
	const auto unprojectedClipSpace = Vector4(clipSpace.x, clipSpace.y, -1.0f, 0.0f);			// Set the Z to -1 for proper unprojection and W to 0 for direction vector in the case of a ray
	const auto worldSpace = Matrix4(toArray(viewMatrix)).invert() * unprojectedClipSpace;  // Transform from clip space to world space by applying the inverse of the view matrix

	return {worldSpace.x, worldSpace.y, worldSpace.z};
}

Ray Viewport::getMouseRay(const double mouseX, const double mouseY) {
	return {camPos, unproject(mouseX, mouseY, 1.0f).normalize()};
}

// Drawing functions

/** Draw the coordinate system axes. */
void Viewport::drawAxes() {
	glBegin(GL_LINES);

	// X-axis in red
	color3f(RED);
	glVertex3f(-AXES_LENGTH, 0, 0);    // Start point
	glVertex3f( AXES_LENGTH, 0, 0);    // End point

	// Y-axis in green
	color3f(GREEN);
	glVertex3f(0, -AXES_LENGTH, 0);
	glVertex3f(0,  AXES_LENGTH, 0);

	// Z-axis in blue
	color3f(BLUE);
	glVertex3f(0, 0, -AXES_LENGTH);
	glVertex3f(0, 0,  AXES_LENGTH);

	glEnd();
}

/** Draw a grid in the xy-plane to visualize the coordinate system. */
void Viewport::drawGrid() {
	glBegin(GL_LINES);

	// Grid lines in the x direction (front-back lines)
	color3f(GRID_COLOR);
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

void Viewport::drawMouseRay() const {
	glPointSize(5);
	glBegin(GL_POINTS);
	color3f(RED);
	glVertex3f(rayStart.x, rayStart.y, rayStart.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
	glEnd();

	glBegin(GL_LINES);
	color3f(RAY_COLOR);
	glVertex3f(rayStart.x, rayStart.y, rayStart.z);
	glVertex3f(rayEnd.x, rayEnd.y, rayEnd.z);
	glEnd();
}
