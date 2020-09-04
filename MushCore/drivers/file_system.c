#include "file_system.h"
#include "../kernel/interruption_tables.h"
#include "../../MushLib/math.h"


extern boolean read_fs(u_dword sector, boolean slave_drive, u_word len, u_byte sectors, u_word offset, u_word* buffer);


static void primary_drive_handler(registers* regs) {
    // we do not want to interact with this irq.
}

void init_simple_fs_driver() {
    set_interrupt_handler(46, primary_drive_handler);
}



boolean read_struct(int offset, byte* data, int length) {
    if ((offset % 2) || (length % 2)) return false;
    return read_fs(offset / 512, false, length, m_ceil(length, 512), offset % 512, (u_word*) data);
}
