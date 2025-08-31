#pragma once
#include <string>

void NebulaDrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a);

void NebulaDrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a);

void SetTextFont(int font);
void SetTextRightJustified();
void SetTextCentered(bool centered);

void DrawRect(float x, float y, float w, float h,
    int r, int g, int b, int a);

void DrawText(float x, float y, float scale,
    const std::string& text,
    int r, int g, int b, int a);

void DrawSprite(const char* textureDict, const char* textureName,
    float x, float y, float width, float height, float heading,
    int r, int g, int b, int a);

void DrawBanner(float x, float y, float w, float h);

void DrawNotification(const std::string& text);

void RequestTexture(const char* textureDict);
bool HasTextureLoaded(const char* textureDict);