#ifndef APP_HPP
#define APP_HPP

#include <window.hpp>
#include <widget.hpp>
#include <layout_loader.hpp>
#include <kernel/drivers/video/de_api.h>

class ApplicationProcess {
public:
    Window* window;
    bool active;
    WindowTheme theme;

    ApplicationProcess(DE_API* api, const char* kef_path) {
        active = false;
        window = nullptr;

        if (!api || !api->read_file) return;

        if (api->log) {
            api->log("[App] Dosya okunmaya calisiliyor, hedef: ");
            api->log(kef_path);
            api->log("\n");
        }

        uint8_t file_buffer[64 * 1024];
        int read_bytes = -1;

        // --- AKILLI YOL DENEME MEKANİZMASI (FALLBACK) ---
        // 1. Önce doğrudan verilen yolla dene
        read_bytes = api->read_file(kef_path, (char*)file_buffer, sizeof(file_buffer) - 1);

        // 2. Eğer başarısız olduysa ve yol '/' ile başlamıyorsa, başına '/' ekleyerek dene
        if (read_bytes <= 0 && kef_path[0] != '/') {
            char alt_path[256];
            alt_path[0] = '/';
            int i = 0;
            while (kef_path[i] != '\0' && i < 254) {
                alt_path[i + 1] = kef_path[i];
                i++;
            }
            alt_path[i + 1] = '\0';

            if (api->log) {
                api->log("[App] Alternatif yol deneniyor: ");
                api->log(alt_path);
                api->log("\n");
            }
            read_bytes = api->read_file(alt_path, (char*)file_buffer, sizeof(file_buffer) - 1);
        }
        // 3. Eğer '/' ile başlıyorsa ama başarısız olduysa, başındaki '/' karakterini atıp dene
        else if (read_bytes <= 0 && kef_path[0] == '/') {
            if (api->log) {
                api->log("[App] Alternatif yol deneniyor (basindaki / kaldirildi)...\n");
            }
            read_bytes = api->read_file(kef_path + 1, (char*)file_buffer, sizeof(file_buffer) - 1);
        }
        // ------------------------------------------------

        if (read_bytes > 0) {
            uint32_t file_size = (uint32_t)read_bytes;
            file_buffer[file_size] = '\0';

            if (api->log) {
                api->log("[App] === DOSYA ICERIGI BASLANGICI ===\n");
                api->log((const char*)file_buffer);
                api->log("\n[App] === DOSYA ICERIGI BITISI ===\n");
            }

            uint8_t* code_ptr = nullptr;
            uint32_t code_size = 0;
            char* json_data = nullptr;

            if (LayoutLoader::parse_kef_data(file_buffer, file_size, &code_ptr, &code_size, &json_data)) {
                theme = {
                    .bg_color = 0x002B2D42,
                    .header_color = 0x001D1E2C,
                    .active_header_color = 0x003D405B,
                    .border_color = 0x004A4E69,
                    .border_thickness = 2,
                    .header_height = 24
                };

                // const char* dönüşümü eklenerek imza uyumsuzluğu giderildi
                window = new Window(150, 150, theme, (const char*)json_data);
                active = true;
                if (api->log) api->log("[App] Uygulama basariyla baslatildi ve pencere acildi.\n");
            } else {
                if (api->log) api->log("[App] HATA: KEF veri yapisi parse edilemedi!\n");
            }
        } else {
            if (api->log) {
                api->log("[App] HATA: Dosya hiçbir şekilde okunamadı!\n");
            }
        }
    }

    void update_and_draw(DE_API* api, const de_mouse_state_t& mouse) {
        if (!active || !window) return;

        // 1. Önce fare tıklamalarını pencereye ve widget'lara ilet
        window->handle_click(mouse);

        // 2. Sonra pencere konum ve durum güncellemelerini yap
        window->update(mouse);

        // 3. Açıksa ekrana çizdir
        if (window->is_open) {
            window->draw(api);
        } else {
            active = false;
        }
    }

    ~ApplicationProcess() {
        if (window) delete window;
    }
};

#endif // APP_HPP