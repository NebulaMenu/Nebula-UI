// menu.cpp
#include "menu.hpp"
#include "draw.hpp"
#include "script.h"
#include <iomanip>
#include <sstream>
#include <algorithm>

static inline void PlayMenuSound(const char* soundName, const char* soundSet = "HUD_FRONTEND_DEFAULT_SOUNDSET") {
    AUDIO::PLAY_SOUND_FRONTEND(-1, (char*)soundName, (char*)soundSet, false);
}

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
void Menu::AddText(const std::string& label) {
    MenuItem item;
    item.label = label;
    item.type = MenuItemType::TextOption;
    items.push_back(item);
}
void Menu::AddSeparator(const std::string& label) {
    MenuItem item;
    item.label = label;
    item.type = MenuItemType::Separator;
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

Menu::MenuStyle::MenuStyle() {
    x = 0.8f;
    y = 0.15f;
    width = 0.20f;
    headerHeight = 0.08f;
    itemHeight = 0.035f;
    footerHeight = 0.030f;
    listTopGap = 0.010f;

    background = { 10, 10, 10, 230 };
    header = { 15, 15, 15, 255 };
    footer = { 15, 15, 15, 255 };
    selection = { 255, 255, 255, 30 };
    text = { 255, 255, 255, 255 };
    selectedText = { 255, 255, 255, 255 };
    disabledText = { 150, 150, 150, 255 };
    toggleOn = { 100, 255, 100, 255 };
    toggleOff = { 255, 100, 100, 255 };
}

bool Menu::isSelectable(const MenuItem& it) const {
    return !(it.type == MenuItemType::Separator || it.type == MenuItemType::TextOption);
}
int Menu::selectableCount() const {
    int c = 0;
    for (auto& it : items) if (isSelectable(it)) ++c;
    return c;
}
int Menu::findNextSelectable(int from, int step) const {
    if (items.empty()) return -1;
    if (selectableCount() == 0) return -1;
    int idx = from;
    for (int i = 0; i < (int)items.size(); ++i) {
        idx = (idx + step + (int)items.size()) % (int)items.size();
        if (isSelectable(items[idx])) return idx;
    }
    return -1;
}
int Menu::indexInSelectableList(int absoluteIndex) const {
    if (absoluteIndex < 0 || absoluteIndex >= (int)items.size()) return -1;
    if (!isSelectable(items[absoluteIndex])) return -1;
    int rank = 0;
    for (int i = 0; i < absoluteIndex; ++i) if (isSelectable(items[i])) ++rank;
    return rank;
}

void Menu::DrawHeader() {
    float x = style.x;
    float y = style.y;

    DrawBanner(x, y, style.width, style.headerHeight);

    int totalSel = selectableCount();
    int selOrd = -1;
    if (!items.empty() && selected >= 0 && selected < (int)items.size()) {
        selOrd = indexInSelectableList(selected);
    }

    char counter[32];
    if (totalSel > 0 && selOrd >= 0) snprintf(counter, sizeof(counter), "%d/%d", selOrd + 1, totalSel);
    else snprintf(counter, sizeof(counter), "-/-");

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
    if (!isSelectable(items[selected])) return;

    int visibleIndex = selected - scrollOffset;
    if (visibleIndex < 0 || visibleIndex >= maxDisplay) return;

    float x = style.x;
    float y = style.y + style.headerHeight / 2 + style.listTopGap + visibleIndex * style.itemHeight;

    DrawRect(x, y + style.itemHeight / 2, style.width, style.itemHeight,
        style.selection.r, style.selection.g, style.selection.b, style.selection.a);
}

void Menu::DrawItems() {
    float x = style.x;
    float startY = style.y + style.headerHeight / 2 + style.listTopGap;

    int endItem = std::min(scrollOffset + maxDisplay, (int)items.size());

    for (int i = scrollOffset; i < endItem; i++) {
        int drawIndex = i - scrollOffset;
        float itemY = startY + drawIndex * style.itemHeight;

        const auto& item = items[i];
        bool isSelectedRow = (i == selected) && isSelectable(item);

        int tr = isSelectedRow ? style.selectedText.r : style.text.r;
        int tg = isSelectedRow ? style.selectedText.g : style.text.g;
        int tb = isSelectedRow ? style.selectedText.b : style.text.b;
        int ta = isSelectedRow ? style.selectedText.a : style.text.a;

        if (item.type == MenuItemType::Separator) {
            UI::SET_TEXT_FONT(4);
            UI::SET_TEXT_SCALE(0.33f, 0.33f);
            UI::SET_TEXT_COLOUR(style.disabledText.r, style.disabledText.g, style.disabledText.b, style.disabledText.a);
            UI::SET_TEXT_CENTRE(true);
            UI::_SET_TEXT_ENTRY((char*)"STRING");
            UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(item.label.c_str()));
            UI::_DRAW_TEXT(x, itemY + style.itemHeight * 0.35f);
            continue;
        }
        if (item.type == MenuItemType::TextOption) {
            UI::SET_TEXT_FONT(4);
            UI::SET_TEXT_SCALE(0.33f, 0.33f);
            UI::SET_TEXT_COLOUR(style.disabledText.r, style.disabledText.g, style.disabledText.b, style.disabledText.a);
            UI::SET_TEXT_CENTRE(false);
            UI::_SET_TEXT_ENTRY((char*)"STRING");
            UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(item.label.c_str()));
            UI::_DRAW_TEXT(x - style.width / 2 + 0.005f, itemY);
            continue;
        }

        UI::SET_TEXT_FONT(4);
        UI::SET_TEXT_SCALE(0.35f, 0.35f);
        UI::SET_TEXT_COLOUR(tr, tg, tb, ta);
        UI::SET_TEXT_CENTRE(false);
        UI::_SET_TEXT_ENTRY((char*)"STRING");
        UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(item.label.c_str()));
        UI::_DRAW_TEXT(x - style.width / 2 + 0.005f, itemY);

        float rightX = x + style.width / 2 - 0.005f;

        switch (item.type) {
        case MenuItemType::Toggle: {
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
        }
        case MenuItemType::Submenu: {
            UI::SET_TEXT_FONT(4);
            UI::SET_TEXT_SCALE(0.35f, 0.35f);
            UI::SET_TEXT_COLOUR(tr, tg, tb, ta);
            UI::SET_TEXT_RIGHT_JUSTIFY(true);
            UI::SET_TEXT_WRAP(0.0f, rightX);
            UI::_SET_TEXT_ENTRY((char*)"STRING");
            UI::_ADD_TEXT_COMPONENT_STRING((char*)">");
            UI::_DRAW_TEXT(rightX, itemY);
            break;
        }
        case MenuItemType::NumberOption: {
            std::stringstream ss;
            if (item.isFloat && item.floatValue) ss << std::fixed << std::setprecision(1) << *item.floatValue;
            else if (item.intValue)             ss << *item.intValue;
            std::string valueStr = "< " + ss.str() + " >";
            UI::SET_TEXT_FONT(4);
            UI::SET_TEXT_SCALE(0.35f, 0.35f);
            UI::SET_TEXT_COLOUR(tr, tg, tb, ta);
            UI::SET_TEXT_RIGHT_JUSTIFY(true);
            UI::SET_TEXT_WRAP(0.0f, rightX);
            UI::_SET_TEXT_ENTRY((char*)"STRING");
            UI::_ADD_TEXT_COMPONENT_STRING(const_cast<char*>(valueStr.c_str()));
            UI::_DRAW_TEXT(rightX, itemY);
            break;
        }
        default:
            break;
        }
    }
}

