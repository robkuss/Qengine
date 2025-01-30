#include "scene/SceneManager.h"
#include "viewport/Viewport.h"

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
			vp->render();				// Force a re-render during resizing
			UI::unsavedChanges = true;	// Trigger UI resize

			glfwSwapBuffers(cbWindow);
		}
	});

	// Mouse button callbacks
	glfwSetMouseButtonCallback(window, [](GLFWwindow* cbWindow, const int button, const int action, const int) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow))) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				const auto mousePos = Vector2(*SceneManager::mouseX, *SceneManager::mouseY);
				vp->setMouseRay(mousePos);
				SceneManager::select(mousePos, false);
				UI::checkButtonPressed();
			}

			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				vp->activeCamera->initRotation(action == GLFW_PRESS, *SceneManager::mouseX, *SceneManager::mouseY);
			}
		}
	});

	// Cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* cbWindow, const double x, const double y) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow))) {
			// Update the mouse position in the Viewport
			glfwGetCursorPos(cbWindow, SceneManager::mouseX, SceneManager::mouseY);

			if (SceneManager::transformMode == NONE) {
				// Viewport rotation
				if (vp->activeCamera->rotating) {
					vp->activeCamera->rotate(x, y);
				}
			} else {
				// Object transformation
				const Vector3 worldPos = unproject(Vector2(*SceneManager::mouseX, *SceneManager::mouseY), vp->viewport.get(), vp->activeCamera->viewMatrix, vp->activeCamera->projMatrix);
				SceneManager::transform(x, y, worldPos, vp->activeCamera->camPos);
			}
		}
	});

	// Mouse scroll callback
	glfwSetScrollCallback(window, [](GLFWwindow* cbWindow, const double, const double yOffset) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->activeCamera->zoom(yOffset);
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
		case GLFW_KEY_TAB : {															// TAB -> Toggle Object/Edit Mode
			SceneManager::toggleSelectionMode();
			SceneManager::deselectAllVertices();
			break;
		}

		// Number keys for perspective toggling
		case GLFW_KEY_1: activeCamera->setPerspective(  0.0f,  0.0f); break;		// 1 -> Front View  (towards negative X)
		case GLFW_KEY_2: activeCamera->setPerspective(-90.0f,  0.0f); break;		// 2 -> Right View  (towards negative Y)
		case GLFW_KEY_3: activeCamera->setPerspective(  0.0f, 90.0f); break;		// 3 -> Top View    (towards negative Z)
		case GLFW_KEY_4: activeCamera->setPerspective(180.0f,  0.0f); break;		// 4 -> Back View   (towards positive X)
		case GLFW_KEY_5: activeCamera->setPerspective( 90.0f,  0.0f); break;		// 5 -> Left View   (towards positive Y)
		case GLFW_KEY_6: activeCamera->setPerspective(  0.0f,-90.0f); break;		// 6 -> Bottom View (towards positive Z)

		// Set Transform Mode
		case GLFW_KEY_G: SceneManager::setTransformMode(GRAB); break;					// G -> Grab
		case GLFW_KEY_S: SceneManager::setTransformMode(SCALE); break;					// S -> Scale
		case GLFW_KEY_R: SceneManager::setTransformMode(ROTATE); break; 				// R -> Rotate
		case GLFW_KEY_E: SceneManager::setTransformMode(EXTRUDE); break;				// E -> Extrude
		case GLFW_KEY_F: SceneManager::setTransformMode(FILL); break;					// F -> Fill
		case GLFW_KEY_M: SceneManager::setTransformMode(MERGE); break;					// M -> Merge

		case GLFW_KEY_C: drawCoordinateSystem = !drawCoordinateSystem; break;			// C -> Toggle coordinate system visibility

		// Set Transform SubMode
		case GLFW_KEY_X || GLFW_KEY_Y || GLFW_KEY_Z: {
			if (SceneManager::transformMode != NONE) {
				case GLFW_KEY_X: SceneManager::setTransformSubMode(SubMode::X); break;	// X -> Snap transformation to X direction
				case GLFW_KEY_Z: SceneManager::setTransformSubMode(SubMode::Y); break;	// Z -> Snap transformation to Y direction TODO: Make this depend on scancode, not key
				case GLFW_KEY_Y: SceneManager::setTransformSubMode(SubMode::Z); break;	// Y -> Snap transformation to Z direction TODO: Make this depend on scancode, not key
			}
		}

		// Toggle Shading
		case GLFW_KEY_LEFT_SHIFT: SceneManager::toggleShadingMode();					// Left Shift -> Toggle ShadingMode for selected Mesh

		default: {
			#ifdef TEXT
				//text->setErrorText("Invalid key.");
			#endif
		}
	}
}
