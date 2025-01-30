#include "Text.h"

#include <iostream>

#include "graphics/color/Colors.h"
#include "graphics/ui/UI.h"


// Function to initialize FreeType and load the font
Text::Text() {
    // Initialize the FreeType library
    if (FT_Init_FreeType(&library)) {
        cerr << "ERROR: Could not initialize FreeType library" << endl;
        exit(-1);
    }

    // Load the font face
    if (FT_New_Face(library, fontPath, 0, &font)) {
        cerr << "ERROR: Could not load font " << fontPath << endl;
        exit(-1);
    }

    // Set the font size
    FT_Set_Pixel_Sizes(font, 0, fontSize);

    // Load the first 128 ASCII characters
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(font, c, FT_LOAD_RENDER)) {
            cerr << "ERROR: Failed to load Glyph " << c << endl;
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
        characters.insert(pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::destruct() {
    // Cleanup
    if (font) {
        FT_Done_Face(font);
        font = nullptr;     // Set to nullptr to avoid dangling pointer
    }

    if (library) {
        FT_Done_FreeType(library);
        library = nullptr;  // Set to nullptr to indicate it's freed
    }

    if (fontTexture) {
        glDeleteTextures(1, &fontTexture);
        fontTexture = 0;    // Set to 0 to avoid accidental reuse
    }
}


/** Line number to screen coordinates */
float Text::line(const int lineNumber, const int textSize) {
    return UI::firstLineY + static_cast<float>(lineNumber) * static_cast<float>(textSize) * lineSpacing;
}

/**
 * Draw UI Text somewhere on the screen
 * @param text the String that will be rendered to the screen
 * @param textMode center, left, or right
 * @param x the x position in screen coordinates where the text will be drawn
 * @param y the y position in screen coordinates where the text will be drawn
 * @param textSize font size for this particular text
 * @param color the color that the text will be drawn in
 */
void Text::renderText(const string& text, TextMode textMode, const float x, const float y, const int textSize, const Color color) {
    // Save OpenGL state
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTexture);

    color3f(color);

    // Switch to the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Move to the position where the text should be drawn
    glTranslatef(x, y, 0.0f);

    const float scalar = static_cast<float>(textSize) / static_cast<float>(fontSize);
    glScalef(scalar, scalar, 1.0f);

    // Render the text character by character
    for (int i = 0; text[i] != '\0'; ++i) {
        char c = text[i];
        auto it = characters.find(c);
        if (it == characters.end()) {
            cerr << "Character " << c << " not found in the map" << endl;
            continue;
        }
        auto&[textureID, sizeX, sizeY, bearingX, bearingY, advance] = it->second;

        // Correct kerning by considering the glyph's bearing.x
        const auto xpos = static_cast<float>(bearingX);
        const auto ypos = static_cast<float>(sizeY) - static_cast<float>(bearingY);

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

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(xpos, ypos);

        glEnd();

        // Move the cursor to the next position (advance by glyph's advance value)
        glTranslatef(static_cast<float>(advance) / 64.0f, 0.0f, 0.0f);
    }

    // Restore the model view matrix
    glPopMatrix();

    // Restore OpenGL state
    glPopAttrib();
}

/**
 * Set a non-fatal error text to be displayed at the bottom of the screen.
 * The text will disappear after 3 seconds.
 */
void Text::setErrorText(const string& text) {
    nonFatalErrorText = text;

    if (!errorTimerRunning) {
        errorTimerRunning = true;

        // Launch a thread to reset the errorText after 3 seconds
        thread([this] {
            this_thread::sleep_for(chrono::seconds(3));
            nonFatalErrorText = "";
            errorTimerRunning = false;
        }).detach(); // Detach the thread so it runs independently
    }
}

/** Draw a non-fatal error text at the bottom of the screen */
void Text::drawErrorText(const int windowH) {
    if (!nonFatalErrorText.empty())
        renderText(
            nonFatalErrorText,
            TextMode::LEFT,
            UI::firstLineX,
            static_cast<float>(windowH) - UI::bottomLineY,
            fontSize,
            Colors::RED
        );
}
