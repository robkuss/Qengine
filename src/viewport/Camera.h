#pragma once

#include <math/ray/Ray.h>


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


class Camera {
public:
	Camera();
	Camera(const Vector3 &camPos, const Vector3 &lookAt, const Vector3 &up);
	~Camera();

	std::array<GLfloat, 16> viewMatrix{};
	std::array<GLfloat, 16> projMatrix{};

	Vector3 camPos		= CAMERA_POSITION_INIT;
	Vector3 lookAt		= LOOK_AT_POINT_INIT;
	Vector3 up			= UP_VECTOR_INIT;
	float camDist		= CAMERA_DISTANCE_INIT;

	bool rotating		= false;
	double rotH			= 0.0;
	double rotV			= 0.0;

	void loadProjectionMatrix(float aspect);
	void loadViewMatrix();
	void loadFixedViewMatrix();

	void updatePosition();
	void initRotation(bool isRotating, double mouseX, double mouseY);
	void rotate(double mouseX, double mouseY);
	void zoom(double yoffset);
	void setPerspective(float h, float v);

private:
	double rotSens		= 0.5;
	double lastH		= 0.0;
	double lastV		= 0.0;

	double mouseSpeed	= 0.05;
	float zoomSpeed		= 1.0f;
};
