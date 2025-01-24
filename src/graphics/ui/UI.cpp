#include "UI.h"

#include <ranges>

#include "UIBar.h"
#include "UIOption.h"
#include "Label.h"
#include "../text/Debug.h"


int *UI::width, *UI::height;

int UI::boundLeft, UI::boundRight, UI::boundTop, UI::boundBottom;
float UI::firstLineX;
float UI::firstLineY;
float UI::bottomLineY;


UI::UI(int* w, int* h) : Scene() {
	width = w;
	height = h;

	// Initialize FreeType for on-screen text
	Text();
}

UI::~UI() {
	// Cleanup
	Text::destruct();
}


void UI::setup() {
	// Initialize UITab and UIOptionList lists
	auto tabs = std::vector<std::shared_ptr<UIOptionList>>();

	// Iterate through labels for each UITab and create elements
	for (const auto& [i, labelList] : std::views::enumerate(uiStructure)) {

		// Initialize dimensions
		const auto tabX  = firstTabX + static_cast<float>(i) * tabWidth;
		constexpr auto tabY  = tabPadding;
		constexpr auto tabSX = Dim(tabWidth, DimType::Pixels);
		constexpr auto optSX = Dim(optionWidth, DimType::Pixels);
		constexpr auto tabSY = Dim(unit - 2.0f * tabPadding, DimType::Pixels);
		constexpr auto optSY = UNIT;

		// Dynamically and recursively create UIOptionList from the labels
		auto options = std::vector<std::shared_ptr<UIOptionVariant>>();
		for (const auto& [j, label] : std::views::enumerate(labelList->children)) {

			const auto optionVariant = createOptionListRecursively(
				j,
				label,
				tabX, tabY, optSX, optSY
			);

			options.emplace_back(std::make_shared<UIOptionVariant>(optionVariant));
		}

		// Dynamically create UITab from the UIOptionList
		const auto tab = std::make_shared<UIOptionList>(
			labelList->label,
			true,
			options,
			tabX, tabY, tabSX, tabSY
		);

		// Add UITabs to the UI Scene
		addElement(tab, 1);

		tabs.emplace_back(tab);
	}

	// Create UIBar from the UITabs
	const auto bar = std::make_shared<UIBar>(
		tabs,
		"UIBar",
		0.0f, 0.0f, PC_100, UNIT
	);

	// Add UIBar to the UI Scene
	addElement(bar, 0);

	update();
}

// ReSharper disable once CppNotAllPathsReturnValue
UIOptionVariant UI::createOptionListRecursively(const long long index, const std::shared_ptr<LabelNode>& label, const float x, const float y, const Dim sx, const Dim sy) { // NOLINT(*-no-recursion)
	// If the node has no children, it is a single UIOption
	const auto newY = y + static_cast<float>(index + 1) * sy.value;
	if (label->children.empty()) {
		return *std::make_shared<UIOption>(
			label->label,
			x, newY, sx, sy
		);
	}

	// If the node has children, it is a UIOptionList -> process each child recursively
	auto options = std::vector<std::shared_ptr<UIOptionVariant>>();
	for (const auto& [i, child] : std::views::enumerate(label->children)) {
		const auto optionX  = x + sx.value;
		const auto optionY  = y + static_cast<float>(index) * sy.value;

		// Recursive call for each child
		auto optionVariant = createOptionListRecursively(
			i,
			child,
			optionX, optionY, sx, sy
		);

		options.push_back(std::make_shared<UIOptionVariant>(optionVariant));
	}

	// Create and return a UIOptionList for this node
	return *std::make_shared<UIOptionList>(
		label->label,
		false,
		options,
		x, newY, sx, sy
	);
}

void UI::addElement(const std::shared_ptr<UIElement>& element, const int layer) { // NOLINT(*-no-recursion)
	for (const auto& vertex : element->vertices) {
		vertexPointers.push_back(&vertex);
	}

	layers[layer].push_back(element);
}

void UI::update() const {
	for (const auto &elementsOnLayer: layers | std::views::values) {
		for (const auto& element : elementsOnLayer) {
			element->setVertices();
		}
	}

	for (const auto& vertex : vertexPointers) {
		// Check if the vertex coordinates (x and y) confine the bounds more
		if (vertex->x < boundLeft)   boundLeft   = static_cast<int>(vertex->x);
		if (vertex->x > boundRight)  boundRight  = static_cast<int>(vertex->x);
		if (vertex->y > boundTop)    boundTop    = static_cast<int>(vertex->y);
		if (vertex->y < boundBottom) boundBottom = static_cast<int>(vertex->y);
	}

	firstLineX  = 15.0f + static_cast<float>(boundLeft);
	firstLineY  = 35.0f + static_cast<float>(boundTop) ;
	bottomLineY = 10.0f + static_cast<float>(boundBottom);
}

void UI::render() const {
	glDisable(GL_DEPTH_TEST);	// Disable depth testing for 2D UI rendering
	glEnable(GL_BLEND);			// Enable blending for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Save projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, *width, *height, 0, -1, 1);

	// Save model view matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Render text
	#ifdef DEBUG
		Debug::drawDebugText();
	#endif
	Text::drawErrorText(*height);

	// Render elements
	for (const auto &elementsOnLayer: layers | std::views::values) {
		for (const auto& element : elementsOnLayer) {
			element->render();
		}
	}

	// Restore matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}

void UI::checkButtonPressed() {
	for (const auto &elementsOnLayer: layers | std::views::values) {
		for (const auto& element : elementsOnLayer) {
			if (const auto buttonElement = std::dynamic_pointer_cast<const UIButtonElement>(element)) {
				buttonElement->checkButtonPressed();
			}
		}
	}
}
