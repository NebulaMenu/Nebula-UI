#include "menu.hpp"
#include "draw.hpp"

void Menu::AddAction(const std::string& label, std::function<void()> action) {
    items.push_back({ label, MenuItemType::Action, action });
}
void Menu::AddToggle(const std::string& label, bool* state) {
    items.push_back({ label, MenuItemType::Toggle, nullptr, state });
}
void Menu::AddSubmenu(const std::string& label, std::shared_ptr<Menu> submenu) {
    items.push_back({ label, MenuItemType::Submenu, nullptr, nullptr, submenu });
}

void Menu::Render() {
    float x = 0.15f, y = 0.15f;

    NebulaDrawRect(x, y, 0.20f, 0.05f, 109, 40, 217, 200);
    NebulaDrawText(x - 0.09f, y - 0.02f, 0.5f, title, 255, 255, 255, 255);

    for (int i = 0; i < (int)items.size(); i++) {
        float rowY = y + 0.06f + i * 0.035f;
        bool isSel = (i == selected);

        int bg = isSel ? 150 : 80;
        NebulaDrawRect(x, rowY, 0.20f, 0.03f, 30, 30, 30, bg);
        NebulaDrawText(x - 0.09f, rowY - 0.01f, 0.35f, items[i].label, 255, 255, 255, 255);

        if (items[i].type == MenuItemType::Toggle && items[i].toggleState) {
            if (*items[i].toggleState)
                NebulaDrawText(x + 0.07f, rowY - 0.01f, 0.35f, "ON", 0, 255, 0, 255);
            else
                NebulaDrawText(x + 0.07f, rowY - 0.01f, 0.35f, "OFF", 255, 0, 0, 255);
        }
        else if (items[i].type == MenuItemType::Submenu) {
            NebulaDrawText(x + 0.07f, rowY - 0.01f, 0.35f, ">>", 200, 200, 200, 255);
        }
    }
}

void Menu::Up() { if (!items.empty()) selected = (selected - 1 + (int)items.size()) % (int)items.size(); }
void Menu::Down() { if (!items.empty()) selected = (selected + 1) % (int)items.size(); }

std::shared_ptr<Menu> Menu::Select() {
    if (items.empty()) return nullptr;
    auto& it = items[selected];
    if (it.type == MenuItemType::Action && it.action) it.action();
    else if (it.type == MenuItemType::Toggle && it.toggleState) *it.toggleState = !*it.toggleState;
    else if (it.type == MenuItemType::Submenu && it.submenu) return it.submenu;
    return nullptr;
}
