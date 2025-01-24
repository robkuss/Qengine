#pragma once

#include <vector>
#include <memory>

#include "UIElement.h"
#include "viewport/scene/Scene.h"
#include "../text/Text.h"


class Vector2;
struct LabelNode;


class UI final : public Scene {
public:
	static int *width, *height;

	// Constructor & Destructor
	UI(int *w, int *h);
	~UI() override;

	void setup();
	void update() const;
	void render() const override;

	void checkButtonPressed();

	void addElement(const std::shared_ptr<UIElement> &element, int layer);

	static std::shared_ptr<UIElement> variantToElement(const std::shared_ptr<UIOptionVariant> &variant);

protected:
	friend class Text;
	friend class Debug;

	static float firstLineX;
	static float firstLineY;
	static float bottomLineY;

	static int boundTop, boundBottom, boundLeft, boundRight;

private:
	// Elements are stored as pointers, sorted by layer (lowest layer first)
	std::map<int, std::vector<std::shared_ptr<UIElement>>> layers;
	std::vector<const Vector2*> vertexPointers;

	static UIOptionVariant createOptionListRecursively(long long index, const std::shared_ptr<LabelNode> &label, float x, float y, Dim sx, Dim sy);
};
