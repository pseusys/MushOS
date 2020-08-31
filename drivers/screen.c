#include "screen.h"
#include "ports_io.h"
#include "../mushlib/memory.h"
#include "../mushlib/heap.h"
#include "../mushlib/vararg.h"

static u_dword get_cursor_offset() {
    port_byte_out(REG_SCREEN_CTRL, 14);
    u_dword offset = (u_dword) ((port_byte_in(REG_SCREEN_DATA) << 8) & 0xff00);
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += (u_dword) (port_byte_in(REG_SCREEN_DATA) & 0x00ff);
    return offset;
}

static void set_cursor_offset(u_dword offset) {
    //if (offset > COLS_NUM * ROWS_NUM) return; FIXME simple check not working.
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (byte) ((offset & 0xff00) >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (byte) (offset & 0x00ff));
}

screen_coords get_cursor() {
    u_dword offset = get_cursor_offset();
    return (screen_coords) {offset % COLS_NUM + 1, offset / COLS_NUM};
}

void set_cursor(screen_coords coords) {
    set_cursor_offset(coords.row * COLS_NUM + coords.column);
}



static u_dword scroll (u_dword cursor_offset) {
    if (cursor_offset < COLS_NUM * ROWS_NUM * 2) return cursor_offset;

    for (int i = 1; i < ROWS_NUM; ++i) {
        byte* curr_row = (byte*) VIDEO_MEMORY + (i * COLS_NUM * 2);
        byte* prev_row = (byte*) VIDEO_MEMORY + ((i - 1) * COLS_NUM * 2);
        memory_copy(curr_row, prev_row, COLS_NUM * 2);
    }

    byte* last = (byte*) VIDEO_MEMORY + ((ROWS_NUM - 1) * COLS_NUM * 2);
    u_word sample = (BLACK << 12u) + (LIGHT_GRAY << 8u) + ' ';
    memory_fill(last, (byte *) &sample, 2, COLS_NUM);
    cursor_offset -= COLS_NUM * 2;

    return cursor_offset;
}



void print_char (char c) {
    print_char_color_at(c, (screen_coords) {-1, -1}, LIGHT_GRAY, BLACK);
}

void print_char_at (char c, screen_coords coords) {
    print_char_color_at(c, coords, LIGHT_GRAY, BLACK);
}

void print_char_color (char c, byte text_color, byte back_color) {
    print_char_color_at(c, (screen_coords) {-1, -1}, text_color, back_color);
}

void print_char_color_at (char c, screen_coords coords, byte text_color, byte back_color) {
    byte attributes = (back_color << 4u) + text_color;
    byte* vm = (byte*) VIDEO_MEMORY;

    u_dword mem_offset;
    if ((coords.column == -1) && (coords.row == -1)) mem_offset = get_cursor_offset() * 2;
    else mem_offset = coords.row * coords.column;

    if (c == '\n') {
        u_dword rows = (mem_offset / 2) / COLS_NUM + 1;
        mem_offset = 2 * COLS_NUM * rows - 2;
    } else if (c == '\b') {
        mem_offset -= 2;
        vm[mem_offset] = ' ';
        vm[mem_offset + 1] = attributes;
        mem_offset -= 2;
    } else if (c == '\t') {
        for (int i = 0; i < 4; ++i) {
            vm[mem_offset + (2 * i)] = ' ';
            vm[mem_offset + (2 * i) + 1] = attributes;
        }
        mem_offset += 6;
    } else {
        vm[mem_offset] = c;
        vm[mem_offset + 1] = attributes;
    }

    mem_offset += 2;
    mem_offset = scroll(mem_offset);
    set_cursor_offset(mem_offset / 2);
}



void print (c_string str) {
    print_color_at(str, (screen_coords) {-1, -1}, LIGHT_GRAY, BLACK);
}

void print_at (c_string str, screen_coords coords) {
    print_color_at(str, coords, LIGHT_GRAY, BLACK);
}

void print_color (c_string str, byte text_color, byte back_color) {
    print_color_at(str, (screen_coords) {-1, -1}, text_color, back_color);
}

