#include "UI.h"

#include <ranges>

#include "UIBar.h"
#include "UIOption.h"
#include "Label.h"
#include "UIWindow.h"
#include "../text/Debug.h"


int *UI::width, *UI::height;

int UI::boundLeft, UI::boundRight, UI::boundTop, UI::boundBottom;
float UI::firstLineX;
float UI::firstLineY;
float UI::bottomLineY;


UI::UI(const std::string& name, int* w, int* h) : Scene(name) {
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
	// Initialize highest-level UIOptionLists
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

		// Initialite Button onClick() events
		setButtonOnClickEvents(tab);

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
UIOptionVariant UI::createOptionListRecursively(	 // NOLINT(*-no-recursion)
	const long long index,
	const std::shared_ptr<LabelNode>& label,
	const float x,
	const float y,
	const Dim sx,
	const Dim sy
) {
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

	// Render elements
	for (const auto &elementsOnLayer: layers | std::views::values) {
		for (const auto& element : elementsOnLayer) {
			element->render();
		}
	}

	// Render SceneManager
	renderSceneManager();

	// Restore matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
}

void UI::renderSceneManager() {
	constexpr auto smWidth = 400.0f;
	const auto x  =	static_cast<float>(*width) - smWidth;
	const auto y	  = static_cast<float>(boundTop);
	constexpr auto sx = Dim(smWidth, DimType::Pixels);
	constexpr auto sy = PC_100;

	const auto window = std::make_shared<UIWindow>(
		"Scene Manager",
		x, y, sx, sy
	);
	window->render();

	glLineWidth(1.0f);
	color3f(Colors::UI_OUTLINE_COLOR);
	glBegin(GL_LINES);
	glVertex2f(x, y + 1.65f * unit);
	glVertex2f(x + sx.value, y + 1.65f * unit);
	glEnd();

	Text::renderText(
		"Scene Manager",
		TextMode::LEFT,
		x + 2.5f * unit,
		static_cast<float>(boundTop) + unit,
		24,
		Colors::TEXT_COLOR
	);

	const float xLine = x + unit;
	float yLine = y + 2.7f * unit;
	int i = 0;
	for (const auto& scene : SceneManager::scenes) {
		glPointSize(5.0f);
		color3f(Colors::TEXT_COLOR);
		glBegin(GL_POINTS);
		glVertex2f(xLine, yLine);
		glEnd();
		Text::renderText(scene->name, TextMode::LEFT, xLine + 0.6f * unit, yLine + 0.2f * unit, 24, Colors::TEXT_COLOR);
		yLine += unit;
		for (const auto& object : scene->sceneObjects) {
			glPointSize(5.0f);
			color3f(Colors::TEXT_COLOR);
			glBegin(GL_POINTS);
			glVertex2f(xLine + unit, yLine);
			glEnd();
			Text::renderText(object->name, TextMode::LEFT, xLine + 1.6f * unit, yLine + 0.2f * unit, 24, Colors::TEXT_COLOR);
			yLine += unit;
		}
		for (const auto& light : scene->lights) {
			glPointSize(5.0f);
			color3f(Colors::TEXT_COLOR);
			glBegin(GL_POINTS);
			glVertex2f(xLine + unit, yLine);
			glEnd();
			Text::renderText(light->name, TextMode::LEFT, xLine + 1.6f * unit, yLine + 0.2f * unit, 24, Colors::TEXT_COLOR);
			yLine += unit;
		}
		i++;
	}
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


bool UI::setOnClickForButton(	// NOLINT(*-no-recursion)
	const std::shared_ptr<UIOptionList>& list,
	const std::string& label,
	const std::function<void()>& onClickAction
) {
	if (!list) return false;

	// Check the button of the current UIOptionList
	if (list->button && list->button->label == label) {
		list->button->onClick = onClickAction;
		return true;
	}

	// Check buttons in all UIOptions in the current list
	for (const auto& innerOption : list->options) {
		if (!innerOption) continue;
		const auto button =
			  std::holds_alternative<UIOption>(*innerOption)
			? std::get<UIOption>(*innerOption).button
			: std::get<UIOptionList>(*innerOption).button;

		if (button && button->label == label) {
			button->onClick = onClickAction;
			return true;
		}
	}

	// Recursively check all nested UIOptionLists
	if (std::ranges::any_of(list->options, [&](const auto& nestedList) {
		if (std::holds_alternative<UIOptionList>(*nestedList)) {
			const auto nestedOptionList = std::make_shared<UIOptionList>(std::get<UIOptionList>(*nestedList));
			return setOnClickForButton(nestedOptionList, label, onClickAction);
		}
		return false;
	})) return true;

	return false;	// Return false if no matching button was found
}


