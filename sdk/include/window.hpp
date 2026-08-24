#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <kernel/drivers/video/de_api.h>
#include <stdint.h>
#include <widget.hpp>
#include <layout_loader.hpp>

struct WindowTheme {
    uint32_t bg_color;
    uint32_t header_color;
    uint32_t active_header_color;
    uint32_t border_color;
    int border_thickness;
    int header_height;
};

class Window {
public:
    int x, y;
    int width, height;
    int dragging;
    int drag_offset_x, drag_offset_y;
    int is_open;
    char title[64];

    WindowTheme theme;

    // Widget havuzu
    Widget* widgets[16];
    int widget_count;
    
    // SDK yapısına uygun tekil element konfigürasyonu
    ElementConfig elem_config;
    int has_elem_config;

    Window(int wx, int wy, WindowTheme custom_theme, const char* json_data = nullptr)  
        : x(wx), y(wy), dragging(0), drag_offset_x(0), drag_offset_y(0), is_open(1),
          theme(custom_theme), widget_count(0), has_elem_config(0) {
        
        width = 500;
        height = 350;
        
        // Varsayılan başlığı güvenli kopyala
        const char* def_title = "Kef Loader Yürütücüsü";
        int t_idx = 0;
        while (def_title[t_idx] != '\0' && t_idx < 63) {
            title[t_idx] = def_title[t_idx];
            t_idx++;
        }
        title[t_idx] = '\0';

        if (json_data) {
            WindowConfig win_cfg;
            win_cfg.width = 0;
            win_cfg.height = 0;
            win_cfg.title[0] = '\0';
            win_cfg.background_color = 0;

            // SDK'nın desteklediği doğru imza ile konfigürasyonu parse ediyoruz
            LayoutLoader::parse_json_config(json_data, win_cfg, elem_config);
            has_elem_config = 1;

            if (win_cfg.width > 50) width = win_cfg.width;
            if (win_cfg.height > 50) height = win_cfg.height;

            theme.bg_color = win_cfg.background_color;

            if (win_cfg.title[0] != '\0') {
                int i = 0;
                while (win_cfg.title[i] != '\0' && i < 63) {
                    title[i] = win_cfg.title[i];
                    i++;
                }
                title[i] = '\0';
            }
        }

        // Eğer JSON verisi yoksa veya element yüklenmediyse varsayılan bir panel ekleyelim
        if (!has_elem_config) {
            // Varsayılan panel değerleri
            Panel* panel = new Panel("Sol Dizin", x + 50, y + theme.header_height + 50, 100, 100, 0x002A2E3D, 4);
            add_widget(panel);
        } else {
            // SDK'daki ElementConfig alan adlarına (örn: panel_x, panel_width vb.) göre oluşturma
            // Not: LayoutLoader yapındaki alan isimlerine göre burayı şekillendirebilirsin.
            Panel* panel = new Panel("LayoutElement", x + 50, y + theme.header_height + 50, 100, 100, 0x002A2E3D, 4);
            add_widget(panel);
        }

        update_widgets();
    }

    ~Window() {
        for (int i = 0; i < widget_count; i++) {
            delete widgets[i];
        }
    }

    void add_widget(Widget* w) {
        if (widget_count < 16) {
            widgets[widget_count++] = w;
        }
    }

    Widget* getElementByName(const char* target_name) {
        if (!target_name) return nullptr;
        for (int i = 0; i < widget_count; i++) {
            if (!widgets[i]) continue;
            int j = 0;
            bool match = true;
            while (widgets[i]->name[j] != '\0' || target_name[j] != '\0') {
                if (widgets[i]->name[j] != target_name[j]) {
                    match = false;
                    break;
                }
                j++;
            }
            if (match) return widgets[i];
        }
        return nullptr;
    }

    void update_widgets() {
        // Gerekirse widget konum senkronizasyonları buraya eklenebilir
    }

    void handle_click(const de_mouse_state_t& mouse) {
        if (mouse.left_button) {
            for (int i = 0; i < widget_count; i++) {
                if (!widgets[i]) continue;
                if (mouse.x >= widgets[i]->x && mouse.x <= widgets[i]->x + widgets[i]->width &&
                    mouse.y >= widgets[i]->y && mouse.y <= widgets[i]->y + widgets[i]->height) {
                    
                    if (widgets[i]->type == WIDGET_BUTTON && widgets[i]->on_click_callback) {
                        widgets[i]->on_click_callback();
                    }
                }
            }
        }
    }

    void update(const de_mouse_state_t& mouse) {
        if (!is_open) return;

        // --- HOVER KONTROLÜ ---
        for (int i = 0; i < widget_count; i++) {
            if (!widgets[i]) continue;
            if (widgets[i]->type == WIDGET_BUTTON) {
                Button* btn = (Button*)widgets[i];
                if (mouse.x >= btn->x && mouse.x <= btn->x + btn->width &&
                    mouse.y >= btn->y && mouse.y <= btn->y + btn->height) {
                    btn->is_hovered = true;
                } else {
                    btn->is_hovered = false;
                }
            }
        }
        // ---------------------

        if (mouse.left_button == 1) {
            if (dragging == 0) {
                int close_x = x + width - theme.header_height;
                if (mouse.x >= x && mouse.x <= (x + width) &&
                    mouse.y >= y && mouse.y <= (y + theme.header_height)) {
                    
                    if (mouse.x >= close_x && mouse.x <= (x + width)) {
                        is_open = 0;
                        return;
                    }

                    dragging = 1;
                    drag_offset_x = mouse.x - x;
                    drag_offset_y = mouse.y - y;
                }
            } else {
                int new_x = mouse.x - drag_offset_x;
                int new_y = mouse.y - drag_offset_y;

                if (new_x < 0) new_x = 0;
                if (new_y < 0) new_y = 0;
                if (new_x > 1920 - width) new_x = 1920 - width;
                if (new_y > 1080 - height) new_y = 1080 - height;

                x = new_x;
                y = new_y;
            }
        } else {
            dragging = 0;
        }

        update_widgets();
    }

    void draw(DE_API* api) {
        if (!is_open || !api || !api->draw_rect) return;

        int t = theme.border_thickness;
        api->draw_rect(x - t, y - t, width + (t * 2), height + (t * 2), theme.border_color);
        api->draw_rect(x, y, width, height, theme.bg_color);

        uint32_t current_header = (dragging == 1) ? theme.active_header_color : theme.header_color;
        api->draw_rect(x, y, width, theme.header_height, current_header);

        if (title[0] != '\0' && api->draw_text) {
            api->draw_text(x + 8, y + (theme.header_height - 8) / 2, title, 0x00FFFFFF);
        }

        int btn_size = 14;
        int btn_x = x + width - theme.header_height + (theme.header_height - btn_size) / 2;
        int btn_y = y + (theme.header_height - btn_size) / 2;
        api->draw_rect(btn_x, btn_y, btn_size, btn_size, 0x00E63946);

        for (int i = 0; i < widget_count; i++) {
            if (widgets[i]) {
                widgets[i]->draw(api);
            }
        }
    }
};

#endif // WINDOW_HPP