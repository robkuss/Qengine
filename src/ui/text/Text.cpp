#ifndef TEXT_C
#define TEXT_C

#include <iostream>
#include <map>
#include <fstream>

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../../scene/graphics/color/Color.h"


// Define a struct to represent each character glyph (similar to Kotlin's Character class)
struct Character {
	GLuint textureID;		    // ID handle of the glyph texture
	unsigned int sizeX, sizeY;	// Size of glyph
    int bearingX, bearingY;     // Offset from baseline to left/top of glyph
	FT_Pos advance;			    // Offset to advance to next glyph
};

class Text {
public:
    static constexpr float firstLineX = 10.0f;
    static constexpr float firstLineY = 45.0f;
    static constexpr float bottomLineY = 10.0f;
    static constexpr float lineSpacing = 1.2f;
    static constexpr float fontScale = 0.5f;
    std::string nonFatalErrorText;

    double errorTime = 0.0;

    Text();
    ~Text();

    void initFreeType();
    void renderText(const char *text, float x, float y, int windowW, int windowH, Color color);
    void setErrorText(const std::string& text);
    void drawErrorText(int windowW, int windowH);

    static float line(int lineNumber);

private:
    static constexpr auto fontPath = "C:/Users/rober/OneDrive/Qengine/Qengine/resources/fonts/cour.ttf";
    static constexpr int fontSize = 48;

    FT_Library library{};
    FT_Face font{};
    GLuint fontTexture{};
    std::map<char, Character> characters;

    std::atomic<bool> errorTimerRunning = false;
};


// Function to initialize FreeType and load the font
inline Text::Text() {
    // Initialize the FreeType library
    if (FT_Init_FreeType(&library)) {
        std::cerr << "ERROR: Could not initialize FreeType library" << std::endl;
        exit(-1);
    }

    // Load the font face
    if (FT_New_Face(library, fontPath, 0, &font)) {
        std::cerr << "ERROR: Could not load font " << fontPath << std::endl;
        exit(-1);
    }

    // Set the font size
    FT_Set_Pixel_Sizes(font, 0, fontSize);

    // Load the first 128 ASCII characters
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(font, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR: Failed to load Glyph " << c << std::endl;
            continue;
        }

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,
            static_cast<int>(font->glyph->bitmap.width),
            static_cast<int>(font->glyph->bitmap.rows),
            0,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            font->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Store character information for later use
        Character character = {
            texture,
            font->glyph->bitmap.width,
            font->glyph->bitmap.rows,
            font->glyph->bitmap_left,
            font->glyph->bitmap_top,
            font->glyph->advance.x
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline Text::~Text() {
    if (font) {
        FT_Done_Face(font);
        font = nullptr;     // Optional: set to nullptr to avoid dangling pointer
    }
    // Free the library handle if it was initialized
    if (library) {
        FT_Done_FreeType(library);
        library = nullptr;  // Optional: set to nullptr to indicate it's freed
    }
    if (fontTexture) {
        glDeleteTextures(1, &fontTexture);
        fontTexture = 0;    // Optional: set to 0 to avoid accidental reuse
    }
}


/** Line number to screen coordinates */
inline float Text::line(const int lineNumber) {
    return firstLineY * fontScale + static_cast<float>(lineNumber) * fontSize * fontScale * lineSpacing;
}

/**
 * Draw UI Text somewhere on the screen
 * @param text the String that will be rendered to the screen
 * @param x the x position in screen coordinates where the text will be drawn
 * @param y the y position in screen coordinates where the text will be drawn
 * @param windowW the width of the window
 * @param windowH the height of the window
 * @param color the color that the text will be drawn in
 */
inline void Text::renderText(const char* text, const float x, const float y, const int windowW, const int windowH, const Color color) {
    // Save the current matrix state (3D perspective matrix)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glOrtho(0, windowW, windowH, 0, -1, 1);

    // Switch to the model view matrix to render the text
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Enable necessary settings for text rendering
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    color3f(color);

    // Move to the position where the text should be drawn
    glTranslatef(x, y, 0.0f);
    glScalef(fontScale, fontScale, 1.0f);

    // Render the text character by character
    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        auto it = characters.find(c);
        if (it == characters.end()) continue;  // Skip to the next iteration if the character is not in the map
        auto&[textureID, sizeX, sizeY, bearingX, bearingY, advance] = it->second;

        // Correct kerning by considering the glyph's bearing.x
        const auto xpos = static_cast<float>(bearingX);
        const auto ypos = static_cast<float>(sizeY - bearingY);

        // Bind the character's texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Render the character quad with the glyph texture
        glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(xpos, ypos - static_cast<float>(sizeY));

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(xpos + static_cast<float>(sizeX), ypos - static_cast<float>(sizeY));

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(xpos + static_cast<float>(sizeX), ypos);

        glTexCoord2f(0.0, 1.0);
        glVertex2f(xpos, ypos);

        glEnd();

        // Move the cursor to the next position (advance by glyph's advance value)
        glTranslatef(static_cast<float>(advance) / 64, 0.0f, 0.0f);
    }

    // Restore settings
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    // Restore the model view matrix
    glPopMatrix();

    // Restore the projection matrix (back to 3D perspective)
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // Switch back to the model view matrix
    glMatrixMode(GL_MODELVIEW);
}

/**
 * Set a non-fatal error text to be displayed at the bottom of the screen.
 * The text will disappear after 3 seconds.
 */
inline void Text::setErrorText(const std::string& text) {
    nonFatalErrorText = text;

    if (!errorTimerRunning) {
        errorTimerRunning = true;

        // Launch a thread to reset the errorText after 3 seconds
        std::thread([this] {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            nonFatalErrorText = "";
            errorTimerRunning = false;
        }).detach(); // Detach the thread so it runs independently
    }
}

/** Draw a non-fatal error text at the bottom of the screen */
inline void Text::drawErrorText(const int windowW, const int windowH) {
    if (!nonFatalErrorText.empty())
        renderText(nonFatalErrorText.c_str(), firstLineX, static_cast<float>(windowH) - bottomLineY, windowW, windowH, RED);
}


#endif // TEXT_C