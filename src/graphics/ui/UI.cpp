#include "UI.h"

#include "UIBar.h"
#include "../text/Debug.h"

#include <memory>
#include <ranges>


int *UI::width, *UI::height;

int UI::boundLeft, UI::boundRight, UI::boundTop, UI::boundBottom;
float UI::firstLineX;
float UI::firstLineY;
float UI::bottomLineY;


UI::UI(int* w, int* h) : Scene() {
	width = w;
	height = h;
	Text();		// Initialize FreeType for on-screen text
}

UI::~UI() {
	Text::destruct();
}

void UI::setup() {
	std::map<std::string, UIOption> labels = {
		{"File", std::vector<std::string>{"New", "Open", "Save"}},
		{"Edit", std::vector<std::string>{"Undo", "Redo", "Cut", "Copy", "Paste"}},
		{"Add", UIOptionList{
	        {"Mesh", {"Cube", "Sphere"}},
			{"Light", {"Point", "Sun"}}
		}}
	};

	auto tabs = std::vector<std::shared_ptr<UITab>>();
	for (const auto& [i, pair] : std::views::enumerate(labels)) {
		const auto& [key, value] = pair;
		const auto button	  = std::make_shared<Button>(key, tabFontSize);
		const auto optionList = std::make_shared<UIOptionList>();
		const auto tab		  = std::make_shared<UITab>(
			button,
			optionList,
			firstTabX + static_cast<float>(i) * buttonWidth,
			0.0f,
			Dim(buttonWidth, DimType::Pixels),
			UNIT
		);
		addElement(button, 2);
		addElement(tab, 1);
		tabs.emplace_back(tab);
	}

	// Create UIBar
	addElement(
		std::make_shared<UIBar>(
			tabs,
			0.0f, 0.0f, PC_100, UNIT
		),
	0);

	update();
}

void UI::addElement(const std::shared_ptr<UIElement> &element, const int layer) {
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
	glOrtho(
		0,
		*width,
		*height,
		0,
		-1,
		1
	);

	// Save model view matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Render elements
	for (const auto &elementsOnLayer: layers | std::views::values) {
		for (const auto& element : elementsOnLayer) {
			element->render();
		}
	}

	#ifdef DEBUG
		Debug::drawDebugText();
	#endif
	Text::drawErrorText(*height);

	// Restore matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}
