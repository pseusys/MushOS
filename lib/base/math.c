#include "math.h"


u_dword min(u_dword first, u_dword second) {
    return first > second ? second : first;
}

u_dword max(u_dword first, u_dword second) {
    return first > second ? first : second;
}
