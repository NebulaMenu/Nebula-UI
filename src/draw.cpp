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
