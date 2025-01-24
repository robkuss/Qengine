#pragma once

// Node structure for holding labels
struct LabelNode {
	std::string label;
	std::vector<std::shared_ptr<LabelNode>> children;

	explicit LabelNode(std::string label) : label(std::move(label)) {}
};

// Helper function to create a node with children
inline std::shared_ptr<LabelNode> cn(const std::string& label, const std::initializer_list<std::shared_ptr<LabelNode>> children = {}) {
	auto node = std::make_shared<LabelNode>(label);
	node->children.insert(node->children.end(), children.begin(), children.end());
	return node;
}


// Initialize labels for UIElements
const auto uiStructure = std::vector{
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
