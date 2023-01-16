#include <mushlib.h>

#include <assert.h>
#include <stdio.h>

#include "../integrator.h"


int main() {
    void* heap = allocate_space_for_heap(heap_size);
    initialize_heap(heap, heap_size);
    mod_string str;

    str = format("String with no placeholders.\n");
    printf(str);
    assert(equals(str, "String with no placeholders.\n"));
    unalloc(str);

    str = format("Logical value: %l.\n", true);
    printf(str);
    assert(equals(str, "Logical value: true.\n"));
    unalloc(str);

    str = format("Null pointer value: %p.\n", nullptr);
    printf(str);
    assert(equals(str, "Null pointer value: .null.\n"));
    unalloc(str);

    void* pointer = (void*) 0x10000;
    str = format("Pointer value: %p.\n", pointer);
    printf(str);
    assert(equals(str, "Pointer value: .10000.\n"));
    unalloc(str);

    str = format("Decimal value: %d.\n", 1024);
    printf(str);
    assert(equals(str, "Decimal value: 1024.\n"));
    unalloc(str);

    str = format("Hexadecimal value: %h.\n", 0x8000);
    printf(str);
    assert(equals(str, "Hexadecimal value: 0x8000.\n"));
    unalloc(str);

    str = format("Binary value: %b.\n", 0b10111);
    printf(str);
    assert(equals(str, "Binary value: 0b10111.\n"));
    unalloc(str);

    str = format("Float value: %f.\n", 3.1415);
    printf(str);
    assert(equals(str, "Float value: 3.1415.\n"));
    unalloc(str);

    str = format("Character value: %c.\n", 'g');
    printf(str);
    assert(equals(str, "Character value: g.\n"));
    unalloc(str);

    str = format("String value: %s.\n", "Some string");
    printf(str);
    assert(equals(str, "String value: Some string.\n"));
    unalloc(str);

    str = format("Two string values: %s, %s.\n", "First string", "Second string");
    printf(str);
    assert(equals(str, "Two string values: First string, Second string.\n"));
    unalloc(str);

    check_heap_for_leaks();
    return 0;
}
