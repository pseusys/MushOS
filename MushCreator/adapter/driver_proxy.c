#include "driver_proxy.h"
#include "adapter.h"

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
