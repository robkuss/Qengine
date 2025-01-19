#pragma once

#include <map>
#include <fstream>

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <viewport/scene/graphics/color/Colors.h>

// Struct to represent each character glyph
struct Character {
	GLuint textureID;		    // ID handle of the glyph texture
	unsigned int sizeX, sizeY;	// Size of glyph
	int bearingX, bearingY;     // Offset from baseline to left/top of glyph
	FT_Pos advance;			    // Offset to advance to next glyph
};

class Text {
public:
	constexpr static float fontScale   = 0.5f;

	Text();
	~Text();

	void renderText(const char *text, float x, float y, Color color);
	void setErrorText(const std::string& text);
	void drawErrorText(int windowH);

	static float line(int lineNumber);

private:
	constexpr static auto  fontPath	   = "../resources/fonts/cour.ttf";
	constexpr static int   fontSize	   = 48;
	constexpr static float lineSpacing = 1.2f;

	FT_Library library{};
	FT_Face font{};
	GLuint fontTexture{};
	std::map<char, Character> characters;

	std::string nonFatalErrorText;
	double errorTime = 0.0;
	std::atomic<bool> errorTimerRunning = false;
};
