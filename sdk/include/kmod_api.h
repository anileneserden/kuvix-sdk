#ifndef KMOD_API_H
#define KMOD_API_H

#include <stdint.h>
#include <stddef.h>

// Çekirdeğin modüle sağlayacağı temel servisler
typedef struct {
    void (*printk)(const char* fmt, ...);
    void* (*kmalloc)(uint32_t size);
    void (*kfree)(void* ptr);
    int (*read_file)(const char* path, char* buffer, uint32_t max_size);
} KModKernelAPI;

// Tamamen dinamik ve isme göre operasyon döndüren evrensel yapı:
typedef struct {
    void* (*get_operation)(const char* op_name);
} KModOperations;

#endif