void Menu::DrawFooter() {
    float x = style.x;
    float footerY = style.y + style.headerHeight / 2 + style.listTopGap + maxDisplay * style.itemHeight;

    DrawRect(x, footerY + style.footerHeight / 2, style.width, style.footerHeight,
        style.footer.r, style.footer.g, style.footer.b, style.footer.a);

    UI::SET_TEXT_FONT(4);
    UI::SET_TEXT_SCALE(0.3f, 0.3f);
    UI::SET_TEXT_COLOUR(200, 200, 200, 255);
    UI::SET_TEXT_CENTRE(true);
    UI::_SET_TEXT_ENTRY((char*)"STRING");
    UI::_ADD_TEXT_COMPONENT_STRING((char*)"Navigate: ~c~UP/DOWN~s~  Select: ~c~Enter~s~  Back: ~c~Backspace");
    UI::_DRAW_TEXT(x, footerY + 0.008f);
}

void Menu::DrawScrollIndicator() {
    if (items.size() <= maxDisplay) return;

    float x = style.x + style.width / 2 + 0.005f;
    float startY = style.y + style.headerHeight / 2 + style.listTopGap;
    float scrollHeight = maxDisplay * style.itemHeight;

    DrawRect(x, startY + scrollHeight / 2, 0.002f, scrollHeight, 40, 40, 40, 160);

    float thumbHeight = (float)maxDisplay / items.size() * scrollHeight;
    float thumbProgress = (float)scrollOffset / (items.size() - maxDisplay);
    float thumbY = startY + thumbProgress * (scrollHeight - thumbHeight) + thumbHeight / 2;

    DrawRect(x, thumbY, 0.003f, thumbHeight, 255, 255, 255, 200);
}

