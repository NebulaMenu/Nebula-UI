#pragma once
#include <string>

void NebulaDrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a);

void NebulaDrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a);
