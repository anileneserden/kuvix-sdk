#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <kernel/drivers/video/de_api.h>
#include <login_api.h>
#include <stdint.h>
#include <string.h>

enum WidgetType {
    WIDGET_UNKNOWN = 0,
    WIDGET_LABEL,
    WIDGET_BUTTON,
    WIDGET_PANEL,
    WIDGET_PICTUREBOX,
    WIDGET_INPUT
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

    // KURUCUYU BURADA INLINE OLARAK TANIMLIYORUZ:
    Widget(WidgetType t, const char* w_name, int px, int py, int pw, int ph, uint32_t bg, uint32_t fg, int radius, const char* txt) {
        type = t;
        int i = 0;
        while (w_name && w_name[i] != '\0' && i < 31) {
            name[i] = w_name[i];
            i++;
        }
        name[i] = '\0';

        x = px;
        y = py;
        width = pw;
        height = ph;
        background_color = bg;
        color = fg;
        border_radius = radius;

        i = 0;
        while (txt && txt[i] != '\0' && i < 63) {
            text[i] = txt[i];
            i++;
        }
        text[i] = '\0';
        
        on_click_callback = nullptr;
        Click = nullptr;
    }

    virtual ~Widget() {}
    
    virtual void draw(DE_API* api) {}
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

class PictureBox : public Widget {
public:
    char image_path[128];

    PictureBox(const char* w_name, int px, int py, int pw, int ph, const char* img_path)
        : Widget(WIDGET_PICTUREBOX, w_name, px, py, pw, ph, 0, 0, 0, "") {
        
        int i = 0;
        while (img_path[i] != '\0' && i < 127) {
            image_path[i] = img_path[i];
            i++;
        }
        image_path[i] = '\0';
    }

    void draw(DE_API* api) override {
        if (!api || !api->render_kbi) return;
        api->render_kbi(x, y, width, height, image_path);
    }
};

// InputField artık Widget'tan miras alıyor ve inline olarak tanımlanıyor
class InputField : public Widget {
public:
    bool focused;
    int len;

    InputField(const char* w_name, int px, int py, int pw, int ph, uint32_t bg = 0x111111, uint32_t fg = 0xFFFFFF, int radius = 4, const char* placeholder = "Kullanici adi...")
        : Widget(WIDGET_INPUT, w_name, px, py, pw, ph, bg, fg, radius, "") {
        focused = false;
        len = 0;
        
        if (placeholder) {
            int i = 0;
            while (placeholder[i] != '\0' && i < 63) {
                text[i] = placeholder[i];
                i++;
            }
            text[i] = '\0';
        }
    }

    void update_click(int mx, int my, uint8_t left_btn, uint8_t prev_left_btn) {
        if (left_btn && !prev_left_btn) {
            if (mx >= x && mx <= x + width && my >= y && my <= y + height) {
                focused = true;
            } else {
                focused = false;
            }
        }
    }

    void handle_key(char c) {
        if (!focused) return;

        if (c == '\b' || c == 127) {
            if (len > 0) {
                len--;
                text[len] = '\0';
            }
        } else if (c >= 32 && c < 127) {
            if (len < (int)sizeof(text) - 1) {
                text[len++] = c;
                text[len] = '\0';
            }
        }
    }

    void draw(DE_API* api) override {
        // Standart DE_API çizimi için (gerekirse doldurulabilir)
    }

    void draw_login(LoginAPI* api) {
        if (!api) return;

        uint32_t border_color = focused ? 0x00FF00 : 0xFFFFFF;
        if (api->draw_rect) {
            api->draw_rect(x, y, width, height, border_color);
            api->draw_rect(x + 2, y + 2, width - 4, height - 4, background_color);
        }

        if (api->draw_text) {
            const char* display_text = (len > 0) ? text : "Kullanici adi...";
            uint32_t text_color = (len > 0) ? color : 0x777777;
            api->draw_text(x + 8, y + 10, display_text, text_color);
        }
    }
};

#endif // WIDGET_HPP