void Menu::Render() {
    float bgY = style.y + style.headerHeight / 2 + style.listTopGap + (maxDisplay * style.itemHeight) / 2;
    float bgHeight = maxDisplay * style.itemHeight;

    DrawRect(style.x, bgY, style.width, bgHeight,
        style.background.r, style.background.g, style.background.b, style.background.a);

    DrawHeader();
    DrawSelection();
    DrawItems();
    DrawFooter();
    DrawScrollIndicator();
}

void Menu::AdjustScrollForTop() {
    if (scrollOffset <= 0 || items.empty()) return;
    if (selected < 2) { scrollOffset = 0; return; }
    int prev = scrollOffset - 1;
    if (prev >= 0 && items[prev].type == MenuItemType::Separator) {
        scrollOffset = prev;
    }
}

void Menu::Up() {
    if (items.empty() || selectableCount() == 0) return;
    int next = findNextSelectable(selected, -1);
    if (next < 0 || next == selected) return;

    selected = next;

    if (selected < scrollOffset) {
        scrollOffset = selected;
    }
    else if (selected >= scrollOffset + maxDisplay) {
        scrollOffset = std::max(0, selected - maxDisplay + 1);
    }

    AdjustScrollForTop();
    PlayMenuSound("NAV_UP_DOWN");
}

void Menu::Down() {
    if (items.empty() || selectableCount() == 0) return;
    int next = findNextSelectable(selected, +1);
    if (next < 0 || next == selected) return;

    selected = next;

    if (selected < scrollOffset) {
        scrollOffset = selected;
    }
    else if (selected >= scrollOffset + maxDisplay) {
        scrollOffset = std::max(0, selected - maxDisplay + 1);
    }

    AdjustScrollForTop();
    PlayMenuSound("NAV_UP_DOWN");
}

void Menu::Left() {
    if (items.empty()) return;
    auto& item = items[selected];
    if (item.type == MenuItemType::NumberOption) {
        if (item.isFloat && item.floatValue) {
            float before = *item.floatValue;
            *item.floatValue = std::max(item.minFloat, *item.floatValue - item.stepFloat);
            if (*item.floatValue != before) PlayMenuSound("NAV_UP_DOWN");
        }
        else if (item.intValue) {
            int before = *item.intValue;
            *item.intValue = std::max(item.minInt, *item.intValue - item.stepInt);
            if (*item.intValue != before) PlayMenuSound("NAV_UP_DOWN");
        }
    }
}
void Menu::Right() {
    if (items.empty()) return;
    auto& item = items[selected];
    if (item.type == MenuItemType::NumberOption) {
        if (item.isFloat && item.floatValue) {
            float before = *item.floatValue;
            *item.floatValue = std::min(item.maxFloat, *item.floatValue + item.stepFloat);
            if (*item.floatValue != before) PlayMenuSound("NAV_UP_DOWN");
        }
        else if (item.intValue) {
            int before = *item.intValue;
            *item.intValue = std::min(item.maxInt, *item.intValue + item.stepInt);
            if (*item.intValue != before) PlayMenuSound("NAV_UP_DOWN");
        }
    }
}
std::shared_ptr<Menu> Menu::Select() {
    if (items.empty()) return nullptr;
    auto& item = items[selected];

    switch (item.type) {
    case MenuItemType::Action:
        if (item.action) { PlayMenuSound("SELECT"); item.action(); }
        break;

    case MenuItemType::Toggle:
        if (item.toggleState) { *item.toggleState = !*item.toggleState; PlayMenuSound("SELECT"); }
        break;

    case MenuItemType::Submenu:
        if (item.submenu) {
            PlayMenuSound("SELECT");
            item.submenu->StartOpenAnimation();
            return item.submenu;
        }
        break;

    default:
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

void Menu::Open() {
    StartOpenAnimation();
    if (selectableCount() > 0) {
        for (int i = 0; i < (int)items.size(); ++i) {
            if (isSelectable(items[i])) { selected = i; break; }
        }
        scrollOffset = std::max(0, selected - (maxDisplay - 1));
    }
    else {
        selected = 0;
        scrollOffset = 0;
    }
    AdjustScrollForTop();
}
