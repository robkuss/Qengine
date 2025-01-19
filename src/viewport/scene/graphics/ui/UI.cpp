#include "UI.h"

#include "UIBar.h"
#include "UITab.h"
#include "UIOption.h"

#include <memory>
#include <viewport/Viewport.h>

#include "text/Debug.h"

// Position markers
int UI::boundLeft, UI::boundRight, UI::boundTop, UI::boundBottom;
float UI::firstLineX;
float UI::firstLineY;
float UI::bottomLineY;


UI::UI(const Viewport *viewport) : viewport(viewport) {
	vp = &viewport->viewport;

	#ifdef TEXT
		text = new Text();	// Initialize FreeType for on-screen text
	#endif
}

UI::~UI() {
	// Cleanup
	#ifdef TEXT
		text->~Text();
	#endif
}


void UI::setup() {
	// Create UIOptionLists
	auto fileOptions = std::make_shared<UIOptionList>("New", "Open", "Save");
	auto editOptions = std::make_shared<UIOptionList>("Undo", "Redo", "Cut", "Copy", "Paste");

	// Create a nested option list for "Mesh"
	auto meshOptions  = std::make_shared<UIOptionList>("Cube", "Sphere");
	auto lightOptions = std::make_shared<UIOptionList>("Point", "Sun");

	// Create the "Add" tab's option list
	auto addOptions = std::make_shared<UIOptionList>(
		std::make_shared<UIOptionList>("Mesh", meshOptions),
		std::make_shared<UIOptionList>("Light", lightOptions)
	);

	// Create UITabs
	const auto fileTab = std::make_shared<UITab>("File", fileOptions);
	const auto editTab = std::make_shared<UITab>("Edit", editOptions);
	const auto addTab  = std::make_shared<UITab>("Add", addOptions);

	// Create UIBar with properties and tabs
	addElement(std::make_shared<UIBar>(
		Vector2(0, 0),
		Dim(1.0f, DimType::Percent),
		Dim(40.0f, DimType::Pixels),
		fileTab,
		editTab,
		addTab
	));

	update();
}

void UI::update() const {
	// Reinitialize bounds to the full viewport dimensions
	/*boundLeft = 0;
	boundRight = (*vp)[2];
	boundTop = 0;
	boundBottom = (*vp)[3];*/

	for (const auto& element : elements) {
		element->update();
	}

	for (const auto& vertex : vertexPointers) {
		// Check if the vertex coordinates (x and y) confine the bounds more
		if (vertex->x < boundLeft)   boundLeft   = static_cast<int>(vertex->x);
		if (vertex->x > boundRight)  boundRight  = static_cast<int>(vertex->x);
		if (vertex->y > boundTop)    boundTop    = static_cast<int>(vertex->y);
		if (vertex->y < boundBottom) boundBottom = static_cast<int>(vertex->y);
	}

	firstLineX  = (10.0f + static_cast<float>(boundLeft))   / Text::fontScale;
	firstLineY  = (35.0f + static_cast<float>(boundTop))    / Text::fontScale;
	bottomLineY = (10.0f + static_cast<float>(boundBottom)) / Text::fontScale;
}

void UI::render() const {
	// Save the current matrix state (3D perspective matrix)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, (*vp)[2], (*vp)[3], 0, -1, 1);

	// Switch to the model view matrix to render the bar
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	for (const auto& element : elements) {
		element->render();
	}

	#ifdef TEXT
		Debug::drawDebugText(viewport);
		text->drawErrorText((*vp)[3]);
	#endif

	// Restore the model view matrix
	glPopMatrix();

	// Restore the projection matrix (back to 3D perspective)
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Switch back to the model view matrix
	glMatrixMode(GL_MODELVIEW);
}
