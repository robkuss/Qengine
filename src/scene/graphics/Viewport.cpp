#ifndef VIEWPORT_C
#define VIEWPORT_C

#include <cmath>
#include <sstream>
#include <iomanip>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "../../Qengine.cpp"
#include "../SceneManager.cpp"
#include "../../Mode.h"
#include "../../ui/text/Text.cpp"
#include "../../math/ray/Ray.cpp"
#include "../../math/vector/Vector4.cpp"
#include "../../math/matrix/Matrix4x4.cpp"

// Constants
const auto CAMERA_POSITION_INIT			= Vector3(10, 0, 0);			// Default camera position
const float CAMERA_DISTANCE_INIT		= CAMERA_POSITION_INIT.length();	// Camera distance from the origin
const auto LOOK_AT_POINT_INIT			= Vector3(0, 0, 0);			// Default: Looking at origin
const auto UP_VECTOR_INIT				= Vector3(0, 0, 1);			// Default: Up direction is positive Z

constexpr float CAMERA_DISTANCE_MIN		= 0.02f;
constexpr float CAMERA_DISTANCE_MAX		= 10000.0f;
constexpr float Z_NEAR					= CAMERA_DISTANCE_MIN / 2;			// Near Clipping Plane: The distance from the camera to the near clipping plane. Objects closer than this distance are not rendered.
constexpr float Z_FAR					= CAMERA_DISTANCE_MAX * 2;			//  Far Clipping Plane: The distance from the camera to the far clipping plane. Objects further than this distance are not rendered.
constexpr float FOV_Y					= 45.0f;							// Field of View in Y dimension
constexpr float ZOOM_SENSITIVITY		= 2.0f;
constexpr float AXES_LENGTH				= 100.0f;
constexpr float MOUSE_RAY_LENGTH		= 1000.0f;


class Viewport {
public:
	explicit Viewport(const SceneManager& sceneManager);
	~Viewport() = default;

	void render();

	void windowResize(int width, int height);
	void select();
	void initRotation(bool isRotating);
	void transform(double mouseX, double mouseY);
	void zoom(double yoffset);
	void togglePerspective(float h, float v);
	void toggleViewportMode();
	void changeTransformMode(Mode::ModeEnum mode);
	void changeTransformSubMode(SubMode subMode);

	void onKeyboardInput(GLFWwindow *cbWindow, int key, int scancode, int action, int mods);

private:
	SceneManager sceneManager;

	// Mode
	Mode viewportMode		= OBJECT;
	Mode transformMode		= NONE;

	// Initial values
	Vector3 cameraPosition	= CAMERA_POSITION_INIT;
	Vector3 lookAt			= LOOK_AT_POINT_INIT;
	Vector3 up				= UP_VECTOR_INIT;
	float cameraDistance	= CAMERA_DISTANCE_INIT;
	float zoomSpeed			= 1.0f;
	float fontScale			= 0.5f;

	// Variables for camera rotation
	bool rotating			= false;
	double rotSensitivity	= 0.5;
	double rotH				= 0.0;
	double rotV				= 0.0;
	double lastH			= 0.0;
	double lastV			= 0.0;

	// Variables for object transformation
	Vector3 transformation	= Vector3::ZERO;
	Vector3 lastTransform	= transformation;

	// Mouse Ray
	Vector3 rayStart        = Vector3(-1, -1, -1);
	Vector3 rayEnd          = Vector3( 1,  1,  1);

	// "Pointers" - Shared resources for viewport state used mainly by the OpenGL library
	int* viewport			= new int[4];
	float* projectionMatrix = new float[16];
	float* viewMatrix		= new float[16];
	double* mouseX			= new double[1];
	double* mouseY			= new double[1];

	// Functions
	void gluPerspective();
	void gluLookAt(Vector3 eye, Vector3 center, Vector3 up);
	void updateCameraPosition();

	Vector3 screenToWorld(double mouseX, double mouseY, float depth);
	Ray getMouseRay(double mouseX, double mouseY);
	void handleSelection(double selectX, double selectY);

	void drawAxes();
	void drawGrid();
	void drawMouseRay();

	void drawOnScreenText();
};

