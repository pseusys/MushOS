#include "file_system.h"
#include "../kernel/interruption_tables.h"
#include "../../MushLib/math.h"
#include "../../MushLib/stdio.h"


extern boolean read_fs(u_dword sector, boolean slave_drive, u_word len, u_byte sectors, u_word offset, u_word* buffer);


static void primary_drive_handler(registers* regs) {
    // we do not want to interact with this irq.
}

void init_simple_fs_driver() {
    set_interrupt_handler(46, primary_drive_handler);
}



boolean read_struct(int offset, byte* data, int length) {
    int page_offset = offset % 512;
    bad("Reading %d sectors at offset %d (%d bytes, %d offset)\n", m_ceil(length + 20, 512), offset, length, page_offset)
    if ((offset % 2) || (length % 2)) {
        bad("Data offset and length should be aligned to 2.\n")
        return false;
    }
    read_fs(offset / 512, false, length, m_ceil(length + 20, 512), page_offset, (u_word*) data);
    //bad("ERROR: %l\n", );
    return true;
}
