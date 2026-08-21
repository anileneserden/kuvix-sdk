#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <kernel/drivers/video/de_api.h>
#include <stdint.h>

enum WidgetType {
    WIDGET_UNKNOWN = 0,
    WIDGET_LABEL,
    WIDGET_BUTTON,
    WIDGET_PANEL
};

class Widget {
public:
    char name[32];
    int x, y, width, height;
    WidgetType type;
    
    // Anchor ve esneme için orijinal değerler
    int init_x, init_y, init_width, init_height;
    int ref_win_width, ref_win_height;
    char anchor[32];

    uint32_t background_color;
    uint32_t color;
    int border_radius;
    char text[64];
    
    void (*on_click_callback)(void);
    void (*Click)(void);

    Widget(WidgetType t, const char* w_name, int px, int py, int pw, int ph, uint32_t bg, uint32_t fg, int radius, const char* txt);
    virtual ~Widget() {}
    virtual void draw(DE_API* api);
};

class Label : public Widget {
public:
    Label(const char* w_name, int px, int py, uint32_t fg, const char* txt);
    void draw(DE_API* api) override;
};

class Button : public Widget {
public:
    bool is_hovered;
    Button(const char* w_name, int px, int py, int pw, int ph, uint32_t bg, uint32_t fg, int radius, const char* txt);
    void draw(DE_API* api) override;
};

class Panel : public Widget {
public:
    Panel(const char* w_name, int px, int py, int pw, int ph, uint32_t bg, int radius);
    void draw(DE_API* api) override;
};

#endif // WIDGET_HPP