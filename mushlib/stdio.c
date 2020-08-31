#include "stdio.h"
#include "heap.h"
#include "vararg.h"
#include "syscall.h"
#include "../drivers/screen.h"

#define undefined '?'

typedef enum {
    DECIMAL = 10, HEXADECIMAL = 16, BINARY = 2
} system;

typedef enum {
    INTEGER, FLOAT
} type;



void print_char(char c, console_color front, console_color back) {
    orbit(48, 4, back, front, c, 1)
}

void print_color (c_string str, byte text_color, byte back_color) {
    orbit(48, 4, back_color, text_color, str, 2)
}

static void print_atom(u_byte b, u_dword front, u_dword back, system sys) {
    if ((sys >= 2) && (sys <= 10)) print_char(b + 48, front, back);
    else if ((sys > 10) && (sys <= 36)) {
        if (b < 10) print_char(b + 48, front, back);
        else print_char(b + 65 - 10, front, back);
    } else print_char(undefined, front, back);
}

static void print_int(u_dword b, u_dword front, u_dword back, system sys) {
    if (b) {
        byte symbol = b % sys;
        print_int(b / sys, front, back, sys);
        print_atom(symbol, front, back, sys);
    }
}

static void print_float(float b, u_dword front, u_dword back, system sys, u_dword after_comma) {
    if (after_comma) {
        if (b > 1) {
            byte symbol = (u_dword) b % sys;
            print_float(b / (float) sys, front, back, sys, 4);
            print_atom(symbol, front, back, sys);
            if (b / (float) sys < 1) print_char('.', front, back);
        } else if (b) {
            b *= (float) sys;
            byte symbol = (u_dword) b % sys;
            print_float(b / (float) sys, front, back, sys, after_comma - 1);
            print_atom(symbol, front, back, sys);
        }
    }
}

static void print_boolean(boolean b, u_dword front, u_dword back) {
    if (b) print_color("true", front, back);
    else print_color("false", front, back);
}

static void print_pointer(u_dword b, u_dword front, u_dword back) {
    print_color(".", front, back);
    if (b == nullptr) print_color("null", front, back);
    else print_int(b, front, back, HEXADECIMAL);
}

void print_number(u_dword num, type t, system sys, u_dword front, u_dword back) {
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
            print_colored(front, back, "(base %d) ", sys);
    }

    if (num == 0) print_atom(0, front, back, sys);
    else if (num < 0) {
        print_char('-', front, back);
        if (t == INTEGER) print_int(-num, front, back, sys);
        else print_float(-num, front, back, sys, 4);
    } else {
        if (t == INTEGER) print_int(num, front, back, sys);
        else print_float(num, front, back, sys, 4);
    }
}



void print_with_arguments() {

}

void print(c_string format, ...) {

}

void print_colored(console_color font, console_color back, c_string format, ...) {
    u_dword temps = 0, length = 0;
    while (format[length]) {
        if (format[length] == '%') temps++;
        length++;
    }

    if (temps == 0) {
        print_color(format, font, back);
        return;
    }

    u_dword* args = malloc(temps * sizeof(u_dword));
    args_init_from(args, sizeof(console_color) * 2 + sizeof(c_string))

    u_dword argumentor = 0;
    for (int j = 0; j < length; ++j)
        if (format[j] != '%') print_char(format[j], font, back);
        else if (j < length - 1) {
            j++;
            switch (format[j]) {
                case 'l':
                    print_boolean(args[argumentor], font, back);
                    break;
                case 'p':
                    print_pointer(args[argumentor], font, back);
                    break;
                case 'd':
                    print_number(args[argumentor], INTEGER, DECIMAL, font, back);
                    break;
                case 'h':
                    print_number(args[argumentor], INTEGER, HEXADECIMAL, font, back);
                    break;
                case 'b':
                    print_number(args[argumentor], INTEGER, BINARY, font, back);
                    break;
                case 'f':
                    print_number(args[argumentor], FLOAT, DECIMAL, font, back);
                    break;
                case 'c':
                    print_char(args[argumentor], font, back);
                    break;
                case 's':
                    print_color((c_string) args[argumentor], font, back);
                    break;
                default:
                    argumentor--;
                    j--;
                    break;
            }
            argumentor++;
        }

    free(args);
}
