#pragma once

#include <complex>
#include <functional>
#include <vector>
#include <memory>

#include "UIElement.h"
#include "viewport/scene/Scene.h"
#include "../text/Text.h"

class UIBulletPoint;
class Vector2;
struct LabelNode;


class UI {
public:
	static int *width, *height;
	static bool unsavedChanges;

	static void setup(int *w, int *h);
	static void update();

	static void render();

	static void addElement(const shared_ptr<UIElement> &element, int layer);

	static void checkButtonPressed();

	static void setOptionButtonOnClickEvents(const shared_ptr<UIOptionList>& tab);
	static bool setOnClickForOptionButton(const shared_ptr<UIOptionList> &list, const string &label, const function<void()> &onClickAction);

protected:
	friend class Text;
	friend class Debug;
	friend class Viewport;

	static float firstLineX;
	static float firstLineY;
	static float bottomLineY;

	static int boundTop, boundBottom, boundLeft, boundRight;

private:
	// Elements are stored as pointers, sorted by layer (lowest layer first)
	static map<int, vector<shared_ptr<UIElement>>> layers;
	static vector<const Vector2*> vertexPointers;

	static UIOptionVariant createOptionListRecursively(long long index, const shared_ptr<LabelNode> &label, float x, float y, Dim sx, Dim sy);
};
