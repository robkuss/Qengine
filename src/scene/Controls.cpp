#include "graphics/Viewport.h"

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
 *      - Mesh operations:
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
			const Ray ray = vp->getMouseRay(vp->mouseX[0], vp->mouseY[0]);
			vp->sceneManager.select(ray);
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			vp->initRotation(action == GLFW_PRESS);
		}
	});

	// Cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* cbWindow, const double mouseX, const double mouseY) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow))) {
			glfwGetCursorPos(cbWindow, vp->mouseX, vp->mouseY); // Update the mouse position in the Viewport

			if (vp->sceneManager.transformMode.mode == Mode::NONE) {
				if (vp->rotating) {
					vp->rotate(mouseX, mouseY); // Regular rotation handling if no transformation mode is active
				}
			} else {
				vp->sceneManager.transform(mouseX, mouseY, vp->width, vp->height, vp->screenToWorld(mouseX, mouseY, 0), vp->camPos);
			}
		}
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
		case GLFW_KEY_TAB : sceneManager.toggleViewportMode(); break;			// TAB -> Toggle Object/Edit Mode

		// Number keys for perspective toggling
		case GLFW_KEY_1 : togglePerspective(  0.0f,  0.0f); break;			// 1 -> Front View  (towards negative X)
		case GLFW_KEY_2 : togglePerspective(-90.0f,  0.0f); break;			// 2 -> Right View  (towards negative Y)
		case GLFW_KEY_3 : togglePerspective(  0.0f, 90.0f); break;			// 3 -> Top View    (towards negative Z)
		case GLFW_KEY_4 : togglePerspective(180.0f,  0.0f); break;			// 4 -> Back View   (towards positive X)
		case GLFW_KEY_5 : togglePerspective( 90.0f,  0.0f); break;			// 5 -> Left View   (towards positive Y)
		case GLFW_KEY_6 : togglePerspective(  0.0f,-90.0f); break;			// 6 -> Bottom View (towards positive Z)

		// Set Transform Mode
		case GLFW_KEY_G : sceneManager.setTransformMode(Mode::GRAB); break;		// G -> Grab
		case GLFW_KEY_S : sceneManager.setTransformMode(Mode::SCALE); break;	// S -> Scale
		case GLFW_KEY_R : sceneManager.setTransformMode(Mode::ROTATE); break; 	// R -> Rotate
		case GLFW_KEY_E : {														// E -> Extrude
			sceneManager.setTransformMode(Mode::EXTRUDE);
			text->setErrorText("Extruding is not yet implemented."); break;
		}
		case GLFW_KEY_F : {														// F -> Fill
			sceneManager.setTransformMode(Mode::FILL);
			text->setErrorText("Filling is not yet implemented."); break;
		}
		case GLFW_KEY_M : {														// M -> Merge
			sceneManager.setTransformMode(Mode::MERGE);
			text->setErrorText("Merging is not yet implemented."); break;
		}

		// Set Transform SubMode
		case GLFW_KEY_X : if (sceneManager.transformMode.mode != Mode::NONE) sceneManager.setTransformSubMode(SubMode::X); break; // X -> Snap transformation to X direction
		case GLFW_KEY_Y : if (sceneManager.transformMode.mode != Mode::NONE) sceneManager.setTransformSubMode(SubMode::Y); break; // Y -> Snap transformation to Y direction
		case GLFW_KEY_Z : if (sceneManager.transformMode.mode != Mode::NONE) sceneManager.setTransformSubMode(SubMode::Z); break; // Z -> Snap transformation to Z direction

		default: text->setErrorText("Invalid key.");
	}
}