#ifndef MUSHOS_LOGGER_H
#define MUSHOS_LOGGER_H

#include "../kernel/generic.h"
#include "../drivers/screen.h"

typedef enum {
    DECIMAL = 10, HEXADECIMAL = 16, BINARY = 2
} system;

typedef enum {
    INTEGER, FLOAT
} type;

void print_number(float num, type t, system sys, color front, color back);

void log(color text, color back, const char* template, ...);

#define bad(temp, args...) log(HIGH_RED, BLACK, temp, ## args);
#define warn(temp, args...) log(YELLOW, BLACK, temp, ## args);
#define good(temp, args...) log(HIGH_GREEN, BLACK, temp, ## args);
#define info(temp, args...) log(HIGH_BLUE, BLACK, temp, ## args);
#define plain(temp, args...) log(LIGHT_GRAY, BLACK, temp, ## args);

#define locate(text, back) log(text, back, "%s - %d: ", __FILE__, __LINE__);

#define error(temp, args...) {locate(HIGH_RED, BLACK) bad(temp, ## args)}
#define warning(temp, args...) {locate(YELLOW, BLACK) warn(temp, ## args)}

#endif //MUSHOS_LOGGER_H
