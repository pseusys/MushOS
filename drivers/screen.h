#ifndef MUSHOS_SCREEN_H
#define MUSHOS_SCREEN_H

#include "../kernel/generic.h"

#define VIDEO_MEMORY 0xb8000

#define ROWS_NUM 25
#define COLS_NUM 80

typedef enum {
    BLACK = 0, LOW_BLUE = 1, LOW_GREEN = 2, LOW_CYAN = 3, LOW_RED = 4, LOW_MAGENTA = 5,
    BROWN = 6, LIGHT_GRAY = 7, DARK_GRAY = 8, HIGH_BLUE = 9, HIGH_GREEN = 10,
    HIGH_CYAN = 11, HIGH_RED = 12, HIGH_MAGENTA = 13, YELLOW = 14, WHITE = 15
} color;

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
