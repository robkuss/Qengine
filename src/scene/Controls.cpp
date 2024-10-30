#ifndef CONTROLS_C
#define CONTROLS_C

#include "graphics/Viewport.cpp"
#include "../Qengine.cpp"			// for window

#include <GLFW/glfw3.h>

/**
 * Initialization function to set callbacks for events:
 *   - Window resizing
 *   - Mouse:
 *      - Left mouse button:
 *          - Select object
 *      - Middle mouse button:
 *          - Hold and drag: Rotate Viewport around the origin
 *          - Scroll: Zoom in and out of the Viewport
 *   - Keys:
 *      - Number keys:
 *          - Toggle perspective
 *      - TAB
 *          - Toggle Object/Edit Mode
 *      - Object transformation:
 *          - G: Grab
 *          - S: Scale
 *          - R: Rotate
 *          - E: Extrude
 *          - F: Fill
 *          - M: Merge
 */
void setCallbacks(Viewport* viewport) {
	// Window resize callback
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* cbWindow, const int width, const int height) {
		if (auto* vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->windowResize(width, height);
	});

	// Mouse button callbacks
	glfwSetMouseButtonCallback(window, [](GLFWwindow* cbWindow, const int button, const int action, const int) {
		auto* vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow));
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			vp->select();
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			vp->initRotation(action == GLFW_PRESS);
		}
	});

	// Cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* cbWindow, const double xPos, const double yPos) {
		if (auto* vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->transform(xPos, yPos);
	});

	// Mouse scroll callback
	glfwSetScrollCallback(window, [](GLFWwindow* cbWindow, const double, const double yOffset) {
		if (auto* vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->zoom(yOffset);
	});

	// Key callbacks
	glfwSetKeyCallback(window, [](GLFWwindow *cbWindow, const int key, const int scancode, const int action, int const mods) {
		if (auto* vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->onKeyboardInput(cbWindow, key, scancode, action, mods);
	});

	// Set the user pointer to the Viewport instance
	glfwSetWindowUserPointer(window, &viewport);
}

/** Key callbacks */
void Viewport::onKeyboardInput(GLFWwindow *cbWindow, const int key, const int scancode, const int action, const int mods) {
	if (action != GLFW_PRESS) return;
	switch (key) {
		case GLFW_KEY_TAB : toggleViewportMode();					// TAB -> Toggle Object/Edit Mode

		// Number keys for perspective toggling
		case GLFW_KEY_1	  : togglePerspective(  0,  0);			// 1 -> Front View  (towards negative X)
		case GLFW_KEY_2   : togglePerspective(-90,  0);			// 2 -> Right View  (towards negative Y)
		case GLFW_KEY_3   : togglePerspective(  0, 90);			// 3 -> Top View    (towards negative Z)
		case GLFW_KEY_4   : togglePerspective(180,  0);			// 4 -> Back View   (towards positive X)
		case GLFW_KEY_5   : togglePerspective( 90,  0);			// 5 -> Left View   (towards positive Y)
		case GLFW_KEY_6   : togglePerspective(  0,-90);			// 6 -> Bottom View (towards positive Z)

		// Change Transform Mode
		case GLFW_KEY_G	  : changeTransformMode(Mode::GRAB);		// G -> Grab
		case GLFW_KEY_S	  : changeTransformMode(Mode::SCALE);		// S -> Scale
		case GLFW_KEY_R   : changeTransformMode(Mode::ROTATE);  	// R -> Rotate
		case GLFW_KEY_E	  : changeTransformMode(Mode::EXTRUDE);		// E -> Extrude
		case GLFW_KEY_F	  : changeTransformMode(Mode::FILL);		// F -> Fill
		case GLFW_KEY_M   : changeTransformMode(Mode::MERGE);		// M -> Merge

		// Change Transform SubMode
		case GLFW_KEY_X   : changeTransformSubMode(SubMode::X);		// X -> Snap transformation to X direction
		case GLFW_KEY_Y   : changeTransformSubMode(SubMode::Y);		// Y -> Snap transformation to Y direction
		case GLFW_KEY_Z   : changeTransformSubMode(SubMode::Z);		// Z -> Snap transformation to Z direction

		default: throw std::invalid_argument("Invalid key");
	}
}

#endif // CONTROLS_C