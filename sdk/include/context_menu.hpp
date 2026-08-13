#ifndef CONTEXT_MENU_HPP
#define CONTEXT_MENU_HPP

#include <kernel/drivers/video/de_api.h>

void context_menu_init();
void context_menu_handle(DE_API* api, de_mouse_state_t* mouse, bool* menu_open_flag);
void context_menu_draw(DE_API* api);

#endif
