#include "UI.h"

#include "UIBar.h"
#include "text/Debug.h"

#include <memory>
#include <ranges>


#define PC_100 (Dim(1.0f, DimType::Percent))	// 100%
#define PC_50  (Dim(0.5f, DimType::Percent))	// 50%
#define PC_25  (Dim(0.25f, DimType::Percent))	// 25%


int *UI::width, *UI::height;

int UI::boundLeft, UI::boundRight, UI::boundTop, UI::boundBottom;
float UI::firstLineX;
float UI::firstLineY;
float UI::bottomLineY;

constexpr int tabFontSize	= 20;

constexpr float barHeight	= 40.0f;
constexpr float firstTabX	= barHeight;
constexpr float tabPadding	= 1.5f;		// Upper and lower padding within the bar
constexpr float buttonWidth	= 120.0f;


UI::UI(int* w, int* h) : Scene() {
	width = w;
	height = h;
	Text();		// Initialize FreeType for on-screen text
}

UI::~UI() {
	Text::destruct();
}


void UI::setup() {
	// Create UIOptionLists
	auto fileOptions  = std::make_shared<UIOptionList>("New", "Open", "Save");
	auto editOptions  = std::make_shared<UIOptionList>("Undo", "Redo", "Cut", "Copy", "Paste");

	auto meshOptions  = std::make_shared<UIOptionList>("Cube", "Sphere");
	auto lightOptions = std::make_shared<UIOptionList>("Point", "Sun");

	// Create the "Add" tab's option list
	auto addOptions = std::make_shared<UIOptionList>(
		std::make_shared<UIOptionList>("Mesh", meshOptions),
		std::make_shared<UIOptionList>("Light", lightOptions)
	);

	// Create UITabs
	const auto fileButton = std::make_shared<Button>("File", tabFontSize);
	const auto editButton = std::make_shared<Button>("Edit", tabFontSize);
	const auto addButton  = std::make_shared<Button>("Add",  tabFontSize);

	const auto fileTab	  = std::make_shared<UITab>(fileButton, fileOptions);
	const auto editTab	  = std::make_shared<UITab>(editButton, editOptions);
	const auto addTab	  = std::make_shared<UITab>(addButton,  addOptions);

	int i = 0;
	for (const auto& tab : std::vector{fileTab, editTab, addTab}) {
		tab->x  = tab->button->x  = firstTabX + static_cast<float>(i) * buttonWidth;
		tab->button->y = tabPadding;
		tab->sx = tab->button->sx = Dim(buttonWidth, DimType::Pixels);
		tab->sy = Dim(barHeight, DimType::Pixels);
		tab->button->sy = Dim(barHeight - 2*tabPadding, DimType::Pixels);

		tab->setVertices();
		tab->button->setVertices();
		tab->button->setActivated(true);

		addElement(tab, 1);
		addElement(tab->button, 2);

		i++;
	}

	// Create UIBar
	const auto bar = std::make_shared<UIBar>(
		fileTab,
		editTab,
		addTab
	);
	bar->x = bar->y = 0;
	bar->sx = PC_100;
	bar->sy = Dim(barHeight, DimType::Pixels);
	addElement(bar, 0);

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
