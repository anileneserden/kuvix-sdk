#ifndef CMD_API_H
#define CMD_API_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    void (*print)(const char* str);
    char (*get_key)(void);
    int  (*read_file)(const char* path, char* buffer, uint32_t max_size);
    int  (*write_file)(const char* path, const char* buffer, uint32_t size);
    int  (*get_file_size)(const char* path);
    
    // Sürücülerden veya sistem servislerinden anahtar-değer (key-value) tabanlı veri çekmek için:
    int  (*get_driver_value)(const char* driver_name, const char* key, char* out_buf, uint32_t max_size);
    
    // Sürücüye özel komut, kontrol veya işlem göndermek için (Sürücü control rutini köprüsü):
    int  (*control_driver)(const char* driver_name, const char* command, void* arg, uint32_t arg_size);

    // Dinamik sürücü/modül yüklemek için (modprobe desteği):
    int  (*load_driver)(const char* name, const char* filepath);

    // Dinamik kmod operasyonlarına ve modüller arası köprülere erişmek için:
    void* (*get_kmod_operation)(const char* op_name);
} CmdAPI;

#endif