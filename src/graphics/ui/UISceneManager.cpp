#include "UISceneManager.h"

#include <iostream>
#include <memory>
#include <ranges>

#include "objects/Object.h"
#include "objects/light/Light.h"
#include "UIBulletTree.h"
#include "UIWindow.h"

Dim UISceneManager::sx = Dim(400.0f, DimType::Pixels);
Dim UISceneManager::sy = PC_100;

shared_ptr<UIWindow> UISceneManager::window = make_shared<UIWindow>("Scene Manager", sx, sy);
shared_ptr<UIBulletTreeNode> UISceneManager::bpTree = make_shared<UIBulletTreeNode>(make_shared<UIBulletPoint>("Scene Manager"));


void UISceneManager::render(const float xpos, const float ypos) {
	window->render(xpos, ypos);

	glLineWidth(1.0f);
	color3f(Colors::UI_OUTLINE_COLOR);
	glBegin(GL_LINES);
	glVertex2f(xpos, ypos + 1.65f * unit);
	glVertex2f(xpos + sx.value, ypos + 1.65f * unit);
	glEnd();

	Text::renderText(
		window->label,
		TextMode::LEFT,
		xpos + 2.5f * unit,
		ypos + unit,
		24,
		Colors::TEXT_COLOR
	);

	float yline = ypos + 2.7f * unit;

	// Recursive lambda for rendering tree nodes
	function<void(const UIBulletTreeNode&, float, int)> renderNode =
		[&](const UIBulletTreeNode& node, const float xline, const int depth) {
			// Render the current node's bullet point
			if (node.bp) {
				node.bp->sx = sx - (xline - xpos) - unit;
				node.bp->sy = UNIT;
				node.bp->render(
					xline,
					yline
				);
			}

			// Recursively render the children of the current node
			for (const auto& child : node.children) {
				// Update yline for the next row
				yline += unit;

				renderNode(*child, xline + unit, depth + 1);
			}
	};

	// Start rendering
	for (const auto& child : bpTree->children) {
		renderNode(*child, xpos + unit, 0);
		yline += unit;
	}
}

void UISceneManager::update() {
	for (const auto& scene : SceneManager::scenes) {
		bpTree->addBulletPoint(scene->name, bpTree->bp->label, []{});

		for (const auto& object : scene->sceneObjects) {
			bpTree->addBulletPoint(object->name, scene->name, [object]{ SceneManager::selectObject(object); });
		}

		for (const auto& light : scene->lights) {
			bpTree->addBulletPoint(light->name, scene->name, [light]{ /*SceneManager::selectObject(light);*/ });
		}
	}
}
