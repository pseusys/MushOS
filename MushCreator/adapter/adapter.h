#ifndef MUSHFS_WINDOWS_H
#define MUSHFS_WINDOWS_H

#include "../presets.h"

void create_fs();

int read_out_file(char* name, byte* raw_bytes);
int write_out_file(char* name, byte* raw_bytes, int size);

void write_at(int pos, byte info);
byte read_at(int pos);

long get_drive_size();


#endif //MUSHFS_WINDOWS_H
