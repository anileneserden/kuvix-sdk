#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <kernel/drivers/video/de_api.h>
#include <stdint.h>

enum WidgetType {
    WIDGET_LABEL,
    WIDGET_BUTTON
};

class Widget {
public:
    WidgetType type;
    char name[32];
    int x, y;
    int width, height;
    uint32_t background_color;
    uint32_t color;
    int border_radius;
    char text[64];

    void (*on_click_callback)();

    // Sadece imza (Gövde widget.cpp içinde olacak)
    Widget(WidgetType t, const char* w_name, int px, int py, int pw, int ph, uint32_t bg, uint32_t fg, int radius, const char* txt);
    virtual ~Widget() {}

    virtual void draw(DE_API* api) = 0;

    void onClick(void (*func)()) {
        on_click_callback = func;
    }
};

class Label : public Widget {
public:
    Label(const char* w_name, int px, int py, uint32_t fg, const char* txt);
    void draw(DE_API* api) override;

    void setText(const char* new_text) {
        int i = 0;
        while (new_text[i] != '\0' && i < 63) {
            text[i] = new_text[i];
            i++;
        }
        text[i] = '\0';
    }
};

class Button : public Widget {
public:
    Button(const char* w_name, int px, int py, int pw, int ph, uint32_t bg, uint32_t fg, int radius, const char* txt);
    void draw(DE_API* api) override;
};

#endif // WIDGET_HPP