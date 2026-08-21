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

    // Dinamik element yönetimi için widget havuzu ve konfigürasyon
    Widget* widgets[16];
    int widget_count;
    ElementConfig elem_cfg; // Elem_cfg artık sınıf seviyesinde üye değişken

    Window(int wx, int wy, WindowTheme custom_theme, const char* json_data = nullptr)  
        : x(wx), y(wy), dragging(0), drag_offset_x(0), drag_offset_y(0), is_open(1),
          theme(custom_theme), widget_count(0) {
        
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

        // Varsayılan element konum ve boyutları
        elem_cfg.label_x = 20; elem_cfg.label_y = 40;
        elem_cfg.button_x = 20; elem_cfg.button_y = 80;
        elem_cfg.button_width = 160; elem_cfg.button_height = 35;
        
        // Varsayılan metinleri atayalım
        const char* def_label_text = "Varsayilan Etiket";
        int l_i = 0;
        while(def_label_text[l_i] != '\0' && l_i < 127) {
            elem_cfg.label_text[l_i] = def_label_text[l_i];
            l_i++;
        }
        elem_cfg.label_text[l_i] = '\0';

        const char* def_btn_text = "Tikla";
        int b_i = 0;
        while(def_btn_text[b_i] != '\0' && b_i < 63) {
            elem_cfg.button_text[b_i] = def_btn_text[b_i];
            b_i++;
        }
        elem_cfg.button_text[b_i] = '\0';

        if (json_data) {
            WindowConfig win_cfg;
            win_cfg.width = 0;
            win_cfg.height = 0;
            win_cfg.title[0] = '\0';
            win_cfg.background_color = 0; // Varsayılan değer

            LayoutLoader::parse_json_config(json_data, win_cfg, elem_cfg);

            if (win_cfg.width > 50) width = win_cfg.width;
            if (win_cfg.height > 50) height = win_cfg.height;

            // Eğer JSON'dan geçerli bir renk okunduysa temaya işle
            // (Siyah renk #000000 de dahil olmak üzere doğrudan atayabiliriz, 
            // çünkü parse_json_config içinde varsayılan olarak zaten atanıyor)
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

        // Widget'ları oluştur ve pencereye ekle
        Label* lbl = new Label("label", elem_cfg.label_x, elem_cfg.label_y, 0x0089B4FA, elem_cfg.label_text);
        Button* btn = new Button("button", elem_cfg.button_x, elem_cfg.button_y, elem_cfg.button_width, elem_cfg.button_height, 0x003D405B, 0x00FFFFFF, 4, elem_cfg.button_text);

        add_widget(lbl);
        add_widget(btn);

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

    // Widget'ları pencerenin `(x, y)` konumuna ve header yüksekliğine göre güncelliyoruz
    void update_widgets() {
        for (int i = 0; i < widget_count; i++) {
            if (!widgets[i]) continue;
            
            // İsme veya tipe göre konumları güncelle
            if (widgets[i]->name[0] == 'l' || widgets[i]->name[0] == 'L') {
                widgets[i]->x = x + elem_cfg.label_x;
                widgets[i]->y = y + theme.header_height + elem_cfg.label_y;
            } else if (widgets[i]->name[0] == 'b' || widgets[i]->name[0] == 'B') {
                widgets[i]->x = x + elem_cfg.button_x;
                widgets[i]->y = y + theme.header_height + elem_cfg.button_y;
                widgets[i]->width = elem_cfg.button_width;
                widgets[i]->height = elem_cfg.button_height;
            }
        }
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
                // Fare butonun sınırları içinde mi?
                if (mouse.x >= btn->x && mouse.x <= btn->x + btn->width &&
                    mouse.y >= btn->y && mouse.y <= btn->y + btn->height) {
                    if (!btn->is_hovered) {
                        btn->is_hovered = true;
                        // İsteğe bağlı: Loglama veya konsola yazdırma
                        // printf("Buton uzerine gelindi!\n"); 
                    }
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