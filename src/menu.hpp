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
    TextOption
};

struct MenuItem {
    std::string label;
    MenuItemType type;
    std::function<void()> action;
    bool* toggleState = nullptr;
    std::shared_ptr<class Menu> submenu = nullptr;

    // For number options
    int* intValue = nullptr;
    float* floatValue = nullptr;
    int minInt = 0;
    int maxInt = 100;
    float minFloat = 0.0f;
    float maxFloat = 100.0f;
    int stepInt = 1;
    float stepFloat = 1.0f;
    bool isFloat = false;
};

class Menu {
private:
    std::string title;
    std::vector<MenuItem> items;
    int selected = 0;
    int scrollOffset = 0;
    int maxDisplay = 12; // Max visible items

    // Visual settings - Terror/Elegance style
    struct MenuStyle {
        float x;
        float y;
        float width;
        float headerHeight;
        float itemHeight;
        float footerHeight;

        // Colors (grayscale theme)
        struct Color { int r; int g; int b; int a; };
        Color background;
        Color header;
        Color footer;
        Color selection;
        Color text;
        Color selectedText;
        Color disabledText;
        Color toggleOn;
        Color toggleOff;

        MenuStyle() {
            x = 0.8f;
            y = 0.15f;
            width = 0.20f;
            headerHeight = 0.08f;
            itemHeight = 0.035f;
            footerHeight = 0.030f;

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
    } style;

    // Animation
    float openAnimation = 0.0f;
    bool isOpening = false;

    void DrawHeader();
    void DrawItems();
    void DrawFooter();
    void DrawSelection();
    void DrawScrollIndicator();

public:
    Menu(const std::string& t) : title(t) {}

    // Add items
    void AddAction(const std::string& label, std::function<void()> action);
    void AddToggle(const std::string& label, bool* state);
    void AddSubmenu(const std::string& label, std::shared_ptr<Menu> submenu);
    void AddNumber(const std::string& label, int* value, int min, int max, int step = 1);
    void AddNumber(const std::string& label, float* value, float min, float max, float step = 0.1f);

    // Helpers
    std::shared_ptr<Menu> AddFolder(const std::string& label);
    std::shared_ptr<Menu> AddFolder(const std::string& label,
        const std::function<void(std::shared_ptr<Menu>)>& build);

    // Core functions
    void Render();
    void Up();
    void Down();
    void Left();
    void Right();
    std::shared_ptr<Menu> Select();

    // Getters
    MenuItemType CurrentType() const;
    std::shared_ptr<Menu> CurrentSubmenu() const;

    void StartOpenAnimation() {
        isOpening = true;
        openAnimation = 0.0f;
    }
    void Open() { StartOpenAnimation(); }
    void Close() {
        isOpening = false;
    }
};