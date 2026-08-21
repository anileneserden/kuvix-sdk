#ifndef KUVIX_H
#define KUVIX_H

#include <widget.hpp>
#include <stdarg.h> // va_list için gerekli

// C++ sınıflarını doğrudan kullanabilmek için C++ ortamında extern "C" dışına alıyoruz
class UIElement : public Widget {
public:
    UIElement(WidgetType t, const char* w_name, int px, int py, int pw, int ph, uint32_t bg, uint32_t fg, int radius, const char* txt)
        : Widget(t, w_name, px, py, pw, ph, bg, fg, radius, txt) {}
        
    void draw(DE_API*) override {}
};

#ifdef __cplusplus
extern "C" {
#endif

// Temel KuvixOS GUI SDK Fonksiyonları
void loadUI(const char* path);
void setElementText(const char* name, const char* text);
void runApp(void);

// Çekirdekten gelen ksprintf fonksiyonunun prototipi
int ksprintf(char *buf, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

// İsme göre arama yapıp doğrudan Button veya Widget pointer'ı döndüren fonksiyonlar
#ifdef __cplusplus
Button* getButtonByName(const char* name);
Label* getLabelByName(const char* name);
Widget* getElementByName(const char* name);
#endif

#endif // KUVIX_H