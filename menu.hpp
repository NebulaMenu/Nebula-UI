#pragma once
#include <string>
#include <vector>
#include <functional>

struct NebulaMenuItem {
    std::string label;
    std::function<void()> action;
};

class NebulaMenu {
    std::string title;
    std::vector<NebulaMenuItem> items;
    size_t selected = 0;
public:
    NebulaMenu(const std::string& t) : title(t) {}

    void AddItem(const std::string& label, std::function<void()> action);
    void Render();
    void Up();
    void Down();
    void Select();
};
