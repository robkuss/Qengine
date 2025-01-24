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

inline std::shared_ptr<UIElement> UI::variantToElement(const std::shared_ptr<UIOptionVariant>& variant) {
	std::shared_ptr<UIElement> element;
	std::visit(
		[&]<typename T0>(const T0& actualOption) {
			// Create a shared_ptr<UIElement> from the extracted object
			element = std::make_shared<std::decay_t<T0>>(actualOption);
		},
		*variant // Dereference the shared_ptr to access the variant
	);
	return element;
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
