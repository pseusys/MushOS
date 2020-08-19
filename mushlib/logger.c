#include "logger.h"

static const char undefined = '?';

static void print_atom(byte b, color front, color back, system sys) {
    if ((sys >= 2) && (sys <= 10)) print_char_color(b + 48, front, back);
    else if ((sys > 10) && (sys <= 36)) {
        if (b < 10) print_char_color(b + 48, front, back);
        else print_char_color(b + 65 - 10, front, back);
    } else print_char_color(undefined, front, back);
}

static void print_num(u_dword b, color front, color back, system sys) {
    if (b) {
        byte symbol = b % sys;
        print_num(b / sys, front, back, sys);
        print_atom(symbol, front, back, sys);
    } else print_atom(0, front, back, sys);
}

static void print_boolean(boolean b, color front, color back, system sys) {
    if (sys == DECIMAL) {
        if (b) print_color("true", front, back);
        else print_color("false", front, back);
    } else {
        if (b) print_char_color('1', front, back);
        else print_char_color('0', front, back);
    }
}

static void print_pointer(u_dword b, color front, color back, system sys) {
    print_color(".", front, back);
    print_number(b, UNSIGNED_DOUBLE_WORD, sys, front, back);
}



void print_number(dword num, mode m, system sys, color front, color back) {
    switch (m) {
        case BOOLEAN:
            print_boolean(num, front, back, sys);
            break;

        case BYTE:
            if (num < 0) print_char_color('-', front, back);
            print_number(-num, UNSIGNED_BYTE, sys, front, back);
            break;

        case WORD:
            if (num < 0) print_char_color('-', front, back);
            print_number(-num, UNSIGNED_WORD, sys, front, back);
            break;

        case DOUBLE_WORD:
            if (num < 0) print_char_color('-', front, back);
            print_number(-num, UNSIGNED_DOUBLE_WORD, sys, front, back);
            break;

        case UNSIGNED_BYTE:
        case UNSIGNED_WORD:
        case UNSIGNED_DOUBLE_WORD:
            print_num(num, front, back, sys);
            break;

        case POINTER:
            print_pointer(num, front, back, sys);
            break;

        default:
            print_char_color(undefined, front, back);
            break;
    }
}
