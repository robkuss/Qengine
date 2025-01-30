#pragma once

// Ensure proper linkage and calling convention for Windows API functions
#define WINGDIAPI __declspec(dllimport)
#define APIENTRY __stdcall
#include <GL/glew.h>


class Color {
public:
	Color(const int r, const int g, const int b, const int a = 255)
		: r(r), g(g), b(b), a(a) {}

	[[nodiscard]] float red()   const { return static_cast<float>(r) / 255.0f; }
	[[nodiscard]] float green() const { return static_cast<float>(g) / 255.0f; }
	[[nodiscard]] float blue()  const { return static_cast<float>(b) / 255.0f; }
	[[nodiscard]] float alpha() const { return static_cast<float>(a) / 255.0f; }

	[[nodiscard]] Color transparent(const float alpha) const {
		return {
			r, g, b, static_cast<int>(a * alpha)	 // NOLINT(*-narrowing-conversions)
		};
	}

	[[nodiscard]] GLfloat* toGLfloat() const { return new GLfloat[4]{ red(), green(), blue(), alpha() }; }

	static Color blendColors(const Color& color1, const Color& color2) {
		int alpha = static_cast<int>(255 - (255 - color2.alpha()) * (255 - color1.alpha()));

		return {
			(color2.r * color2.a / alpha + color1.r * color1.a * (1 - color2.a) / alpha),
			(color2.g * color2.a / alpha + color1.g * color1.a * (1 - color2.a) / alpha),
			(color2.b * color2.a / alpha + color1.b * color1.a * (1 - color2.a) / alpha),
			alpha
		};
	}

private:
	int r, g, b, a;
};

inline void clearColor(const Color& color) {
	glClearColor(color.red(), color.green(), color.blue(), color.alpha());
}

inline void color3f(const Color& color) {
	glColor3f(color.red(), color.green(), color.blue());
}
