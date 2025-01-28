#pragma once

#include <memory>

#include "UIWindow.h"

struct UIBulletTreeNode;
class UIWindow;


class UISceneManager {
public:
	static void render(float xpos, float ypos);
	static void update();

	void addBulletPoint(const std::string &label, const std::string &parentLabel);

private:
	friend class UI;

	static Dim sx;
	static Dim sy;

	static std::shared_ptr<UIWindow> window;
	static std::shared_ptr<UIBulletTreeNode> bpTree;
};
