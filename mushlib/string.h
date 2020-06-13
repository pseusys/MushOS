#ifndef MUSHOS_STRING_H
#define MUSHOS_STRING_H

#include "../kernel/generic.h"

typedef struct {
    u_dword size;
    char* raw;
} string;

string* str(c_string c_str);

#endif //MUSHOS_STRING_H
