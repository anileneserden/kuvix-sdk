#ifndef KUVIX_H
#define KUVIX_H

#ifdef __cplusplus
extern "C" {
#endif

// UI Element yapılandırması için basit bir sınıf/yapı
class UIElement {
public:
    const char* name;
    void event(const char* eventName, void (*callback)());
};

// Temel KuvixOS SDK Fonksiyonları
void loadUI(const char* path);
void setElementText(const char* name, const char* text);
void runApp(void);
UIElement getElementByName(const char* name);
void printk(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // KUVIX_H