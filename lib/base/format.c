#include "string.h"

#include "heap.h"
#include "vararg.h"


#define undefined '?'

typedef enum {
    DECIMAL = 10, HEXADECIMAL = 16, BINARY = 2
} system;

typedef enum {
    INTEGER, FLOAT
} type;


static mod_string char_to_string(char c) {
    mod_string str = ralloc(2);
    str[0] = c;
    str[1] = 0;
    return str;
}

static mod_string const_to_string(string str) {
    u_dword length = len(str);
    mod_string mod = ralloc(length + 1);
    for (u_dword i = 0; i < length; i++) mod[i] = str[i];
    mod[length] = 0;
    return mod;
}


static char atom_to_char(u_byte b, system sys) {
    if ((sys >= 2) && (sys <= 10)) return b + 48;
    else if ((sys > 10) && (sys <= 36)) {
        if (b < 10) return b + 48;
        else return b + 65 - 10;
    } else return undefined;
}

static mod_string int_to_string(u_dword b, system sys) {
    u_dword mult = sys, power = 1;
    while (mult <= b) {
        mult *= sys;
        power++;
    }

    mod_string result = ralloc(power);
    result[power] = 0;

    u_dword integer = b;
    while (integer) {
        power--;
        byte symbol = integer % sys;
        integer /= sys;
        result[power] = atom_to_char(symbol, sys);
    }
    return result;
}

static mod_string boolean_to_string(boolean b) {
    if (b) return const_to_string("true");
    else return const_to_string("false");
}

static mod_string float_to_string(precise b, u_dword after_comma) {
    if (after_comma) {
        mod_string integer = int_to_string((u_dword) b, DECIMAL);
        mod_string full = concatenate(integer, ".");
        unalloc(integer);

        precise floating = b - (u_dword) b;
        for (u_dword i = 0; i < after_comma; i++) floating *= DECIMAL;
        mod_string part = int_to_string(floating, DECIMAL);
        mod_string result = concatenate(full, part);
        unalloc(part);
        unalloc(full);

        if (b < 0) {
            mod_string signum = concatenate("-", result);
            unalloc(result);
            result = signum;
        }
        return result;
    } else return int_to_string((u_dword) b, DECIMAL);
}

static mod_string pointer_to_string(void* ptr) {
    if (ptr == nullptr) return const_to_string(".null");
    else {
        mod_string integer = int_to_string((u_dword) ptr, HEXADECIMAL);
        mod_string result = concatenate(".", integer);
        unalloc(integer);
        return result;
    }
}

static mod_string number_to_string(dword num, system sys) {
    mod_string number;
    if (num == 0) number = char_to_string(atom_to_char(0, sys));
    else if (num < 0) number = int_to_string(-num, sys);
    else number = int_to_string(num, sys);

    mod_string result;
    switch (sys) {
        case DECIMAL:
            result = number;
            break;
        case HEXADECIMAL:
            result = concatenate("0x", number);
            unalloc(number);
            break;
        case BINARY:
            result = concatenate("0b", number);
            unalloc(number);
            break;
        default:
            mod_string prefix = format("(base %d) ", sys);
            result = concatenate(prefix, number);
            unalloc(prefix);
            unalloc(number);
            break;
    }

    if (num < 0) {
        mod_string signum = concatenate("-", result);
        unalloc(result);
        result = signum;
    }

    return result;
}


static mod_string free_concat(mod_string string1, mod_string string2) {
    mod_string result = concatenate(string1, string2);
    unalloc(string1);
    unalloc(string2);
    return result;
};

mod_string format(string template, ...) {
    mod_string result = (mod_string) zalloc(1);
    u_dword argumentor = sizeof(string), printed = 0;
    for (int j = 0; j < len(template); ++j) {
        if (template[j] == '%') {
            result = free_concat(result, substring_mid(template, printed, j));
            printed = j + 2;
            j++;

            switch (template[j]) {
                case 'l':
                    result = free_concat(result, boolean_to_string(get_arg(argumentor, boolean)));
                    argumentor += sizeof(boolean);
                    break;
                case 'p':
                    result = free_concat(result, pointer_to_string(get_arg(argumentor, void*)));
                    argumentor += sizeof(void*);
                    break;
                case 'd':
                    result = free_concat(result, number_to_string(get_arg(argumentor, u_dword), DECIMAL));
                    argumentor += sizeof(u_dword);
                    break;
                case 'h':
                    result = free_concat(result, number_to_string(get_arg(argumentor, u_dword), HEXADECIMAL));
                    argumentor += sizeof(u_dword);
                    break;
                case 'b':
                    result = free_concat(result, number_to_string(get_arg(argumentor, u_dword), BINARY));
                    argumentor += sizeof(u_dword);
                    break;
                case 'f':
                    result = free_concat(result, float_to_string(get_arg(argumentor, precise), 4));
                    argumentor += sizeof(precise);
                    break;
                case 'c':
                    result = free_concat(result, char_to_string(get_arg(argumentor, char)));
                    argumentor += sizeof(char);
                    break;
                case 's':
                    result = free_concat(result, const_to_string(get_arg(argumentor, string)));
                    argumentor += sizeof(string);
                    break;
                default:
                    j--;
                    printed -= 2;
                    break;
            }
        }
    }

    return free_concat(result, substring_beg(template, printed));
}