Viewport::Viewport(const SceneManager& sceneManager) : sceneManager(sceneManager) {
	// OpenGL setup
	glEnable(GL_DEPTH_TEST);	// Enable depth testing
	glEnable(GL_MULTISAMPLE);	// Enable multi-sampling (antialiasing)

	gluPerspective();				// Initialize projection matrix
	gluLookAt(						// Initialize modelview matrix
		cameraPosition,
		lookAt,
		up
	);

	initFreeType();					// Initialize FreeType for on-screen text
}

/** Viewport rendering loop called every frame */
void Viewport::render() {
	clearColor(BG_COLOR);										// Background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the framebuffer

	// Draw the coordinate system
	drawAxes();
	drawGrid();

	// Render scene objects
	sceneManager.render(viewportMode, cameraPosition);

	// Draw the mouse ray
	drawMouseRay();

	drawOnScreenText();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Viewport::drawOnScreenText() {
	const auto cube = sceneManager.sceneObjects[0];
	const auto mouseWorld = screenToWorld(mouseX[0], mouseY[0], 0);
	for (int i = 0; i <= 11; i++) {
		std::ostringstream text;
		switch (i) {
			case 0:  text << "FPS: " << lastFPS; break;
			case 1:  text << "Camera Pos: " << std::fixed << std::setprecision(3) << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z; break;
			case 2:  text << "Camera Rot: " << std::fixed << std::setprecision(1) << rotH << " / " << rotV; break;
			case 3:  text << "Zoom: " << std::fixed << std::setprecision(3) << cameraDistance; break;
			case 4:  text << "Mouse Screen: " << mouseX[0]  << " / " << mouseY[0]; break;
			case 5:  text << "Mouse World: "  << std::fixed << std::setprecision(3) << mouseWorld.x << " " << mouseWorld.y << " " << mouseWorld.z; break;
			case 6:	 text << "Mode: " << modeToString(viewportMode.mode);
						  if (transformMode.mode    != Mode::NONE)	  text << " "  << modeToString(transformMode.mode);
						  if (transformMode.subMode != SubMode::NONE) text << " "  << subModeToString(transformMode.subMode); break;
			case 7:  text << "Transform: " << std::fixed << std::setprecision(3) << transformation.x << " " << transformation.y << " " << transformation.z; break;
			case 8:  text << "Cube:"; break;
			case 9:  text << "    Pos: "   << std::fixed << std::setprecision(3) << cube.position.x  << " " << cube.position.y  << " " << cube.position.z;  break;
			case 10: text << "    Scale: " << std::fixed << std::setprecision(3) << cube.scale.x     << " " << cube.scale.y     << " " << cube.scale.z;     break;
			default: text << "    Rot: "   << std::fixed << std::setprecision(3) << cube.rotation.x  << " " << cube.rotation.y  << " " << cube.rotation.z;  break;
		}
		renderText(text.str().c_str(), firstLineX, i, fontScale, windowWidth, windowHeight, TEXT_COLOR);
	}
}

/**
 * Function to set up a perspective projection matrix, which is essential for rendering 3D scenes
 * in a way that simulates human vision, where objects further away appear smaller than those closer.
 */
void Viewport::gluPerspective() {
	glMatrixMode(GL_PROJECTION);	// Subsequent matrix operations will affect the projection matrix

	const double fh = tan(FOV_Y * M_PI / 360) * Z_NEAR;	// Height of the Near Clipping Plane
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
		 side.x,		 zUp.x,		  -forward.x,		 0,
		 side.y,		 zUp.y,		  -forward.y,		 0,
		 side.z,		 zUp.z,		  -forward.z,		 0,
		-side.dot(eye), -zUp.dot(eye), forward.dot(eye), 1
	};

	// Update the view matrix, which sets up the camera's orientation and position
	glLoadIdentity();
	glMultMatrixf(viewMatrix);
}

/** Update camera position based on spherical coordinates. */
void Viewport::updateCameraPosition() {
	const double radH = rotH * M_PI / 180;
	const double radV = rotV * M_PI / 180;

	const auto sinH = static_cast<float>(sin(radH));
	const auto cosH = static_cast<float>(cos(radH));
	const auto sinV = static_cast<float>(sin(radV));
	const auto cosV = static_cast<float>(cos(radV));

	cameraPosition = Vector3(
		cameraDistance * cosV * cosH,
		cameraDistance * cosV * -sinH,
		cameraDistance * sinV
	);
}


// User input processing / Callback response

void Viewport::windowResize(const int width, const int height) {
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
	aspect = static_cast<float>(width) / static_cast<float>(height);
	gluPerspective();
	render();	// Update viewport
}