void print_color_at (c_string str, screen_coords coords, byte text_color, byte back_color) {
    if ((coords.column != -1) && (coords.row != -1)) set_cursor(coords);

    int i = 0;
    while (str[i] != 0) {
        print_char_color(str[i], text_color, back_color);
        i++;
    }
}



void clear_screen() {
    u_word sample = (BLACK << 12) + (LIGHT_GRAY << 8) + ' ';
    memory_fill((byte*) VIDEO_MEMORY, (byte *) &sample, 2, COLS_NUM * ROWS_NUM);
    set_cursor((screen_coords) {0, 0});
}



static void print_atom(u_byte b, color front, color back, system sys) {
    if ((sys >= 2) && (sys <= 10)) print_char_color(b + 48, front, back);
    else if ((sys > 10) && (sys <= 36)) {
        if (b < 10) print_char_color(b + 48, front, back);
        else print_char_color(b + 65 - 10, front, back);
    } else print_char_color(undefined, front, back);
}

static void print_int(u_dword b, color front, color back, system sys) {
    if (b) {
        byte symbol = b % sys;
        print_int(b / sys, front, back, sys);
        print_atom(symbol, front, back, sys);
    }
}

static void print_float(float b, color front, color back, system sys, u_dword after_comma) {
    if (after_comma) {
        if (b > 1) {
            byte symbol = (u_dword) b % sys;
            print_float(b / (float) sys, front, back, sys, 4);
            print_atom(symbol, front, back, sys);
            if (b / (float) sys < 1) print_char_color('.', front, back);
        } else if (b) {
            b *= (float) sys;
            byte symbol = (u_dword) b % sys;
            print_float(b / (float) sys, front, back, sys, after_comma - 1);
            print_atom(symbol, front, back, sys);
        }
    }
}

static void print_boolean(boolean b, color front, color back) {
    if (b) print_color("true", front, back);
    else print_color("false", front, back);
}

static void print_pointer(u_dword b, color front, color back) {
    print_color(".", front, back);
    if (b == nullptr) print_color("null", front, back);
    else print_int(b, front, back, HEXADECIMAL);
}

void print_number(u_dword num, type t, system sys, color front, color back) {
    switch (sys) {
        case DECIMAL:
            break;
        case HEXADECIMAL:
            print_color("0x", front, back);
            break;
        case BINARY:
            print_color("0b", front, back);
            break;
        default:
            log(front, back, "(base %d) ", sys);
    }

    if (num == 0) print_atom(0, front, back, sys);
    else if (num < 0) {
        print_char_color('-', front, back);
        if (t == INTEGER) print_int(-num, front, back, sys);
        else print_float(-num, front, back, sys, 4);
    } else {
        if (t == INTEGER) print_int(num, front, back, sys);
        else print_float(num, front, back, sys, 4);
    }
}



void log(color text, color back, const char* template, ...) {
    u_dword temps = 3, length = 0;
    while (template[length]) {
        if (template[length] == '%') temps++;
        length++;
    }

    u_dword* args = malloc(temps * sizeof(u_dword));
    args_init(args)

    u_dword argumentor = 2;
    for (int j = 0; j < length; ++j)
        if (template[j] != '%') print_char_color(template[j], text, back);
        else if (j < length - 1) {
            argumentor++;
            j++;

            switch (template[j]) {
                case 'l':
                    print_boolean(args[argumentor], text, back);
                    break;
                case 'p':
                    print_pointer(args[argumentor], text, back);
                    break;
                case 'd':
                    print_number(args[argumentor], INTEGER, DECIMAL, text, back);
                    break;
                case 'h':
                    print_number(args[argumentor], INTEGER, HEXADECIMAL, text, back);
                    break;
                case 'b':
                    print_number(args[argumentor], INTEGER, BINARY, text, back);
                    break;
                case 'f':
                    print_number(args[argumentor], FLOAT, DECIMAL, text, back);
                    break;
                case 'c':
                    print_char_color(args[argumentor], text, back);
                    break;
                case 's':
                    print_color((c_string) args[argumentor], text, back);
                    break;
                default:
                    argumentor--;
                    j--;
                    break;
            }
        }

    free(args);
}
