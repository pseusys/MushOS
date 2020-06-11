#ifndef MUSHOS_SCREEN_H
#define MUSHOS_SCREEN_H

#include "../kernel/generic.h"

#define VIDEO_MEMORY 0xb8000

#define ROWS_NUM 25
#define COLS_NUM 80

#define BLACK         0
#define LOW_BLUE      1
#define LOW_GREEN     2
#define LOW_CYAN      3
#define LOW_RED       4
#define LOW_MAGENTA   5
#define BROWN         6
#define LIGHT_GRAY    7
#define DARK_GRAY     8
#define HIGH_BLUE     9
#define HIGH_GREEN    10
#define HIGH_CYAN     11
#define HIGH_RED      12
#define HIGH_MAGENTA  13
#define YELLOW        14
#define WHITE         15

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

typedef struct {
    word row;
    word column;
} screen_coords;

screen_coords get_cursor();
void set_cursor (screen_coords coords);

void print_char (char c);
void print_char_at (char c, screen_coords coords);
void print_char_color (char c, byte text_color, byte back_color);
void print_char_color_at (char c, screen_coords coords, byte text_color, byte back_color);

void print (c_string str);
void print_at (c_string str, screen_coords coords);
void print_color (c_string str, byte text_color, byte back_color);
void print_color_at (c_string str, screen_coords coords, byte text_color, byte back_color);

void clear_screen();

#endif //MUSHOS_SCREEN_H
