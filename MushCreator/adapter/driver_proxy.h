#ifndef MUSHFS_DRIVER_PROXY_H
#define MUSHFS_DRIVER_PROXY_H

#include "../../MushCore/kernel/generic.h"

int read_int(int offset);
void write_int(int offset, int data);

void read_struct(int offset, byte* data, int length);
void write_struct(int offset, byte* data, int length);

#endif //MUSHFS_DRIVER_PROXY_H
