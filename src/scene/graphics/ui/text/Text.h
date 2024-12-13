#pragma once

#include <map>
#include <fstream>

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <scene/graphics/color/Colors.h>

// Struct to represent each character glyph
struct Character {
	GLuint textureID;		    // ID handle of the glyph texture
	unsigned int sizeX, sizeY;	// Size of glyph
	int bearingX, bearingY;     // Offset from baseline to left/top of glyph
	FT_Pos advance;			    // Offset to advance to next glyph
};

class Text {
public:
	static constexpr float firstLineX  = 10.0f;
	static constexpr float firstLineY  = 45.0f;
	static constexpr float bottomLineY = 10.0f;
	static constexpr float lineSpacing = 1.2f;
	static constexpr float fontScale   = 0.5f;
	std::string nonFatalErrorText;

	double errorTime = 0.0;

	Text();
	~Text();

	void renderText(const char *text, float x, float y, int windowW, int windowH, Color color);
	void setErrorText(const std::string& text);
	void drawErrorText(int windowW, int windowH);

	static float line(int lineNumber);

private:
	static constexpr auto fontPath = "../resources/fonts/cour.ttf";
	static constexpr int fontSize = 48;

	FT_Library library{};
	FT_Face font{};
	GLuint fontTexture{};
	std::map<char, Character> characters;

	std::atomic<bool> errorTimerRunning = false;
};
