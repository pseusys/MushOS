#ifndef MUSHOS_STDIO_H
#define MUSHOS_STDIO_H

#include "../kernel/generic.h"


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

void print_colored(console_color font, console_color back, c_string format, ...);



#define bad(temp, args...) print_colored(HIGH_RED, BLACK, temp, ## args);
#define warn(temp, args...) print_colored(YELLOW, BLACK, temp, ## args);
#define good(temp, args...) print_colored(HIGH_GREEN, BLACK, temp, ## args);
#define info(temp, args...) print_colored(HIGH_BLUE, BLACK, temp, ## args);
#define plain(temp, args...) print_colored(LIGHT_GRAY, BLACK, temp, ## args);

#define locate(text, back) print_colored(text, back, "%s - %d: ", __FILE__, __LINE__);

#define error(temp, args...) {locate(HIGH_RED, BLACK) bad(temp, ## args)}
#define warning(temp, args...) {locate(YELLOW, BLACK) warn(temp, ## args)}

#endif //MUSHOS_STDIO_H
