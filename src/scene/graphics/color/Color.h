#pragma once

class Color {
public:
	Color(const int r, const int g, const int b, const int a = 255)
		: r(r), g(g), b(b), a(a) {}

	[[nodiscard]] float red()   const { return static_cast<float>(r) / 255.0f; }
	[[nodiscard]] float green() const { return static_cast<float>(g) / 255.0f; }
	[[nodiscard]] float blue()  const { return static_cast<float>(b) / 255.0f; }
	[[nodiscard]] float alpha() const { return static_cast<float>(a) / 255.0f; }

	[[nodiscard]] GLfloat* toGLfloat() const { return new GLfloat[4]{red(), green(), blue(), alpha()}; }

private:
	int r, g, b, a;
};

inline void clearColor(const Color& color) {
	glClearColor(color.red(), color.green(), color.blue(), color.alpha());
}

inline void color3f(const Color& color) {
	glColor3f(color.red(), color.green(), color.blue());
}
