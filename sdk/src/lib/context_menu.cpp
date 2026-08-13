#include <context_menu.hpp>

static bool menu_open = false;
static int menu_x = 0;
static int menu_y = 0;
static int hovered_index = -1;

static const int menu_width = 160;
static const int item_height = 25;
static const int item_count = 3;
static const int menu_height = item_count * item_height + 8;

static const char* items[3] = {
    "Yeni Klasor",
    "Terminali Ac",
    "Ekran Ayarlari"
};

void context_menu_init() {
    menu_open = false;
    hovered_index = -1;
}

void context_menu_handle(DE_API* api, de_mouse_state_t* mouse, bool* menu_open_flag) {
    static uint8_t prev_right_button = 0;
    static uint8_t prev_left_button = 0;

    if (!api || !mouse) return;

    if (mouse->right_button && !prev_right_button) {
        menu_open = true;
        menu_x = mouse->x;
        menu_y = mouse->y;
        if (menu_open_flag) *menu_open_flag = true;
    }

    hovered_index = -1;

    if (menu_open) {
        bool inside_menu = (mouse->x >= menu_x && mouse->x <= menu_x + menu_width &&
                            mouse->y >= menu_y && mouse->y <= menu_y + menu_height);

        if (inside_menu) {
            int relative_y = mouse->y - (menu_y + 4);
            if (relative_y >= 0) {
                int idx = relative_y / item_height;
                if (idx >= 0 && idx < item_count) {
                    hovered_index = idx;
                }
            }
        }

        if (mouse->left_button && !prev_left_button) {
            if (inside_menu && hovered_index != -1) {
                if (hovered_index == 0) {
                    if (api->log) api->log("[RightClickTest] Secildi: Yeni Klasor");
                } else if (hovered_index == 1) {
                    if (api->log) api->log("[RightClickTest] Secildi: Terminali Ac");
                } else if (hovered_index == 2) {
                    if (api->log) api->log("[RightClickTest] Secildi: Ekran Ayarlari");
                }
            }
            menu_open = false;
            if (menu_open_flag) *menu_open_flag = false;
        }
    }

    prev_right_button = mouse->right_button;
    prev_left_button = mouse->left_button;
}

void context_menu_draw(DE_API* api) {
    if (!api || !menu_open) return;

    if (api->fill_round_rect) {
        api->fill_round_rect(menu_x, menu_y, menu_width, menu_height, 4, 0x001D1E2C);
        api->draw_rect(menu_x, menu_y, menu_width, menu_height, 0x004A4E69);
    }

    for (int i = 0; i < item_count; i++) {
        int item_y = menu_y + 4 + (i * item_height);

        if (i == hovered_index) {
            if (api->fill_round_rect) {
                api->fill_round_rect(menu_x + 4, item_y, menu_width - 8, item_height, 3, 0x003D405B);
            }
        }

        if (api->draw_text) {
            api->draw_text(menu_x + 12, item_y + 6, items[i], 0x00E0E0E0);
        }
    }
}
