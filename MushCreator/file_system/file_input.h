#ifndef MUSHFS_FILE_INPUT_H
#define MUSHFS_FILE_INPUT_H

#include "../../MushCore/kernel/generic.h"
#include "structures.h"
#include "../utils/string_utils.h"


int get_page_offset(int page_num);
int get_page_content_offset(int page_num);

boolean check_drive();
void check_root();

int get_next_block_number(int current);
int get_previous_block_number(int current);
int get_file_block_head_number(int file_page);

boolean data_step(data_iterator* iterator, boolean flush_page);

int get_root_dir_number();
boolean is_eof(data_iterator* iterator);
boolean set_offset(data_iterator* iterator, int skip);
boolean add_offset(data_iterator* iterator, int skip);
void reset_and_reload(data_iterator* iterator);

void get_file_header(data_iterator* iterator, byte* container, int length);
void get_file_page_header(int main_page, byte* container, int length);

data_iterator* load_data(int main_page);
void dismiss_data(data_iterator* container);

boolean get_next_bytes(data_iterator* iterator, byte* container, int length);
boolean get_previous_bytes(data_iterator* iterator, byte* container, int length);



file* open_file(int page);
void close_file(file* f);

void seek_to(file* f, int absolute_position);
void seek_by(file* f, int absolute_position);
void read_bytes(file* f, byte* bytes, int size, int offset);



file* find_file_in_dir(string file_name, file* dir);
file* open_file_global(string path);
file* get_root_dir();

boolean file_exists(string path);

int num_files(string path);
mod_string* list_files(string dir_path);

mod_string get_name(file* f);
mod_string get_path(file* f);
mod_string get_extension(file* f);

#endif //MUSHFS_FILE_INPUT_H
