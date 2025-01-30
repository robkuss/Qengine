#pragma once

using namespace std;

#include <string>
#include <atomic>
#include <map>

#include <GL/glew.h>
#include <ft2build.h>

#include FT_FREETYPE_H

class Color;


// Struct to represent each character glyph
struct Character {
	GLuint textureID;		    // ID handle of the glyph texture
	unsigned int sizeX, sizeY;	// Size of glyph
	int bearingX, bearingY;     // Offset from baseline to left/top of glyph
	FT_Pos advance;			    // Offset to advance to next glyph
};

enum class TextMode {
	CENTER,
	LEFT,
	RIGHT
};

inline FT_Library library;
inline FT_Face font;
inline GLuint fontTexture;
inline map<char, Character> characters;

inline string nonFatalErrorText;


class Text {
public:
	constexpr static float fontScale   = 0.5f;
	constexpr static int   fontSize	   = 48;
	constexpr static float lineSpacing = 1.2f;

	Text();
	static void destruct();

	static void renderText(const string& text, TextMode textMode, float x, float y, int textSize, Color color);
	void setErrorText(const string& text);
	static void drawErrorText(int windowH);

	static float line(int lineNumber, int textSize);

private:
	constexpr static auto fontPath = "../resources/fonts/cour.ttf";

	double errorTime = 0.0;
	atomic<bool> errorTimerRunning = false;
};
