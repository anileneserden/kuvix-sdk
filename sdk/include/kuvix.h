#ifndef KUVIX_H
#define KUVIX_H

#ifdef __cplusplus
extern "C" {
#endif

// UI Element yapılandırması (Zincirleme event yönetimi için)
class UIElement {
public:
    const char* name;
    void event(const char* event_name, void (*callback)());
};

// Temel KuvixOS GUI SDK Fonksiyonları
void loadUI(const char* path);
void setElementText(const char* name, const char* text);
void runApp(void);

// Elementi ismine göre seçip döndüren fonksiyon (Referans döner)
UIElement& getElementByName(const char* name);

#ifdef __cplusplus
}
#endif

#endif // KUVIX_H