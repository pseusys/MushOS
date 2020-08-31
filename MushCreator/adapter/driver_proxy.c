#include "driver_proxy.h"
#include "adapter.h"

int read_int(int offset) {
    int data = 0;
    data |= read_at(offset);
    data |= (read_at(offset + 1) << 8);
    data |= (read_at(offset + 2) << 16);
    data |= (read_at(offset + 3) << 24);
    return data;
}

void write_int(int offset, int data) {
    write_at(offset, data & 0x000000ff);
    write_at(offset + 1, (data & 0x0000ff00) >> 8);
    write_at(offset + 2, (data & 0x00ff0000) >> 16);
    write_at(offset + 3, (data & 0xff000000) >> 24);
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
