#include "modules.h"

#include "../../lib/base/stdio.h"
#include "../../lib/base/heap.h"

#include "../kernel/interruption_tables.h"


u_dword modules_placement = 0x30000;


extern boolean read_disk(void* address, boolean slave_drive, u_word length, u_word offset, void* buffer);

void init_module_loading_driver() {
    silence_interrupt(46);
}

void* place_module(u_dword size) {
    u_dword tmp = modules_placement;
    modules_placement += size;
    return (void*) tmp;
}

void* load_module(u_word offset, u_word length) {
    if (length == 0) ; // TODO: throw error: module shouldn't be empty
    if (length % 2 != 0) ; // TODO: throw error: module should be 2 bytes aligned
    if (offset % 2 != 0) ; // TODO: throw error: module should be 2 bytes aligned
    void* place = ralloc(length);
    info("Loading module at offset %d, length %d to memory address: %h\n", offset, length, place)
    boolean res = read_disk((void*) (offset / 512), false, length, offset, (u_word*) place);
    info("Reading result: %d\n", res)
    return place;
}
