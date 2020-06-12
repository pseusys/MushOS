#include "screen.h"
#include "ports_io.h"
#include "../mushlib/memory.h"

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
    u_word sample = (BLACK << 12) + (LIGHT_GRAY << 8) + ' ';
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
    byte attributes = (back_color << 4) + text_color;
    byte* vm = (byte*) VIDEO_MEMORY;

    u_dword mem_offset;
    if ((coords.column == -1) && (coords.row == -1)) mem_offset = get_cursor_offset() * 2;
    else mem_offset = coords.row * coords.column;

    if (c == '\n') {
        u_dword rows = (mem_offset / 2) / COLS_NUM + 1;
        mem_offset = 2 * COLS_NUM * rows - 2;
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
