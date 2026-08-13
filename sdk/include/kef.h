#pragma once
#include <stdint.h>

typedef struct {
    void (*print)(const char* s);
} kef_api_t;

typedef void (*kef_entry_fn)(kef_api_t* api);
