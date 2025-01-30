#include "UI.h"

#include <ranges>

#include "Labels.h"
#include "UIBar.h"
#include "UIOption.h"
#include "UISceneManager.h"
#include "graphics/text/Debug.h"


int *UI::width, *UI::height;

int UI::boundLeft, UI::boundRight, UI::boundTop, UI::boundBottom;
float UI::firstLineX;
float UI::firstLineY;
float UI::bottomLineY;

bool UI::unsavedChanges = false;

vector<const Vector2*> UI::vertexPointers = vector<const Vector2*>();
map<int, vector<shared_ptr<UIElement>>> UI::layers = map<int, vector<shared_ptr<UIElement>>>();


void UI::setup(int* w, int* h) {
	width = w;
	height = h;

	// Initialize FreeType for on-screen text
	Text();

	// Initialize highest-level UIOptionLists
	auto tabs = vector<shared_ptr<UIOptionList>>();

	// Iterate through labels for each UITab and create elements
	for (const auto& [i, labelList] : views::enumerate(uiStructure)) {
		// Initialize dimensions
		const auto tabX  = firstTabX + static_cast<float>(i) * tabWidth;
		constexpr auto tabY  = tabPadding;
		constexpr auto tabSX = Dim(tabWidth, DimType::Pixels);
		constexpr auto optSX = Dim(optionWidth, DimType::Pixels);
		constexpr auto tabSY = Dim(unit - 2.0f * tabPadding, DimType::Pixels);
		constexpr auto optSY = UNIT;

		// Dynamically and recursively create UIOptionList from the labels
		auto options = vector<shared_ptr<UIOptionVariant>>();
		for (const auto& [j, label] : views::enumerate(labelList->children)) {
			options.push_back(make_shared<UIOptionVariant>(
				createOptionListRecursively(j, label, tabX, tabY, optSX, optSY)
			));
		}

		// Dynamically create UITab from the UIOptionList
		const auto tab = make_shared<UIOptionList>(
			labelList->label, true, options, tabX, tabY, tabSX, tabSY
		);

		// Add UITabs to the UI Scene
		addElement(tab, 1);

		// Initialite Button onClick() events
		setOptionButtonOnClickEvents(tab);

		tabs.emplace_back(tab);
	}

	// Create UIBar from the UITabs
	const auto bar = make_shared<UIBar>(
		tabs,
		"UIBar",
		PC_100,
		UNIT
	);

	// Add UIBar to the UI Scene
	addElement(bar, 0);
}

void UI::cleanup() {
	Text::destruct();
}


UIOptionVariant UI::createOptionListRecursively(	// NOLINT(*-no-recursion)
	const long long index,
	const shared_ptr<LabelNode>& label,
	const float x,
	const float y,
	const Dim sx,
	const Dim sy
) {
	// Calculate new Y position for the current node
	const auto newY = y + static_cast<float>(index + 1) * sy.value;

	// Base case: leaf node (single UIOption)
	if (label->children.empty()) {
		return *make_shared<UIOption>(label->label, x, newY, sx, sy);
	}

	// Recursive case: parent node (UIOptionList)
	auto options = vector<shared_ptr<UIOptionVariant>>();
	for (const auto& [i, child] : views::enumerate(label->children)) {
		const auto optionX = x + sx.value;
		const auto optionY = y + static_cast<float>(index) * sy.value;

		// Recursive call for child
		options.push_back(make_shared<UIOptionVariant>(
			createOptionListRecursively(i, child, optionX, optionY, sx, sy)
		));
	}

	// Create and return a UIOptionList
	return *make_shared<UIOptionList>(
		label->label, false, options, x, newY, sx, sy
	);
}

void UI::addElement(const shared_ptr<UIElement>& element, const int layer) {
	for (const auto& vertex : element->vertices) {
		vertexPointers.push_back(&vertex);
	}
	layers[layer].push_back(element);

	unsavedChanges = true;
}

void UI::update() {
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

void UI::render() {
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

	// Render SceneManager
	const auto x  =	static_cast<float>(*width) - 400.0f;
	const auto y	  = static_cast<float>(boundTop);
	UISceneManager::render(x, y);


	// Render elements
	for (const auto &elementsOnLayer: layers | views::values) {
		for (const auto& element : elementsOnLayer) {
			element->render(0.0f, 0.0f);
		}
	}

	// Resize UI if necessary
	if (unsavedChanges) {
		update();
		unsavedChanges = false;
	}


	// Restore matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}


void UI::checkButtonPressed() {
	for (const auto &elementsOnLayer: layers | views::values) {
		for (const auto& element : elementsOnLayer) {
			if (const auto buttonElement = dynamic_pointer_cast<const UIButtonElement>(element)) {
				buttonElement->checkButtonPressed();
			}
		}
	}
}
