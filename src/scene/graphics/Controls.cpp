#include "Viewport.h"

// Controls

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
void Viewport::setCallbacks(GLFWwindow* window) {
	// Window resize callback
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* cbWindow, const int width, const int height) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->windowResize(width, height);
	});

	// Mouse button callbacks
	glfwSetMouseButtonCallback(window, [](GLFWwindow* cbWindow, const int button, const int action, const int) {
		const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow));
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			vp->select();
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			vp->initRotation(action == GLFW_PRESS);
		}
	});

	// Cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* cbWindow, const double xPos, const double yPos) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->transform(xPos, yPos);
	});

	// Mouse scroll callback
	glfwSetScrollCallback(window, [](GLFWwindow* cbWindow, const double, const double yOffset) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->zoom(yOffset);
	});

	// Key callbacks
	glfwSetKeyCallback(window, [](GLFWwindow *cbWindow, const int key, const int scancode, const int action, int const mods) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->onKeyboardInput(cbWindow, key, scancode, action, mods);
	});

	// Set the user pointer to the Viewport instance
	glfwSetWindowUserPointer(window, this);
}

/** Key callbacks */
void Viewport::onKeyboardInput(GLFWwindow *cbWindow, const int key, const int scancode, const int action, const int mods) {
	if (action != GLFW_PRESS) return;
	switch (key) {
		case GLFW_KEY_TAB : toggleViewportMode(); break;						// TAB -> Toggle Object/Edit Mode

		// Number keys for perspective toggling
		case GLFW_KEY_1	  : togglePerspective(  0.0f,  0.0f); break;		// 1 -> Front View  (towards negative X)
		case GLFW_KEY_2   : togglePerspective(-90.0f,  0.0f); break;		// 2 -> Right View  (towards negative Y)
		case GLFW_KEY_3   : togglePerspective(  0.0f, 90.0f); break;		// 3 -> Top View    (towards negative Z)
		case GLFW_KEY_4   : togglePerspective(180.0f,  0.0f); break;		// 4 -> Back View   (towards positive X)
		case GLFW_KEY_5   : togglePerspective( 90.0f,  0.0f); break;		// 5 -> Left View   (towards positive Y)
		case GLFW_KEY_6   : togglePerspective(  0.0f,-90.0f); break;		// 6 -> Bottom View (towards positive Z)

		// Change Transform Mode
		case GLFW_KEY_G	  : changeTransformMode(Mode::GRAB); break;				// G -> Grab
		case GLFW_KEY_S	  : changeTransformMode(Mode::SCALE); break;			// S -> Scale
		case GLFW_KEY_R   : {													// R -> Rotate
			changeTransformMode(Mode::ROTATE);
			text->setErrorText("Rotating is not yet implemented."); break;
		}
		case GLFW_KEY_E	  : {													// E -> Extrude
			changeTransformMode(Mode::EXTRUDE);
			text->setErrorText("Extruding is not yet implemented."); break;
		}
		case GLFW_KEY_F	  : {													// F -> Fill
			changeTransformMode(Mode::FILL);
			text->setErrorText("Filling is not yet implemented."); break;
		}
		case GLFW_KEY_M   : {													// M -> Merge
			changeTransformMode(Mode::MERGE);
			text->setErrorText("Merging is not yet implemented."); break;
		}

		// Change Transform SubMode
		case GLFW_KEY_X   : if (transformMode.mode != Mode::NONE) changeTransformSubMode(SubMode::X); break; // X -> Snap transformation to X direction
		case GLFW_KEY_Y   : if (transformMode.mode != Mode::NONE) changeTransformSubMode(SubMode::Y); break; // Y -> Snap transformation to Y direction
		case GLFW_KEY_Z   : if (transformMode.mode != Mode::NONE) changeTransformSubMode(SubMode::Z); break; // Z -> Snap transformation to Z direction

		default: text->setErrorText("Invalid key.");
	}
}