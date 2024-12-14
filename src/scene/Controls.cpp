#include <math/vector/Vector2.h>
#include <scene/graphics/Viewport.h>

#include "Mode.h"
#include "SceneManager.h"

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
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow))) {
			vp->windowResize(width, height);
			vp->render(); // Force a re-render during resizing
			glfwSwapBuffers(cbWindow);
		}
	});

	// Mouse button callbacks
	glfwSetMouseButtonCallback(window, [](GLFWwindow* cbWindow, const int button, const int action, const int) {
		const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow));
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			const auto mousePos = Vector2(*vp->mouseX, *vp->mouseY);
			vp->setMouseRay(mousePos);
			vp->sceneManager->context->mouseRay = &vp->mouseRay;
			vp->sceneManager->select(mousePos, false);
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			vp->initRotation(action == GLFW_PRESS);
		}
	});

	// Cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* cbWindow, const double mouseX, const double mouseY) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow))) {
			// Update the mouse position in the Viewport
			glfwGetCursorPos(cbWindow, vp->mouseX, vp->mouseY);

			if (vp->sceneManager->transformMode == NONE) {
				// Viewport rotation
				if (vp->rotating) {
					vp->rotate(mouseX, mouseY);
				}
			} else {
				// Object transformation
				const Vector3 worldPos = unproject(Vector2(*vp->mouseX, *vp->mouseY), vp->viewport, vp->viewMatrix, vp->projMatrix);
				vp->sceneManager->transform(mouseX, mouseY, worldPos, vp->camPos);
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
		case GLFW_KEY_TAB : sceneManager->toggleSelectionMode(); break;				// TAB -> Toggle Object/Edit Mode

		// Number keys for perspective toggling
		case GLFW_KEY_1: setPerspective(  0.0f,  0.0f); break;					// 1 -> Front View  (towards negative X)
		case GLFW_KEY_2: setPerspective(-90.0f,  0.0f); break;					// 2 -> Right View  (towards negative Y)
		case GLFW_KEY_3: setPerspective(  0.0f, 90.0f); break;					// 3 -> Top View    (towards negative Z)
		case GLFW_KEY_4: setPerspective(180.0f,  0.0f); break;					// 4 -> Back View   (towards positive X)
		case GLFW_KEY_5: setPerspective( 90.0f,  0.0f); break;					// 5 -> Left View   (towards positive Y)
		case GLFW_KEY_6: setPerspective(  0.0f,-90.0f); break;					// 6 -> Bottom View (towards positive Z)

		// Set Transform Mode
		case GLFW_KEY_G: sceneManager->setTransformMode(GRAB); break;				// G -> Grab
		case GLFW_KEY_S: sceneManager->setTransformMode(SCALE); break;				// S -> Scale
		case GLFW_KEY_R: sceneManager->setTransformMode(ROTATE); break; 			// R -> Rotate
		case GLFW_KEY_E: sceneManager->setTransformMode(EXTRUDE); break;			// E -> Extrude
		case GLFW_KEY_F: sceneManager->setTransformMode(FILL); break;				// F -> Fill
		case GLFW_KEY_M: sceneManager->setTransformMode(MERGE); break;				// M -> Merge

		// Set Transform SubMode
		case GLFW_KEY_X || GLFW_KEY_Y || GLFW_KEY_Z: if (sceneManager->transformMode != NONE) {
			case GLFW_KEY_X: sceneManager->setTransformSubMode(SubMode::X); break;	// X -> Snap transformation to X direction
			case GLFW_KEY_Z: sceneManager->setTransformSubMode(SubMode::Y); break;	// Z -> Snap transformation to Y direction TODO: Make this depend on scancode, not key
			case GLFW_KEY_Y: sceneManager->setTransformSubMode(SubMode::Z); break;	// Y -> Snap transformation to Z direction TODO: Make this depend on scancode, not key
		}

		// Toggle Shading
		case GLFW_KEY_LEFT_SHIFT: sceneManager->toggleShadingMode();				// Left Shift -> Toggle ShadingMode for selected Mesh

		default: {
			#ifdef TEXT
				text->setErrorText("Invalid key.");
			#endif
		}
	}
}
