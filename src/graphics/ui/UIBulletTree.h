#pragma once

using namespace std;

#include "UIBulletPoint.h"


class UIBulletTreeNode {
public:
	shared_ptr<UIBulletPoint> bp;
	vector<shared_ptr<UIBulletTreeNode>> children;

	// Constructor to initialize with a bullet point
	explicit UIBulletTreeNode(const shared_ptr<UIBulletPoint>& bulletPoint)
		: bp(bulletPoint) {}


	void addBulletPoint( // NOLINT(*-no-recursion)
		const string& label,
		const string& parentLabel,
		const function<void()>& onClickAction
	) {
		// If the parentLabel matches the current node's label, add the new node to its children
		if (bp->label == parentLabel) {
			// Do not add the bullet point if its label already exists
			for (const auto& child : children) {
				if (child->bp->label == label) return;
			}

			const auto newBulletPoint = make_shared<UIBulletPoint>(label);

			// Set onClick event for the bullet point's button
			newBulletPoint->setOnClickAction(onClickAction);

			const auto newNode = make_shared<UIBulletTreeNode>(newBulletPoint);
			children.push_back(newNode);

			return;
		}

		// Otherwise, search recursively in the children
		for (const auto& child : children) {
			child->addBulletPoint(label, parentLabel, onClickAction);
		}
	}
};
