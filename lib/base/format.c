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
    mod_string str = malloc(2);
    str[0] = c;
    str[1] = 0;
    return str;
}

static mod_string const_to_string(string str) {
    u_dword length = len(str);
    mod_string mod = malloc(length + 1);
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

    mod_string result = malloc(power);
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

static string boolean_to_string(boolean b) {
    if (b) return "true";
    else return "false";
}

static mod_string float_to_string(precise b, u_dword after_comma) {
    if (after_comma) {
        mod_string integer = int_to_string((u_dword) b, DECIMAL);
        mod_string full = concatenate(integer, ".");
        free(integer);

        precise floating = b - (u_dword) b;
        for (u_dword i = 0; i < after_comma; i++) floating *= DECIMAL;
        mod_string part = int_to_string(floating, DECIMAL);
        mod_string result = concatenate(full, part);
        free(part);
        free(full);

        if (b < 0) {
            mod_string signum = concatenate("-", result);
            free(result);
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
        free(integer);
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
            free(number);
            break;
        case BINARY:
            result = concatenate("0b", number);
            free(number);
            break;
        default:
            mod_string prefix = format("(base %d) ", sys);
            result = concatenate(prefix, number);
            free(prefix);
            free(number);
            break;
    }

    if (num < 0) {
        mod_string signum = concatenate("-", result);
        free(result);
        result = signum;
    }

    return result;
}


// TODO: floating point numbers precision setup.
mod_string format(string template, ...) {
    mod_string result = "";
    u_dword argumentor = sizeof(string), printed = 0;
    for (int j = 0; j < len(template); ++j) {
        if (template[j] == '%') {
            mod_string unmod = substring_mid(template, printed, j);
            result = concatenate(result, unmod);
            free(unmod);

            printed = j + 2;
            j++;

            mod_string prefix = result;
            switch (template[j]) {
                case 'l':
                    result = concatenate(prefix, boolean_to_string(get_arg(argumentor, boolean)));
                    argumentor += sizeof(boolean);
                    break;
                case 'p':
                    mod_string pointer = pointer_to_string(get_arg(argumentor, void*));
                    result = concatenate(prefix, pointer);
                    free(pointer);
                    argumentor += sizeof(void*);
                    break;
                case 'd':
                    mod_string dec_number = number_to_string(get_arg(argumentor, u_dword), DECIMAL);
                    result = concatenate(prefix, dec_number);
                    free(dec_number);
                    argumentor += sizeof(u_dword);
                    break;
                case 'h':
                    mod_string hex_number = number_to_string(get_arg(argumentor, u_dword), HEXADECIMAL);
                    result = concatenate(prefix, hex_number);
                    free(hex_number);
                    argumentor += sizeof(u_dword);
                    break;
                case 'b':
                    mod_string bin_number = number_to_string(get_arg(argumentor, u_dword), BINARY);
                    result = concatenate(prefix, bin_number);
                    free(bin_number);
                    argumentor += sizeof(u_dword);
                    break;
                case 'f':
                    mod_string flo_number = float_to_string(get_arg(argumentor, precise), 4);
                    result = concatenate(prefix, flo_number);
                    free(flo_number);
                    argumentor += sizeof(precise);
                    break;
                case 'c':
                    mod_string character = char_to_string(get_arg(argumentor, char));
                    result = concatenate(prefix, character);
                    free(character);
                    argumentor += sizeof(char);
                    break;
                case 's':
                    result = concatenate(prefix, (get_arg(argumentor, string)));
                    argumentor += sizeof(string);
                    break;
                default:
                    argumentor--;
                    j--;
                    printed -= 2;
                    break;
            }
            free(prefix);

            argumentor++;
        }
    }

    mod_string unmod = substring_beg(template, printed);
    result = concatenate(result, unmod);
    free(unmod);

    return result;
}
