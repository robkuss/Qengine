#pragma once

// Node structure for holding labels
struct LabelNode {
	string label;
	vector<shared_ptr<LabelNode>> children;

	explicit LabelNode(string label) : label(move(label)) {}
};

// Helper function to create a node with children
inline shared_ptr<LabelNode> cn(const string& label, const initializer_list<shared_ptr<LabelNode>> children = {}) {
	auto node = make_shared<LabelNode>(label);
	node->children.insert(node->children.end(), children.begin(), children.end());
	return node;
}


// Initialize labels for UIElements
const auto uiStructure = vector{
	cn("File", {
		cn("New"), cn("Open"), cn("Save"), cn("Save As"), cn("Exit")
	}),
	cn("Edit", {
		cn("Undo"), cn("Redo"), cn("Cut"), cn("Copy"), cn("Paste")
	}),
	cn("Add", {
		cn("Mesh", {
			cn("Cube"), cn("Sphere")
		}),
		cn("Light", {
			cn("Point"), cn("Sun")
		})
	})
};
