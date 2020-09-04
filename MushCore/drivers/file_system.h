#ifndef MUSHOS_FILE_SYSTEM_H
#define MUSHOS_FILE_SYSTEM_H

#include "../../MushLib/generic.h"

void init_simple_fs_driver();

boolean read_struct(int offset, byte* data, int length);

void read_sys();

#endif //MUSHOS_FILE_SYSTEM_H
