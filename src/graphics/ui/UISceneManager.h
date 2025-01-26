#pragma once

#include <map>
#include <memory>

#include "UIElement.h"

class UIBulletPoint;


class UISceneManager final : public UIElement {
public:
	UISceneManager(
		const std::string& label,
		const Dim sx,
		const Dim sy
	) : UIElement(label, sx, sy) {}

	void render(float xpos, float ypos) override;

	static void addBulletPoint(const std::string &label, const std::string &parentLabel);

private:
	friend class UI;

	static std::map<std::string, std::vector<std::shared_ptr<UIBulletPoint>>> bulletPoints;
};
