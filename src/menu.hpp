#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>

enum class MenuItemType { Action, Toggle, Submenu };

struct MenuItem {
    std::string label;
    MenuItemType type;
    std::function<void()> action;
    bool* toggleState = nullptr;
    std::shared_ptr<class Menu> submenu = nullptr;
};

class Menu {
    std::string title;
    std::vector<MenuItem> items;
    int selected = 0;

public:
    Menu(const std::string& t) : title(t) {}

    void AddAction(const std::string& label, std::function<void()> action);
    void AddToggle(const std::string& label, bool* state);
    void AddSubmenu(const std::string& label, std::shared_ptr<Menu> submenu);

    std::shared_ptr<Menu> AddFolder(const std::string& label);
    std::shared_ptr<Menu> AddFolder(const std::string& label,
        const std::function<void(std::shared_ptr<Menu>)>& build);

    void Render();
    void Up();
    void Down();
    std::shared_ptr<Menu> Select();

    MenuItemType CurrentType() const;
    std::shared_ptr<Menu> CurrentSubmenu() const;
};
