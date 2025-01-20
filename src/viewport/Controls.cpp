#include "viewport/Viewport.h"
#include "scene/Scene.h"

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
			vp->render();		// Force a re-render during resizing
			vp->ui->update();	// Resize UI
			glfwSwapBuffers(cbWindow);
		}
	});

	// Mouse button callbacks
	glfwSetMouseButtonCallback(window, [](GLFWwindow* cbWindow, const int button, const int action, const int) {
		const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow));
		for (const auto& scene : scenes) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				const auto mousePos = Vector2(*mouseX, *mouseY);
				vp->setMouseRay(mousePos);
				scene->select(mousePos, false);
			}
			else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				vp->activeCamera.initRotation(action == GLFW_PRESS, *mouseX, *mouseY);
			}
		}
	});

	// Cursor position callback
	glfwSetCursorPosCallback(window, [](GLFWwindow* cbWindow, const double x, const double y) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow))) {
			// Update the mouse position in the Viewport
			glfwGetCursorPos(cbWindow, mouseX, mouseY);

			if (vp->context->transformMode == NONE) {
					// Viewport rotation
					if (vp->activeCamera.rotating) {
						vp->activeCamera.rotate(x, y);
					}
				} else {
					// Object transformation
					const Vector3 worldPos = unproject(Vector2(*mouseX, *mouseY), &vp->viewport, vp->activeCamera.viewMatrix, vp->activeCamera.projMatrix);
					vp->context->transform(x, y, worldPos, vp->activeCamera.camPos);
				}
		}
	});

	// Mouse scroll callback
	glfwSetScrollCallback(window, [](GLFWwindow* cbWindow, const double, const double yOffset) {
		if (const auto vp = static_cast<Viewport*>(glfwGetWindowUserPointer(cbWindow)))
			vp->activeCamera.zoom(yOffset);
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
			context->toggleSelectionMode();
			for (const auto& scene : scenes)
				scene->deselectAllVertices();	// Reset vertex selection data
			break;
		}

		// Number keys for perspective toggling
		case GLFW_KEY_1: activeCamera.setPerspective(  0.0f,  0.0f); break;			// 1 -> Front View  (towards negative X)
		case GLFW_KEY_2: activeCamera.setPerspective(-90.0f,  0.0f); break;			// 2 -> Right View  (towards negative Y)
		case GLFW_KEY_3: activeCamera.setPerspective(  0.0f, 90.0f); break;			// 3 -> Top View    (towards negative Z)
		case GLFW_KEY_4: activeCamera.setPerspective(180.0f,  0.0f); break;			// 4 -> Back View   (towards positive X)
		case GLFW_KEY_5: activeCamera.setPerspective( 90.0f,  0.0f); break;			// 5 -> Left View   (towards positive Y)
		case GLFW_KEY_6: activeCamera.setPerspective(  0.0f,-90.0f); break;			// 6 -> Bottom View (towards positive Z)

		// Set Transform Mode
		case GLFW_KEY_G: context->setTransformMode(GRAB); break;						// G -> Grab
		case GLFW_KEY_S: context->setTransformMode(SCALE); break;						// S -> Scale
		case GLFW_KEY_R: context->setTransformMode(ROTATE); break; 						// R -> Rotate
		case GLFW_KEY_E: context->setTransformMode(EXTRUDE); break;						// E -> Extrude
		case GLFW_KEY_F: context->setTransformMode(FILL); break;						// F -> Fill
		case GLFW_KEY_M: context->setTransformMode(MERGE); break;						// M -> Merge

		// Set Transform SubMode
		case GLFW_KEY_X || GLFW_KEY_Y || GLFW_KEY_Z: if (context->transformMode != NONE) {
			case GLFW_KEY_X: context->setTransformSubMode(SubMode::X); break;			// X -> Snap transformation to X direction
			case GLFW_KEY_Z: context->setTransformSubMode(SubMode::Y); break;			// Z -> Snap transformation to Y direction TODO: Make this depend on scancode, not key
			case GLFW_KEY_Y: context->setTransformSubMode(SubMode::Z); break;			// Y -> Snap transformation to Z direction TODO: Make this depend on scancode, not key
		}

		// Toggle Shading
		case GLFW_KEY_LEFT_SHIFT: context->toggleShadingMode();					// Left Shift -> Toggle ShadingMode for selected Mesh

		default: {
			#ifdef TEXT
				//text->setErrorText("Invalid key.");
			#endif
		}
	}
}
