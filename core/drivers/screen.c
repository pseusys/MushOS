#include "screen.h"
#include "ports_io.h"
#include <mushlib.h>
#include "../kernel/interruption_tables.h"

#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#define VIDEO_MEMORY 0xb8000

#define BLACK 0u
#define LIGHT_GRAY 7u

#define ROWS_NUM 25
#define COLS_NUM 80

typedef struct {
    word row;
    word column;
} screen_coords;



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

static screen_coords get_cursor() {
    u_dword offset = get_cursor_offset();
    return (screen_coords) {offset % COLS_NUM + 1, offset / COLS_NUM};
}

static void set_cursor(screen_coords coords) {
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



void print_char_color (char c, byte text_color, byte back_color) {
    byte attributes = (back_color << 4u) + text_color;
    byte* vm = (byte*) VIDEO_MEMORY;
    u_dword mem_offset = get_cursor_offset() * 2;

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

char get_char(screen_coords coords) {
    byte* vm = (byte*) VIDEO_MEMORY;
    return vm[coords.row * coords.column];
}



void print_string_color(string str, byte text_color, byte back_color, u_dword length) {
    if (length == 0) for (int i = 0; str[i] != 0; ++i) print_char_color(str[i], text_color, back_color);
    else for (int i = 0; i < length; ++i) print_char_color(str[i], text_color, back_color);
}



void clear_screen() {
    u_word sample = (BLACK << 12) + (LIGHT_GRAY << 8) + ' ';
    memory_fill((byte*) VIDEO_MEMORY, (byte *) &sample, 2, COLS_NUM * ROWS_NUM);
    set_cursor((screen_coords) {0, 0});
}



typedef enum {
    GET_CHAR = 0, SET_CHAR = 1, SET_CHARS = 2, GET_CURSOR = 3, SET_CURSOR = 4, CLEAR_SCREEN = 5
} call_type;

static void screen_callback(registers* regs) {
    u_dword call_type, ret_value = 0;
    orbital_get_arg(regs->ebp, 0, call_type)
    switch (call_type) {
        case GET_CHAR: {
            ret_value = get_char((screen_coords) {0, 0});
            break;
        }
        case SET_CHAR: {
            u_dword character, front_color, back_color;
            orbital_get_arg(regs->ebp, 1, character)
            orbital_get_arg(regs->ebp, 2, front_color)
            orbital_get_arg(regs->ebp, 3, back_color)
            print_char_color(character, front_color, back_color);
            break;
        }
        case SET_CHARS: {
            u_dword str, front_color, back_color, length;
            orbital_get_arg(regs->ebp, 1, str)
            orbital_get_arg(regs->ebp, 2, front_color)
            orbital_get_arg(regs->ebp, 3, back_color)
            orbital_get_arg(regs->ebp, 4, length)
            print_string_color((mod_string) str, front_color, back_color, length);
        }
        case GET_CURSOR: {
            ret_value = get_cursor_offset();
            break;
        }
        case SET_CURSOR: {
            u_dword offset;
            orbital_get_arg(regs->ebp, 1, offset)
            set_cursor_offset(offset);
            break;
        }
        case CLEAR_SCREEN:
            clear_screen();
            break;
        default:
            break;
    }
    orbital_push(regs->ebp, ret_value)
}



void init_screen_io() {
    set_interrupt_handler(48, screen_callback);
}
