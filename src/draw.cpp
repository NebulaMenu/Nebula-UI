#include "draw.hpp"
#include "script.h"

void NebulaDrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a) {
    GRAPHICS::DRAW_RECT(x, y, w, h, r, g, b, a);
}

void NebulaDrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a) {
    UI::SET_TEXT_FONT(0);
    UI::SET_TEXT_SCALE(scale, scale);
    UI::SET_TEXT_COLOUR(r, g, b, a);
    UI::SET_TEXT_WRAP(0.0f, 1.0f);
    UI::SET_TEXT_JUSTIFICATION(1);
    UI::SET_TEXT_OUTLINE();
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(text.c_str()));
    UI::_DRAW_TEXT(x, y);
}

void SetTextFont(int font) {
    UI::SET_TEXT_FONT(font);
}

void SetTextRightJustified() {
    UI::SET_TEXT_JUSTIFICATION(2);
}

void SetTextCentered(bool centered) {
    UI::SET_TEXT_CENTRE(centered);
}

void DrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a) {
    GRAPHICS::DRAW_RECT(x, y, w, h, r, g, b, a);
}

void DrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a) {
    UI::SET_TEXT_FONT(4); // Font 4 for clean look
    UI::SET_TEXT_SCALE(scale, scale);
    UI::SET_TEXT_COLOUR(r, g, b, a);
    UI::SET_TEXT_CENTRE(false);
    UI::SET_TEXT_OUTLINE();
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(text.c_str()));
    UI::_DRAW_TEXT(x, y);
}

void DrawSprite(const char* textureDict, const char* textureName,
    float x, float y, float width, float height, float heading,
    int r, int g, int b, int a) {

    if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((char*)textureDict)) {
        GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)textureDict, false);
    }

    GRAPHICS::DRAW_SPRITE((char*)textureDict, (char*)textureName,
        x, y, width, height, heading, r, g, b, a);
}

void DrawBanner(float x, float y, float w, float h) {
    DrawRect(x, y, w, h, 15, 15, 15, 240);

    DrawRect(x, y - h / 2 + 0.001f, w, 0.002f, 255, 255, 255, 100);

    DrawRect(x, y + h / 2 - 0.001f, w, 0.002f, 255, 255, 255, 100);

    // Title text
    UI::SET_TEXT_FONT(1);
    UI::SET_TEXT_SCALE(0.8f, 0.8f);
    UI::SET_TEXT_COLOUR(255, 255, 255, 255);
    UI::SET_TEXT_CENTRE(true);
    UI::SET_TEXT_OUTLINE();
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((char*)"NEBULA");
    UI::_DRAW_TEXT(x, y - 0.025f);

    // Version text
    UI::SET_TEXT_FONT(4);
    UI::SET_TEXT_SCALE(0.28f, 0.28f);
    UI::SET_TEXT_COLOUR(200, 200, 200, 255);
    UI::SET_TEXT_CENTRE(true);
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((char*)"VERSION 0.0.1");
    UI::_DRAW_TEXT(x, y + 0.01f);
}

void DrawNotification(const std::string& text) {
    UI::_SET_NOTIFICATION_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(text.c_str()));
    UI::_DRAW_NOTIFICATION(false, true);
}

void RequestTexture(const char* textureDict) {
    GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((char*)textureDict, false);
}

bool HasTextureLoaded(const char* textureDict) {
    return GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((char*)textureDict);
}