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


class UI final : public Scene {
public:
	static int *width, *height;
	static bool unsavedChanges;

	// Constructor & Destructor
	UI(const std::string &name, int *w, int *h);
	~UI() override;

	static void setup();
	static void update() ;

	void render() override;

	static void addElement(const std::shared_ptr<UIElement> &element, int layer);

	static void checkButtonPressed();

	static void setOptionButtonOnClickEvents(const std::shared_ptr<UIOptionList>& tab);
	static bool setOnClickForOptionButton(const std::shared_ptr<UIOptionList> &list, const std::string &label, const std::function<void()> &onClickAction);

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
	static std::map<int, std::vector<std::shared_ptr<UIElement>>> layers;
	static std::vector<const Vector2*> vertexPointers;

	static UIOptionVariant createOptionListRecursively(long long index, const std::shared_ptr<LabelNode> &label, float x, float y, Dim sx, Dim sy);
};
