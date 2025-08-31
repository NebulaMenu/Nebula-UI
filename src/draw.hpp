#pragma once
#include <string>

// Core drawing functions
void NebulaDrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a);

void NebulaDrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a);

// Font options
void SetTextFont(int font);
void SetTextRightJustified();
void SetTextCentered(bool centered);

// Enhanced drawing functions
void DrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a);

void DrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a);

// Special UI elements
void DrawSprite(const char* textureDict, const char* textureName,
    float x, float y, float width, float height, float heading,
    int r, int g, int b, int a);

void DrawBanner(float x, float y, float w, float h);

void DrawNotification(const std::string& text);

// Sprite management
void RequestTexture(const char* textureDict);
bool HasTextureLoaded(const char* textureDict);