#ifndef MUSHFS_FILE_OUTPUT_H
#define MUSHFS_FILE_OUTPUT_H

#include "../../MushLib/generic.h"
#include "structures.h"
#include "../../MushLib/string.h"


int format_drive();

void set_file_header(data_iterator* iterator, byte* container, int length);

data_iterator* create_data();
void destroy_data(data_iterator* iterator);

void add_page(data_iterator* iterator);
void delete_page(data_iterator* iterator);
void truncate(data_iterator* iterator, int bytes);

void set_next_bytes(data_iterator* iterator, const byte* container, int length);
void set_previous_bytes(data_iterator* iterator, const byte* container, int length);



file* create_file(string name, int parent);
void delete_file(file* f);
void flush_file(file* f);

void append_file(file* f, byte* data, int size);
void truncate_file(file* f, int bytes);
void write_file(file* f, byte* bytes, int size);

void write_bytes(file* f, byte* bytes, int size, int offset);



file* create_file_global(string path);
void delete_file_global(string path);

#endif //MUSHFS_FILE_OUTPUT_H
