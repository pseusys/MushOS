#ifndef MUSHOS_SCREEN_H
#define MUSHOS_SCREEN_H

#include "../kernel/generic.h"

#define VIDEO_MEMORY 0xb8000

#define ROWS_NUM 25
#define COLS_NUM 80

typedef enum {
    BLACK = 0u, LOW_BLUE = 1u, LOW_GREEN = 2u, LOW_CYAN = 3u, LOW_RED = 4u, LOW_MAGENTA = 5u,
    BROWN = 6u, LIGHT_GRAY = 7u, DARK_GRAY = 8u, HIGH_BLUE = 9u, HIGH_GREEN = 10u,
    HIGH_CYAN = 11u, HIGH_RED = 12u, HIGH_MAGENTA = 13u, YELLOW = 14u, WHITE = 15u
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

#define undefined '?'

typedef enum {
    DECIMAL = 10, HEXADECIMAL = 16, BINARY = 2
} system;

typedef enum {
    INTEGER, FLOAT
} type;

void print_number(u_dword num, type t, system sys, color front, color back);

void log(color text, color back, const char* template, ...);

#define bad(temp, args...) log(HIGH_RED, BLACK, temp, ## args);
#define warn(temp, args...) log(YELLOW, BLACK, temp, ## args);
#define good(temp, args...) log(HIGH_GREEN, BLACK, temp, ## args);
#define info(temp, args...) log(HIGH_BLUE, BLACK, temp, ## args);
#define plain(temp, args...) log(LIGHT_GRAY, BLACK, temp, ## args);

#define locate(text, back) log(text, back, "%s - %d: ", __FILE__, __LINE__);

#define error(temp, args...) {locate(HIGH_RED, BLACK) bad(temp, ## args)}
#define warning(temp, args...) {locate(YELLOW, BLACK) warn(temp, ## args)}

#endif //MUSHOS_SCREEN_H