void Viewport::select() {
	handleSelection(mouseX[0], mouseY[0]);

	if (transformMode.mode != Mode::NONE) {
		transformation			= Vector3::ZERO;	// Reset transformation vector
		lastTransform			= Vector3::ZERO;
		transformMode.mode		= Mode::NONE;		// Go back to View Mode
		transformMode.subMode	= SubMode::NONE;
	}
}

void Viewport::initRotation(const bool isRotating) {
	rotating = isRotating;
	if (rotating) {
		lastH = mouseX[0];
		lastV = mouseY[0];
	}
}

void Viewport::transform(const double mouseX, const double mouseY) {
	glfwGetCursorPos(window, this->mouseX, this->mouseY);

	switch (transformMode.mode) {
		case Mode::NONE: {
			if (rotating) {
				const double dx = mouseX - lastH;
				const double dy = mouseY - lastV;
				rotH = fmod(rotH + dx * rotSensitivity, 360.0);  // Adjust horizontal rotation
				rotV = fmod(rotV + dy * rotSensitivity, 360.0);  // Adjust vertical rotation
				lastH = mouseX;
				lastV = mouseY;

				// Apply camera rotation
				updateCameraPosition();
				gluLookAt(cameraPosition, lookAt, up);
			}
			break;
		}
		case Mode::GRAB: {
			Vector3 worldPos = screenToWorld(mouseX, mouseY, 0);	// Ray from the mouse position

			// Truncate world position vector based on the selected direction (Transformation Sub Mode)
			Vector3 wpDirectional = Vector3::ZERO;
			switch (transformMode.subMode) {
				case SubMode::NONE: wpDirectional = worldPos; break;
				// TODO: Make the unidirectional translation also follow the cursor
				case SubMode::X: wpDirectional = Vector3(worldPos.x, 0, 0); break;
				case SubMode::Y: wpDirectional = Vector3(0, worldPos.y, 0); break;
				case SubMode::Z: wpDirectional = Vector3(0, 0, worldPos.z); break;
			}

			const float grabZ = (sceneManager.selectedObject.value().position - cameraPosition).length();	// Distance of the object from the camera
			lastTransform = lastTransform == Vector3::ZERO ? wpDirectional : lastTransform;					// Ensure last transformation is non-zero
			transformation = (wpDirectional - lastTransform) * grabZ;										// Calculate transformation vector
			dynamic_cast<Mesh*>(&sceneManager.selectedObject.value())->applyTransformation(transformMode.mode, transformation);
			lastTransform = wpDirectional;
			break;
		}
		case Mode::SCALE: {
			throw std::runtime_error("Scaling is not yet implemented.");
		}
		case Mode::ROTATE: {
			throw std::runtime_error("Rotating is not yet implemented.");
		}
		case Mode::EXTRUDE: {
			throw std::runtime_error("Extruding is not yet implemented.");
		}
		case Mode::FILL: {
			throw std::runtime_error("Filling is not yet implemented.");
		}
		case Mode::MERGE: {
			throw std::runtime_error("Merge is not yet implemented.");
		}
		default: throw std::runtime_error("Wrong transform mode.");
	}
}

/** Handle the mouse scroll event to zoom in and out. */
void Viewport::zoom(const double yoffset) {
	// Calculate the zoom speed scalar based on the current camera distance
	zoomSpeed = cameraDistance / CAMERA_DISTANCE_INIT * ZOOM_SENSITIVITY;

	// Update the camera distance with the scroll input
	cameraDistance -= static_cast<float>(yoffset) * zoomSpeed;
	cameraDistance = std::clamp(cameraDistance, CAMERA_DISTANCE_MIN, CAMERA_DISTANCE_MAX);

	// Apply zoom
	updateCameraPosition();
	gluLookAt(cameraPosition, lookAt, up);
}

void Viewport::togglePerspective(const float h, const float v) {
	rotH = h;
	rotV = v;
	updateCameraPosition();	 // Apply camera rotation
	gluLookAt(cameraPosition, lookAt, up);
}

void Viewport::toggleViewportMode() {
	switch (viewportMode.mode) {
		case Mode::OBJECT: viewportMode.mode = Mode::EDIT; break;
		case Mode::EDIT: viewportMode.mode = Mode::OBJECT; break;
		default: {}
	}
}

