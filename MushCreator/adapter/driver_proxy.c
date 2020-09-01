#include "driver_proxy.h"
#include "adapter.h"

int read_int(int offset) {
    int data = 0;
    data |= (read_at(offset) & 0xff);
    data |= ((read_at(offset + 1) & 0xff) << 8);
    data |= ((read_at(offset + 2) & 0xff) << 16);
    data |= ((read_at(offset + 3) & 0xff) << 24);
    return data;
}

void write_int(int offset, int data) {
    write_at(offset, data & 0xff);
    write_at(offset + 1, (data >> 8) & 0xff);
    write_at(offset + 2, (data >> 16) & 0xff);
    write_at(offset + 3, (data >> 24) & 0xff);
}



void read_struct(int offset, byte* data, int length) {
    for (int i = 0; i < length; ++i) {
        data[i] = read_at(offset + i);
    }
}

void write_struct(int offset, byte* data, int length) {
    for (int i = 0; i < length; ++i) {
        write_at(offset + i, data[i]);
    }
}
