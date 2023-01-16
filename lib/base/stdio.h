#ifndef MUSHLIB_STDIO_H
#define MUSHLIB_STDIO_H

#include "generic.h"
#include "string.h"
#include "syscall.h"


typedef enum {
    BLACK        = 0u,
    LOW_BLUE     = 1u,
    LOW_GREEN    = 2u,
    LOW_CYAN     = 3u,
    LOW_RED      = 4u,
    LOW_MAGENTA  = 5u,
    BROWN        = 6u,
    LIGHT_GRAY   = 7u,
    DARK_GRAY    = 8u,
    HIGH_BLUE    = 9u,
    HIGH_GREEN   = 10u,
    HIGH_CYAN    = 11u,
    HIGH_RED     = 12u,
    HIGH_MAGENTA = 13u,
    YELLOW       = 14u,
    WHITE        = 15u
} console_color;

#define print_formatted(font, back, temp, args...) {\
    mod_string formatted = format(temp, ## args);\
    call_system(48, 5, 2, formatted, font, back, 0);\
    unalloc(formatted);\
}

#define bad(temp, args...) print_formatted(HIGH_RED, BLACK, temp, ## args)
#define warn(temp, args...) print_formatted(YELLOW, BLACK, temp, ## args)
#define good(temp, args...) print_formatted(HIGH_GREEN, BLACK, temp, ## args)
#define info(temp, args...) print_formatted(HIGH_BLUE, BLACK, temp, ## args)
#define plain(temp, args...) print_formatted(LIGHT_GRAY, BLACK, temp, ## args)

#endif // MUSHLIB_STDIO_H
