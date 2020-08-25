#include "logger.h"
#include "vararg.h"

static const char undefined = '?';

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
