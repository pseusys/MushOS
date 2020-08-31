#include "string.h"

u_dword len(char* str) {
    u_dword pos = 0;
    while (str[pos]) pos++;
    return pos;
}
