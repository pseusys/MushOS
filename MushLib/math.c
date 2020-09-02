#include "math.h"

int m_ceil(int num, int denom) {
    int naive = num / denom;
    int rem = num % denom;
    if (rem > 0) return naive + 1;
    else return naive;
}
