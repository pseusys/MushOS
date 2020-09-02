#ifndef MUSHFS_DRIVER_PROXY_H
#define MUSHFS_DRIVER_PROXY_H

#include "../../MushLib/generic.h"

void read_struct(int offset, byte* data, int length);
void write_struct(int offset, byte* data, int length);

#endif //MUSHFS_DRIVER_PROXY_H
