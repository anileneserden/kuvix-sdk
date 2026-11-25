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

typedef struct {
    int (*control)(uint32_t cmd, void* arg);
    void* (*get_operation)(const char* op_name);
} KModOperations;

#endif