#define GLFW_INCLUDE_GLEXT

#include "Camera.h"

#include <math/Util.h>
#include <math/matrix/Matrix4.h>

#include <GLFW/glfw3.h>


Camera::Camera()
	: camPos(CAMERA_POSITION_INIT), lookAt(LOOK_AT_POINT_INIT), up(UP_VECTOR_INIT) {
}

Camera::Camera(const Vector3& camPos, const Vector3& lookAt, const Vector3& up)
	: camPos(camPos), lookAt(lookAt), up(up) {
}

Camera::~Camera() = default;


/**
 * Function to set up a perspective projection matrix, which is essential for rendering 3D scenes
 * in a way that simulates human vision, where objects further away appear smaller than those closer.
 */
void Camera::loadProjectionMatrix(const float aspect) {
	glMatrixMode(GL_PROJECTION);	// Subsequent matrix operations will affect the projection matrix
	glLoadIdentity();

	const auto fh = static_cast<float>(tan(radians(FOV_Y)) / 2 * Z_NEAR);	// Height of the Near Clipping Plane
	const auto fw = fh * aspect;											//  Width of the Near Clipping Plane
	constexpr float dz = Z_FAR - Z_NEAR;

	// Update the projection matrix
	projMatrix = {
		Z_NEAR / fw, 0.0f,		    0.0f,				  0.0f,
		0.0f,        Z_NEAR / fh,   0.0f,				  0.0f,
		0.0f,        0.0f,		  -(Z_FAR + Z_NEAR) / dz, -1.0f,
		0.0f,        0.0f,		  -(2.0f * Z_FAR * Z_NEAR) / dz,  0.0f
	};

	glMultMatrixf(projMatrix.data());
}

/**
 * Defines a viewing transformation by specifying an eye point, a reference point indicating.
 * the center of the scene, and an up vector.
 */
void Camera::loadViewMatrix() {
	glMatrixMode(GL_MODELVIEW);      // Subsequent matrix operations will affect the modelview matrix
	glLoadIdentity();

	const Vector3 forward = (lookAt - camPos).normalize();	// Calculate the forward vector (direction from eye to center)
	const Vector3 side = forward.cross(up).normalize();		// Calculate the side vector (perpendicular to both forward and up vectors)
	const Vector3 zUp = side.cross(forward);				// Recalculate the actual up vector to ensure orthogonality

	// Update the view matrix
	viewMatrix = {
		 side.x,			zUp.x,			-forward.x,			  0.0f,
		 side.y,			zUp.y,			-forward.y,			  0.0f,
		 side.z,			zUp.z,			-forward.z,			  0.0f,
		-side.dot(camPos), -zUp.dot(camPos), forward.dot(camPos), 1.0f
	};

	glMultMatrixf(viewMatrix.data());
}

void Camera::loadFixedViewMatrix() {
	glMatrixMode(GL_MODELVIEW); // Set the matrix mode to modelview
	glLoadIdentity();           // Reset the current modelview matrix

	// Calculate the forward vector (camera's local negative Z-axis)
	const Vector3 forward = (lookAt - camPos).normalize();

	// Calculate the side vector (perpendicular to the forward and up vectors)
	const Vector3 side = forward.cross(up).normalize();

	// Recalculate the actual up vector to ensure orthogonality
	const Vector3 zUp = side.cross(forward);

	// Create the fixed view matrix without applying camera position translation
	viewMatrix = {
		side.x,   zUp.x,   -forward.x,   0.0f,
		side.y,   zUp.y,   -forward.y,   0.0f,
		side.z,   zUp.z,   -forward.z,   0.0f,
		0.0f,     0.0f,    0.0f,         1.0f // No translation, fixed view
	};

	glMultMatrixf(viewMatrix.data());
}

/** Update camera position based on spherical coordinates. */
void Camera::updatePosition() {
	const double radH = radians(rotH);
	const double radV = radians(rotV);

	const auto sinH = static_cast<float>(sin(radH));
	const auto cosH = static_cast<float>(cos(radH));
	const auto sinV = static_cast<float>(sin(radV));
	const auto cosV = static_cast<float>(cos(radV));

	camPos = Vector3(
		camDist * cosV * cosH,
		camDist * cosV * -sinH,
		camDist * sinV
	);

	loadViewMatrix();
}

void Camera::initRotation(const bool isRotating, const double mouseX, const double mouseY) {
	rotating = isRotating;
	if (rotating) {
		lastH = mouseX;
		lastV = mouseY;
	}
}

void Camera::rotate(const double mouseX, const double mouseY) {
	const double dx = mouseX - lastH;
	const double dy = mouseY - lastV;
	rotH = fmod(rotH + dx * rotSens, 360.0);  // Adjust horizontal rotation
	rotV = fmod(rotV + dy * rotSens, 360.0);  // Adjust vertical rotation
	lastH = mouseX;
	lastV = mouseY;

	// Apply camera rotation
	updatePosition();
}

/** Handle the mouse scroll event to zoom in and out. */
void Camera::zoom(const double yoffset) {
	// Calculate the zoom speed scalar based on the current camera distance
	zoomSpeed = camDist / CAMERA_DISTANCE_INIT * ZOOM_SENSITIVITY;

	// Update the camera distance with the scroll input
	camDist -= static_cast<float>(yoffset) * zoomSpeed;
	camDist = clamp(camDist, CAMERA_DISTANCE_MIN, CAMERA_DISTANCE_MAX);

	// Apply zoom
	updatePosition();
}

void Camera::setPerspective(const float h, const float v) {
	rotH = h;
	rotV = v;

	// Apply camera rotation
	updatePosition();
}
