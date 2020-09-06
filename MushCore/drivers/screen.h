#ifndef MUSHOS_SCREEN_H
#define MUSHOS_SCREEN_H

#include "../../MushLib/generic.h"

void print_char_color(char c, byte text_color, byte back_color);
void print_string_color(string str, byte text_color, byte back_color, u_dword length);
void clear_screen();
void init_screen_io();

#endif //MUSHOS_SCREEN_H
