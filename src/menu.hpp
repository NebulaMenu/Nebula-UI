#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>

enum class MenuItemType {
    Action,
    Toggle,
    Submenu,
    NumberOption,
    TextOption,
    Separator
};

struct MenuItem {
    std::string label;
    MenuItemType type;
    std::function<void()> action;
    bool* toggleState = nullptr;
    std::shared_ptr<class Menu> submenu = nullptr;

    int* intValue = nullptr;
    float* floatValue = nullptr;
    int   minInt = 0;
    int   maxInt = 100;
    float minFloat = 0.0f;
    float maxFloat = 100.0f;
    int   stepInt = 1;
    float stepFloat = 1.0f;
    bool  isFloat = false;
};

class Menu {
private:
    std::string title;
    std::vector<MenuItem> items;
    int selected = 0;
    int scrollOffset = 0;
    int maxDisplay = 12;

    struct MenuStyle {
        float x, y, width, headerHeight, itemHeight, footerHeight;
        struct Color { int r; int g; int b; int a; };
        Color background, header, footer, selection, text, selectedText, disabledText, toggleOn, toggleOff;
        MenuStyle();
    } style;

    float openAnimation = 0.0f;
    bool isOpening = false;

    void DrawHeader();
    void DrawItems();
    void DrawFooter();
    void DrawSelection();
    void DrawScrollIndicator();

    bool isSelectable(const MenuItem& it) const;
    int  findNextSelectable(int from, int step) const;
    int  selectableCount() const;
    int  indexInSelectableList(int absoluteIndex) const;

public:
    Menu(const std::string& t) : title(t) {}

    void AddAction(const std::string& label, std::function<void()> action);
    void AddToggle(const std::string& label, bool* state);
    void AddSubmenu(const std::string& label, std::shared_ptr<Menu> submenu);
    void AddNumber(const std::string& label, int* value, int min, int max, int step = 1);
    void AddNumber(const std::string& label, float* value, float min, float max, float step = 0.1f);

    void AddText(const std::string& label);
    void AddSeparator(const std::string& label = std::string());

    std::shared_ptr<Menu> AddFolder(const std::string& label);
    std::shared_ptr<Menu> AddFolder(const std::string& label,
        const std::function<void(std::shared_ptr<Menu>)>& build);

    void Render();
    void Up();
    void Down();
    void Left();
    void Right();
    std::shared_ptr<Menu> Select();

    MenuItemType CurrentType() const;
    std::shared_ptr<Menu> CurrentSubmenu() const;

    void StartOpenAnimation() { isOpening = true; openAnimation = 0.0f; }
    void Open();
    void Close() { isOpening = false; }
};