void Viewport::changeTransformMode(const Mode::ModeEnum mode) {
	if (!sceneManager.selectedObject) return;	// Don't change mode if no Object is selected
	transformMode.mode = mode;
	transformMode.subMode = SubMode::NONE;		// Reset direction
}

void Viewport::changeTransformSubMode(const SubMode subMode) {
	if (!sceneManager.selectedObject) return;	// Don't change mode if no Object is selected
	if (transformMode.type != ModeType::TRANSFORM) {
		transformMode.subMode = subMode;
	}
}


// Quick Maths

/**
 * This function maps screen space (2D mouse coordinates) to world space (3D).
 *
 * @param mouseX The x-coordinate of the mouse position in screen space.
 * @param mouseY The y-coordinate of the mouse position in screen space.
 * @param depth A Z-value in normalized device coordinates.
 *              Use 1f for a ray (far plane),
 *              and 0f for the near plane or exact position grabbing.
 *
 * @return the world space coordinates for the given mouse position as a Vector3
 */
Vector3 Viewport::screenToWorld(const double mouseX, const double mouseY, const float depth) {
	// Get viewport, projection, and modelview matrices
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);

	// Convert mouse coordinates to normalized device coordinates (NDC)
	const auto x = static_cast<float>(2 * mouseX / viewport[2] - 1);
	const auto y = static_cast<float>(1 - 2 * mouseY / viewport[3]);

	const auto viewSpace = Vector4(x, y, depth, 1);										// Create a vector in clip space
	const auto clipSpace = Matrix4x4(projectionMatrix).invert() * viewSpace;			// Transform from clip space to view space by applying the inverse of the projection matrix
	const auto unprojectedClipSpace = Vector4(clipSpace.x, clipSpace.y, -1, 0);			// Set the Z to -1 for proper unprojection and W to 0 for direction vector in the case of a ray
	const auto worldSpace = Matrix4x4(viewMatrix).invert() * unprojectedClipSpace;	// Transform from clip space to world space by applying the inverse of the view matrix

	return {worldSpace.x, worldSpace.y, worldSpace.z};
}

/**
 * Computes a [Ray] from the camera through the specified mouse coordinates in the window.
 *
 * This function converts the mouse screen coordinates to normalized device coordinates (NDC),
 * then transforms these coordinates into world space to generate a Ray. The Ray is defined by
 * its origin (the camera position) and its direction (the computed direction in world space).
 *
 * @param mouseX The x-coordinate of the mouse position in screen space.
 * @param mouseY The y-coordinate of the mouse position in screen space.
 *
 * @return A [Ray] object representing the origin and direction of the ray in world space.
 */
Ray Viewport::getMouseRay(const double mouseX, const double mouseY) {
	return Ray(cameraPosition, screenToWorld(mouseX, mouseY, 1).normalize());
}

/**
 * Handles the selection of [Object]s in the scene based on the current mouse position.
 *
 * This function retrieves the mouse Ray using the current mouse coordinates and checks for
 * intersections with any [Mesh] in the scene. If a Mesh is intersected, it is selected
 * by the [SceneManager]. Additionally, the Ray's origin and calculated endpoint are logged
 * for debugging purposes.
 *
 * @see getMouseRay for the ray computation logic.
 */
void Viewport::handleSelection(const double selectX, const double selectY) {
	const Ray ray = getMouseRay(selectX, selectY);

	std::vector<Mesh> intersectingObjects;
	intersectingObjects.reserve(sceneManager.sceneObjects.size());
	for (Object& obj : sceneManager.sceneObjects) {
		// Attempt to cast Object to Mesh using dynamic_cast
		if (std::optional meshOpt = dynamic_cast<const Mesh*>(&obj); meshOpt && ray.intersects(**meshOpt)) {
			intersectingObjects.push_back(**meshOpt);
		}
	}

	std::optional<Mesh> selectedObject;
	if (!intersectingObjects.empty()) {
		selectedObject = *std::ranges::min_element(intersectingObjects, [&ray](const Mesh& a, const Mesh& b) {
			return a.position.distance(ray.origin) < b.position.distance(ray.origin);
		});
	}
	sceneManager.selectObject(selectedObject);
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

/**
	 * Draw the [Ray] that's created when the user clicks anywhere in the Viewport
	 */
void Viewport::drawMouseRay() {
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


#endif // VIEWPORT_C
