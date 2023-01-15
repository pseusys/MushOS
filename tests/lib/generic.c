#include <mushlib.h>

#include <assert.h>
#include <stdio.h>

#include "../integrator.h"


int main() {
    printf("u_byte length (should be 1): %d\n", sizeof(u_byte));
    assert(sizeof(u_byte) == 1);
    printf("byte length (should be 1): %d\n", sizeof(byte));
    assert(sizeof(byte) == 1);

    printf("u_word length (should be 2): %d\n", sizeof(u_word));
    assert(sizeof(u_word) == 2);
    printf("word length (should be 2): %d\n", sizeof(word));
    assert(sizeof(word) == 2);

    printf("u_dword length (should be 4): %d\n", sizeof(u_dword));
    assert(sizeof(u_dword) == 4);
    printf("dword length (should be 4): %d\n", sizeof(dword));
    assert(sizeof(dword) == 4);

    printf("u_qword length (should be 8): %d\n", sizeof(u_qword));
    assert(sizeof(u_qword) == 8);
    printf("qword length (should be 8): %d\n", sizeof(qword));
    assert(sizeof(qword) == 8);

    printf("real length (should be 4): %d\n", sizeof(real));
    assert(sizeof(real) == 4);
    printf("precise length (should be 8): %d\n", sizeof(precise));
    assert(sizeof(precise) == 8);
    printf("exact length (should be 12): %d\n", sizeof(exact));
    assert(sizeof(exact) == 12);

    printf("pointer length (should be 4): %d\n", sizeof(void*));
    assert(sizeof(void*) == 4);
    printf("string length (should be 4): %d\n", sizeof(string));
    assert(sizeof(string) == 4);
    printf("mod_string length (should be 4): %d\n", sizeof(mod_string));
    assert(sizeof(mod_string) == 4);

    printf("boolean length (should be 1): %d\n", sizeof(boolean));
    assert(sizeof(boolean) == 1);
    printf("true value (should be 1): %d\n", true);
    assert(true == 1);
    printf("false value (should be 0): %d\n", false);
    assert(false == 0);

    printf("nullptr value (should be 0): %d\n", nullptr);
    assert(nullptr == 0);

    return 0;
}
