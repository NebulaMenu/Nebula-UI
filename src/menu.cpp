#include "menu.hpp"
#include "draw.hpp"
#include "script.h"
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

void Menu::AddAction(const std::string& label, std::function<void()> action) {
    items.push_back({ label, MenuItemType::Action, action });
}

void Menu::AddToggle(const std::string& label, bool* state) {
    items.push_back({ label, MenuItemType::Toggle, nullptr, state });
}

void Menu::AddSubmenu(const std::string& label, std::shared_ptr<Menu> submenu) {
    items.push_back({ label, MenuItemType::Submenu, nullptr, nullptr, submenu });
}

void Menu::AddNumber(const std::string& label, int* value, int min, int max, int step) {
    MenuItem item;
    item.label = label;
    item.type = MenuItemType::NumberOption;
    item.intValue = value;
    item.minInt = min;
    item.maxInt = max;
    item.stepInt = step;
    item.isFloat = false;
    items.push_back(item);
}

void Menu::AddNumber(const std::string& label, float* value, float min, float max, float step) {
    MenuItem item;
    item.label = label;
    item.type = MenuItemType::NumberOption;
    item.floatValue = value;
    item.minFloat = min;
    item.maxFloat = max;
    item.stepFloat = step;
    item.isFloat = true;
    items.push_back(item);
}

std::shared_ptr<Menu> Menu::AddFolder(const std::string& label) {
    auto sub = std::make_shared<Menu>(label);
    AddSubmenu(label, sub);
    return sub;
}

std::shared_ptr<Menu> Menu::AddFolder(const std::string& label,
    const std::function<void(std::shared_ptr<Menu>)>& build) {
    auto sub = std::make_shared<Menu>(label);
    AddSubmenu(label, sub);
    if (build) build(sub);
    return sub;
}

void Menu::DrawHeader() {
    float x = style.x;
    float y = style.y;

    DrawBanner(x, y, style.width, style.headerHeight);

    char counter[32];
    snprintf(counter, sizeof(counter), "%d/%d", selected + 1, (int)items.size());

    UI::SET_TEXT_FONT(4);
    UI::SET_TEXT_SCALE(0.35f, 0.35f);
    UI::SET_TEXT_COLOUR(200, 200, 200, 255);
    UI::SET_TEXT_RIGHT_JUSTIFY(true);
    UI::SET_TEXT_WRAP(0.0f, x + style.width / 2 - 0.005f);
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING(counter);
    UI::_DRAW_TEXT(x + style.width / 2 - 0.005f, y + style.headerHeight / 2 - 0.025f);
}

void Menu::DrawSelection() {
    if (items.empty()) return;

    int visibleIndex = selected - scrollOffset;
    if (visibleIndex < 0 || visibleIndex >= maxDisplay) return;

    float x = style.x;
    float y = style.y + style.headerHeight / 2 + 0.002f + visibleIndex * style.itemHeight;

    DrawRect(x, y + style.itemHeight / 2, style.width, style.itemHeight,
        style.selection.r, style.selection.g, style.selection.b, style.selection.a);
}

void Menu::DrawItems() {
    float x = style.x;
    float startY = style.y + style.headerHeight / 2 + 0.002f;

    int endItem = std::min(scrollOffset + maxDisplay, (int)items.size());

    for (int i = scrollOffset; i < endItem; i++) {
        int drawIndex = i - scrollOffset;
        float itemY = startY + drawIndex * style.itemHeight;

        const auto& item = items[i];
        bool isSelected = (i == selected);

        int tr = isSelected ? style.selectedText.r : style.text.r;
        int tg = isSelected ? style.selectedText.g : style.text.g;
        int tb = isSelected ? style.selectedText.b : style.text.b;
        int ta = isSelected ? style.selectedText.a : style.text.a;

        UI::SET_TEXT_FONT(4);
        UI::SET_TEXT_SCALE(0.35f, 0.35f);
        UI::SET_TEXT_COLOUR(tr, tg, tb, ta);
        UI::SET_TEXT_CENTRE(false);
        UI::_SET_TEXT_ENTRY((char*)"STRING");
        UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(item.label.c_str()));
        UI::_DRAW_TEXT(x - style.width / 2 + 0.005f, itemY);

        float rightX = x + style.width / 2 - 0.005f;

        switch (item.type) {
        case MenuItemType::Toggle:
            if (item.toggleState) {
                const char* state = *item.toggleState ? "ON" : "OFF";
                int sr = *item.toggleState ? style.toggleOn.r : style.toggleOff.r;
                int sg = *item.toggleState ? style.toggleOn.g : style.toggleOff.g;
                int sb = *item.toggleState ? style.toggleOn.b : style.toggleOff.b;

                UI::SET_TEXT_FONT(4);
                UI::SET_TEXT_SCALE(0.35f, 0.35f);
                UI::SET_TEXT_COLOUR(sr, sg, sb, 255);
                UI::SET_TEXT_RIGHT_JUSTIFY(true);
                UI::SET_TEXT_WRAP(0.0f, rightX);
                UI::_SET_TEXT_ENTRY((char*)"STRING");
                UI::_ADD_TEXT_COMPONENT_STRING((char*)state);
                UI::_DRAW_TEXT(rightX, itemY);
            }
            break;

        case MenuItemType::Submenu:
            UI::SET_TEXT_FONT(4);
            UI::SET_TEXT_SCALE(0.35f, 0.35f);
            UI::SET_TEXT_COLOUR(tr, tg, tb, ta);
            UI::SET_TEXT_RIGHT_JUSTIFY(true);
            UI::SET_TEXT_WRAP(0.0f, rightX);
            UI::_SET_TEXT_ENTRY((char*)"STRING");
            UI::_ADD_TEXT_COMPONENT_STRING((char*)">");
            UI::_DRAW_TEXT(rightX, itemY);
            break;

        case MenuItemType::NumberOption:
        {
            std::stringstream ss;
            if (item.isFloat && item.floatValue) {
                ss << std::fixed << std::setprecision(1) << *item.floatValue;
            }
            else if (item.intValue) {
                ss << *item.intValue;
            }

            std::string valueStr = "< " + ss.str() + " >";

            UI::SET_TEXT_FONT(4);
            UI::SET_TEXT_SCALE(0.35f, 0.35f);
            UI::SET_TEXT_COLOUR(tr, tg, tb, ta);
            UI::SET_TEXT_RIGHT_JUSTIFY(true);
            UI::SET_TEXT_WRAP(0.0f, rightX);
            UI::_SET_TEXT_ENTRY((char*)"STRING");
            UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(valueStr.c_str()));
            UI::_DRAW_TEXT(rightX, itemY);
        }
        break;
        }
    }
}

