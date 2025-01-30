#pragma once

#include <memory>

#include "UIWindow.h"

struct UIBulletTreeNode;
class UIWindow;


class UISceneManager {
public:
	static void render(float xpos, float ypos);
	static void update();

	void addBulletPoint(const string &label, const string &parentLabel);

private:
	friend class UI;

	static Dim sx;
	static Dim sy;

	static shared_ptr<UIWindow> window;
	static shared_ptr<UIBulletTreeNode> bpTree;
};
