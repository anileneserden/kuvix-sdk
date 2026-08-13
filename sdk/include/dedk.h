#pragma once
#ifdef __cplusplus
extern "C" {
#endif
typedef struct Window Window;
Window* create_window(const char* title, int width, int height);
bool window_is_open(Window* win);
void window_clear(Window* win, unsigned int color);
void window_present(Window* win);
#ifdef __cplusplus
}
#endif
