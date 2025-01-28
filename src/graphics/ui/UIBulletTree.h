#pragma once

#include "UIBulletPoint.h"


struct UIBulletTreeNode {
	std::shared_ptr<UIBulletPoint> bp;
	std::vector<std::shared_ptr<UIBulletTreeNode>> children;

	// Constructor to initialize with a bullet point
	explicit UIBulletTreeNode(const std::shared_ptr<UIBulletPoint>& bulletPoint)
		: bp(bulletPoint) {}


	void addBulletPoint( // NOLINT(*-no-recursion)
		const std::string& label,
		const std::string& parentLabel,
		const std::function<void()>& onClickAction
	) {
		// If the parentLabel matches the current node's label, add the new node to its children
		if (bp->label == parentLabel) {
			// Do not add the bullet point if its label already exists
			for (const auto& child : children) {
				if (child->bp->label == label) return;
			}

			auto newBulletPoint = std::make_shared<UIBulletPoint>(label);
			const auto newNode = std::make_shared<UIBulletTreeNode>(newBulletPoint);
			children.push_back(newNode);

			// Set onClick event for the bullet point's button
			newBulletPoint->setOnClickAction(onClickAction);

			return;
		}

		// Otherwise, search recursively in the children
		for (const auto& child : children) {
			child->addBulletPoint(label, parentLabel, onClickAction);
		}
	}
};
