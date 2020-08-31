#include "math_utils.h"

int m_ceil(int num, int denom) {
    int naive = num / denom;
    float precise = (float) num / (float) denom;
    if (precise > (float) naive) return naive + 1;
    else return naive;
}
