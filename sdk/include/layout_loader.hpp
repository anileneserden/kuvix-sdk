#ifndef LAYOUT_LOADER_HPP
#define LAYOUT_LOADER_HPP

#include <stdint.h>

struct ElementConfig {
    int label_x, label_y;
    char label_text[64];
    
    int button_x, button_y;
    int button_width, button_height;
    char button_text[64];

    int panel_x, panel_y;
    int panel_width, panel_height;

    char image_path[128];
};

struct WindowConfig {
    char title[64];
    int width;
    int height;
    uint32_t background_color;
};

class LayoutLoader {
public:
    // Güncellenen imza (const veya normal uint8_t* tamponlarını destekler)
    static bool parse_kef_data(const uint8_t* file_buffer, uint32_t file_size, uint8_t** out_code, uint32_t* out_code_size, char** out_json);
    static void parse_json_config(const char* json_data, WindowConfig& win_cfg, ElementConfig& elem_cfg);

private:
    static const char* custom_strstr(const char* haystack, const char* needle);
    static const char* custom_strchr(const char* s, int c);
    static bool find_json_string(const char* json, const char* key, char* out_val, int max_len);
    static int find_json_int(const char* json, const char* key, int default_val);
    static uint32_t hex_to_uint32(const char* hex_str); // Hex renk dönüştürücü eklendi
};

#endif