void Menu::DrawFooter() {
    float x = style.x;
    float footerY = style.y + style.headerHeight / 2 + 0.002f + maxDisplay * style.itemHeight;

    DrawRect(x, footerY + style.footerHeight / 2, style.width, style.footerHeight,
        style.footer.r, style.footer.g, style.footer.b, style.footer.a);

    UI::SET_TEXT_FONT(4);
    UI::SET_TEXT_SCALE(0.3f, 0.3f);
    UI::SET_TEXT_COLOUR(200, 200, 200, 255);
    UI::SET_TEXT_CENTRE(true);
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((char*)"Navigate: ~c~↑ ↓~s~    Select: ~c~Enter~s~    Back: ~c~Backspace");
    UI::_DRAW_TEXT(x, footerY + 0.008f);
}

void Menu::DrawScrollIndicator() {
    if (items.size() <= maxDisplay) return;

    float x = style.x + style.width / 2 + 0.005f;
    float startY = style.y + style.headerHeight / 2 + 0.002f;
    float scrollHeight = maxDisplay * style.itemHeight;

    DrawRect(x, startY + scrollHeight / 2, 0.002f, scrollHeight, 40, 40, 40, 160);

    float thumbHeight = (float)maxDisplay / items.size() * scrollHeight;
    float thumbProgress = (float)scrollOffset / (items.size() - maxDisplay);
    float thumbY = startY + thumbProgress * (scrollHeight - thumbHeight) + thumbHeight / 2;

    DrawRect(x, thumbY, 0.003f, thumbHeight, 255, 255, 255, 200);
}

void Menu::Render() {
    float bgY = style.y + style.headerHeight / 2 + 0.002f + (maxDisplay * style.itemHeight) / 2;
    float bgHeight = maxDisplay * style.itemHeight;

    DrawRect(style.x, bgY, style.width, bgHeight,
        style.background.r, style.background.g, style.background.b, style.background.a);

    DrawHeader();
    DrawSelection();
    DrawItems();
    DrawFooter();
    DrawScrollIndicator();
}

void Menu::Up() {
    if (items.empty()) return;

    selected--;
    if (selected < 0) {
        selected = (int)items.size() - 1;
        scrollOffset = std::max(0, (int)items.size() - maxDisplay);
    }

    if (selected < scrollOffset) {
        scrollOffset = selected;
    }
}

void Menu::Down() {
    if (items.empty()) return;

    selected++;
    if (selected >= (int)items.size()) {
        selected = 0;
        scrollOffset = 0;
    }

    if (selected >= scrollOffset + maxDisplay) {
        scrollOffset = selected - maxDisplay + 1;
    }
}

void Menu::Left() {
    if (items.empty()) return;
    auto& item = items[selected];

    if (item.type == MenuItemType::NumberOption) {
        if (item.isFloat && item.floatValue) {
            *item.floatValue = std::max(item.minFloat, *item.floatValue - item.stepFloat);
        }
        else if (item.intValue) {
            *item.intValue = std::max(item.minInt, *item.intValue - item.stepInt);
        }
    }
}

void Menu::Right() {
    if (items.empty()) return;
    auto& item = items[selected];

    if (item.type == MenuItemType::NumberOption) {
        if (item.isFloat && item.floatValue) {
            *item.floatValue = std::min(item.maxFloat, *item.floatValue + item.stepFloat);
        }
        else if (item.intValue) {
            *item.intValue = std::min(item.maxInt, *item.intValue + item.stepInt);
        }
    }
}

std::shared_ptr<Menu> Menu::Select() {
    if (items.empty()) return nullptr;
    auto& item = items[selected];

    switch (item.type) {
    case MenuItemType::Action:
        if (item.action) item.action();
        break;

    case MenuItemType::Toggle:
        if (item.toggleState) *item.toggleState = !*item.toggleState;
        break;

    case MenuItemType::Submenu:
        if (item.submenu) {
            item.submenu->StartOpenAnimation();
            return item.submenu;
        }
        break;
    }

    return nullptr;
}

MenuItemType Menu::CurrentType() const {
    if (items.empty()) return MenuItemType::Action;
    return items[selected].type;
}

std::shared_ptr<Menu> Menu::CurrentSubmenu() const {
    if (items.empty()) return nullptr;
    return items[selected].submenu;
}