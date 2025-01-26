#include "UISceneManager.h"

#include <memory>

#include <objects/Object.h>

#include "UIBulletPoint.h"
#include "UIWindow.h"


std::map<std::string, std::vector<std::shared_ptr<UIBulletPoint>>> UISceneManager::bulletPoints = std::map<std::string, std::vector<std::shared_ptr<UIBulletPoint>>>();

void UISceneManager::render(const float xpos, const float ypos) {
	bulletPoints.clear();

	const auto window = std::make_shared<UIWindow>(label, sx, sy);
	window->render(xpos, ypos);

	glLineWidth(1.0f);
	color3f(Colors::UI_OUTLINE_COLOR);
	glBegin(GL_LINES);
	glVertex2f(xpos, ypos + 1.65f * unit);
	glVertex2f(xpos + sx.value, ypos + 1.65f * unit);
	glEnd();

	Text::renderText(
		label,
		TextMode::LEFT,
		xpos + 2.5f * unit,
		ypos + unit,
		24,
		Colors::TEXT_COLOR
	);

	float yline = ypos + 2.7f * unit;
	constexpr float textYScalar = 0.2f * unit;

	int i = 0;
	for (const auto& scene : SceneManager::scenes) {
		float xline = xpos + unit;
		constexpr auto textXScalar = 0.6f;

		//bulletPoints["Scene Manager"].push_back(std::make_shared<UIBulletPoint>(scene->name, xline, yline, sx - (xline - xpos) - unit, UNIT, textXScalar, textYScalar));
		xline += unit;
		yline += unit;
		for (const auto& object : scene->sceneObjects) {
			//bulletPoints[scene->name].push_back(std::make_shared<UIBulletPoint>(object->name, xline, yline, sx - (xline - xpos) - unit, UNIT, textXScalar, textYScalar));
			yline += unit;
		}
		for (const auto& light : scene->lights) {
			//bulletPoints[scene->name].push_back(std::make_shared<UIBulletPoint>(light->name, xline, yline, sx - (xline - xpos) - unit, UNIT, textXScalar, textYScalar));
			yline += unit;
		}
		i++;
	}

	for (const auto&[label, bpOnLevel] : bulletPoints) {
		for (const auto& bp : bpOnLevel) {
			bp->render(xpos, ypos);
		}
	}

	UI::setBulletPointButtonOnClickEvents();	// TODO make all of this better with pointers
}

void UISceneManager::addBulletPoint(
	const std::string& label,
	const std::string& parentLabel
) {
	//bulletPoints[parentLabel].push_back(std::make_shared<UIBulletPoint>(label, sx, sy, textXScalar, textYScalar));
}
