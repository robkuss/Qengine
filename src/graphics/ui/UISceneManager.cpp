#include "UISceneManager.h"

#include <memory>

#include <objects/Object.h>

#include "UIBulletPoint.h"
#include "UIWindow.h"


void UISceneManager::render() {
	const auto window = std::make_shared<UIWindow>(
		label,
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
		label,
		TextMode::LEFT,
		x + 2.5f * unit,
		y + unit,
		24,
		Colors::TEXT_COLOR
	);

	const float xline = x + unit;
	float yline = y + 2.7f * unit;
	constexpr float textYScalar = 0.2f * unit;
	int i = 0;
	for (const auto& scene : SceneManager::scenes) {
		auto xScalar = 0.6f;
		UIBulletPoint(scene->name, xline, yline, sx, sy, xline + xScalar * unit, yline + textYScalar).render();
		yline += unit;
		xScalar += 1.0f;
		for (const auto& object : scene->sceneObjects) {
			UIBulletPoint(object->name, xline + unit, yline, sx, sy, xline + xScalar * unit, yline + textYScalar).render();
			yline += unit;
		}
		for (const auto& light : scene->lights) {
			UIBulletPoint(light->name, xline + unit, yline, sx, sy, xline + xScalar * unit, yline + textYScalar).render();
			yline += unit;
		}
		i++;
	}
}
