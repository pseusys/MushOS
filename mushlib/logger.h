#ifndef MUSHOS_LOGGER_H
#define MUSHOS_LOGGER_H

#include "../kernel/generic.h"
#include "../drivers/screen.h"

typedef enum {
    BOOLEAN, UNSIGNED_BYTE, BYTE, UNSIGNED_WORD, WORD, UNSIGNED_DOUBLE_WORD, DOUBLE_WORD, POINTER
} mode;

typedef enum {
    DECIMAL = 10, HEXADECIMAL = 16, BINARY = 2
} system;

void print_number(dword num, mode m, system sys, color front, color back);

void log(char* template, ...);

// SUPPORT MACROS
#define endl() {\
    print_char('\n');\
}
#define fwl(color) {\
    print_color(" - ", color, BLACK);\
}
#define lwm(color) {\
    print_color(" : ", color, BLACK);\
}

// TYPE MACROS
#define point(message, color) {\
    print_color(__FILE__, color, BLACK);\
    fwl(color);\
    print_number(__LINE__, UNSIGNED_DOUBLE_WORD, DECIMAL, color, BLACK);\
    lwm(color);\
    print_color(message, color, BLACK);\
    endl();\
}
#define pointb(color) {\
    print_color(__FILE__, color, BLACK);\
    fwl(color);\
    print_number(__LINE__, UNSIGNED_DOUBLE_WORD, DECIMAL, color, BLACK);\
    lwm(color);\
}
#define pointm(message, color) {\
    print_color(message, color, BLACK);\
}
#define pointn(number, sys, mod, color) {\
    print_number(number, mod, sys, color, BLACK);\
}

#define some(message, color) {\
    print_color(message, color, BLACK);\
    endl();\
}
#define somem(message, color) {\
    print_color(message, color, BLACK);\
}
#define somen(number, mod, sys, color) {\
    print_number(number, mod, sys, color, BLACK);\
}



// BAD MACROS
#define b(message) {\
    point(message, HIGH_RED);\
}
#define bb() {\
    pointb(HIGH_RED);\
}
#define bm(message) {\
    pointm(message, HIGH_RED);\
}
#define bn(number, sys, mod) {\
    pointn(number, sys, mod, HIGH_RED);\
}

// WARNING MACROS
#define w(message) {\
    point(message, YELLOW);\
}
#define wb() {\
    pointb(YELLOW);\
}
#define wm(message) {\
    pointm(message, YELLOW);\
}
#define wn(number, sys, mod) {\
    pointn(number, sys, mod, YELLOW);\
}

// INFORMATION MACROS
#define i(message) {\
    some(message, HIGH_BLUE);\
}
#define im(message) {\
    somem(message, HIGH_BLUE);\
}
#define in(number, sys, mod) {\
    somen(number, mod, sys, HIGH_BLUE);\
}

// GOOD MACROS
#define g(message) {\
    some(message, HIGH_GREEN);\
}
#define gm(message) {\
    somem(message, HIGH_GREEN);\
}
#define gn(number, sys, mod) {\
    somen(number, mod, sys, HIGH_GREEN);\
}

#endif //MUSHOS_LOGGER_H
