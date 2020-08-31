#ifndef MUSHFS_STRUCTURES_H
#define MUSHFS_STRUCTURES_H

#include "../presets.h"


typedef struct {
    int previous;
    int next;
} list_linker;

typedef struct {
    boolean is_occupied;
    list_linker linker;
    int head; // WARNING! For empty blocks this int is insignificant!
} block_header;

typedef struct {
    int magic_number;
    int empty_pages;
    list_linker empty;
    int root_page;
} system_header;



#define magic 0x10afc01
#define fs_header_offset 0x8400
#define page_size 512
#define page_content_size (signed int) (512 - sizeof(block_header))

typedef struct {
    byte* page;
    int current, offset;
} data_iterator;



#define max_path_len 256

typedef enum {
    READABLE, WRITABLE, DESTROYABLE, EXECUTABLE
} user_mode;

typedef struct {
    int size;
    user_mode property;
    boolean is_directory;
    int parent_page;
    char file_name [max_path_len];
} file_header;

typedef struct {
    file_header* header;
    int main_page;
    int absolute_offset;
    data_iterator* iterator;
} file;



#define delimiter '/'

typedef struct {
    int file_offset;
} directory_entry;

typedef enum {
    CREATE, DELETE
} action;

#endif //MUSHFS_STRUCTURES_H
