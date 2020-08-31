#include "file_output.h"
#include "file_input.h"
#include "../windows_adapter/driver_proxy.h"
#include "../windows_adapter/windows.h"
#include <stdlib.h>
#include <stdio.h>


// Call to occupy a block, it returns the block-to-occupy number;
int occupy_block() {
    system_header* header = malloc(sizeof(system_header));
    read_struct(fs_header_offset, (byte*) header, sizeof(system_header));

    int free_block = header->empty.previous;
    header->empty.previous = get_next_block_number(header->empty.previous);
    header->empty_pages--;
    write_struct(fs_header_offset, (byte *) header, sizeof(system_header));

    block_header *block = malloc(sizeof(block_header));
    read_struct(get_page_offset(header->empty.previous), (byte *) block, sizeof(block_header));
    block->linker.previous = -1;
    write_struct(get_page_offset(header->empty.previous), (byte *) block, sizeof(block_header));

    free(block);
    free(header);
    return free_block;
}

// Call to free a block with free_block = freed block number;
void free_block(int free_block) {
    system_header* header = malloc(sizeof(system_header));
    read_struct(fs_header_offset, (byte*) header, sizeof(system_header));

    int previous_last = header->empty.next;
    header->empty.next = free_block;
    header->empty_pages++;
    write_struct(fs_header_offset, (byte *) header, sizeof(system_header));

    block_header *block = malloc(sizeof(block_header));
    read_struct(get_page_offset(previous_last), (byte *) block, sizeof(block_header));
    block->linker.next = free_block;
    write_struct(get_page_offset(previous_last), (byte *) block, sizeof(block_header));

    read_struct(get_page_offset(header->empty.next), (byte *) block, sizeof(block_header));
    block->is_occupied = false;
    block->linker.previous = previous_last;
    block->linker.next = -1;
    write_struct(get_page_offset(header->empty.next), (byte *) block, sizeof(block_header));

    free(block);
    free(header);
}



int format_drive() {
    long available_size = get_drive_size() - fs_header_offset;
    if (available_size <= 0) return 1;

    system_header* header = malloc(sizeof(system_header));
    header->magic_number = magic;
    header->root_page = fs_header_offset + sizeof(system_header);
    header->empty_pages = available_size / page_size - 1;
    header->empty.previous = 0;
    header->empty.next = header->empty_pages - 1;
    write_struct(fs_header_offset, (byte*) header, sizeof(system_header));

    block_header* empty_block = malloc(sizeof(block_header));
    empty_block->is_occupied = false;
    int percent = (header->empty.next - header->empty.previous) / 100;
    int percent_counter = 0, perc = 0;
    for (int i = header->empty.previous; i < header->empty.next; ++i) {
        if (perc == percent) {
            perc = 0;
            percent_counter++;
            printf("Percent formatted: %d\n", percent_counter);
        } else perc++;
        empty_block->linker.previous = i - 1;
        empty_block->linker.next = i + 1;
        write_struct(get_page_offset(i), (byte*) empty_block, sizeof(block_header));
    }
    empty_block->linker.previous = header->empty.next - 1;
    empty_block->linker.next = -1;
    write_struct(get_page_offset(header->empty.next), (byte*) empty_block, sizeof(block_header));
    free(empty_block);
    write_struct(fs_header_offset, (byte*) header, sizeof(system_header));

    int root_block_num = occupy_block();
    read_struct(fs_header_offset, (byte*) header, sizeof(system_header));
    header->root_page = root_block_num;
    write_struct(fs_header_offset, (byte*) header, sizeof(system_header));
    int current_offset = get_page_offset(header->root_page);

    block_header* root_block = malloc(sizeof(block_header));
    root_block->is_occupied = true;
    root_block->linker.previous = root_block->linker.next = -1;
    root_block->head = header->root_page;
    write_struct(current_offset, (byte*) root_block, sizeof(block_header));
    current_offset += sizeof(block_header);

    file_header* root_header = malloc(sizeof(file_header));
    root_header->size = 0;
    root_header->property = WRITABLE;
    root_header->is_directory = true;
    root_header->parent_page = -1;
    copy_part("root", root_header->file_name, max_path_len - 1);
    write_struct(current_offset, (byte*) root_header, sizeof(file_header));

    free(root_block);
    free(root_header);
    free(header);
    return 0;
}



void set_file_header(data_iterator* iterator, byte* container, int length) {
    if (iterator->current == get_file_block_head_number(iterator->current)) for (int i = 0; i < length; ++i) iterator->page[i] = container[i];
    write_struct(get_page_content_offset(get_file_block_head_number(iterator->current)), container, length);
}



data_iterator* create_data() {
    int new_block = occupy_block();
    block_header *block = malloc(sizeof(block_header));
    read_struct(get_page_offset(new_block), (byte *) block, sizeof(block_header));
    block->is_occupied = true;
    block->head = new_block;
    block->linker.previous = block->linker.next = -1;
    write_struct(get_page_offset(new_block), (byte *) block, sizeof(block_header));
    return load_data(new_block);
}

void destroy_data(data_iterator* iterator) {
    block_header *block = malloc(sizeof(block_header));
    read_struct(get_page_offset(get_file_block_head_number(iterator->current)), (byte *) block, sizeof(block_header));
    int next_page = block->linker.next;
    free_block(get_file_block_head_number(iterator->current));
    while (next_page != -1) {
        read_struct(get_page_offset(next_page), (byte *) block, sizeof(block_header));
        int current_page = next_page;
        next_page = block->linker.next;
        free_block(current_page);
    }
    free(block);
    dismiss_data(iterator);
}



void add_page(data_iterator* iterator) {
    block_header *block = malloc(sizeof(block_header));
    read_struct(get_page_offset(iterator->current), (byte*) block, sizeof(block_header));
    int current_page = iterator->current, next_page = block->linker.next, main_page = block->head;
    while (next_page != -1) {
        read_struct(get_page_offset(next_page), (byte *) block, sizeof(block_header));
        current_page = next_page;
        next_page = block->linker.next;
    }

    int new_block = occupy_block();
    read_struct(get_page_offset(current_page), (byte*) block, sizeof(block_header));
    block->linker.next = new_block;
    write_struct(get_page_offset(current_page), (byte*) block, sizeof(block_header));

    read_struct(get_page_offset(new_block), (byte*) block, sizeof(block_header));
    block->is_occupied = true;
    block->head = main_page;
    block->linker.previous = current_page;
    block->linker.next = -1;
    write_struct(get_page_offset(new_block), (byte*) block, sizeof(block_header));

    free(block);
}

void delete_page(data_iterator* iterator) {
    block_header *block = malloc(sizeof(block_header));
    read_struct(get_page_offset(iterator->current), (byte *) block, sizeof(block_header));
    int next_page = block->linker.next;
    while (next_page != -1) {
        read_struct(get_page_offset(next_page), (byte *) block, sizeof(block_header));
        next_page = block->linker.next;
    }

    int previous_page = block->linker.previous;
    read_struct(get_page_offset(previous_page), (byte *) block, sizeof(block_header));
    free_block(block->linker.next);
    block->linker.next = -1;
    write_struct(get_page_offset(previous_page), (byte *) block, sizeof(block_header));

    free(block);
}

void truncate(data_iterator* iterator, int bytes) {
    int free_pages = bytes / page_content_size;
    for (int i = 0; i < free_pages; ++i) delete_page(iterator);
}



void set_next_bytes(data_iterator* iterator, const byte* container, int length) {
    for (int i = 0; i < length; ++i) {
        while (data_step(iterator, true)) add_page(iterator);
        iterator->page[iterator->offset] = container[i];
        iterator->offset++;
    }
    write_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
}

void set_previous_bytes(data_iterator* iterator, const byte* container, int length) {
    for (int i = length; i >= 0; --i) {
        while (data_step(iterator, true)) add_page(iterator);
        iterator->page[iterator->offset] = container[i];
        iterator->offset--;
    }
    write_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
}





file* create_file(string name, int parent) {
    file* f = malloc(sizeof(file));
    f->iterator = create_data();
    f->absolute_offset = 0;
    f->main_page = get_file_block_head_number(f->iterator->current);

    f->header = malloc(sizeof(file_header));
    f->header->size = 0;
    f->header->property = DESTROYABLE;
    f->header->is_directory = false;
    f->header->parent_page = parent;
    copy_part(name, f->header->file_name, max_path_len - 1);
    set_file_header(f->iterator, (byte*) f->header, sizeof(file_header));
    set_offset(f->iterator, sizeof(file_header));

    return f;
}

void delete_file(file* f) {
    destroy_data(f->iterator);
    free(f);
}

void flush_file(file* f) {
    set_file_header(f->iterator, (byte*) f->header, sizeof(file_header));
    reset_and_reload(f->iterator);
    set_offset(f->iterator, sizeof(file_header));
    f->absolute_offset = 0;
}



void append_file(file* f, byte* data, int size) {
    if (f->header->property >= WRITABLE) {
        set_offset(f->iterator, sizeof(file_header) + f->header->size);
        set_next_bytes(f->iterator, data, size);
        f->header->size += size;
        set_file_header(f->iterator, (byte*) f->header, sizeof(file_header));
        f->absolute_offset = f->header->size;
    }
}

void truncate_file(file* f, int bytes) {
    if (f->header->property >= WRITABLE) {
        f->header->size -= bytes;
        set_file_header(f->iterator, (byte *) f->header, sizeof(file_header));
        if (f->absolute_offset > f->header->size) {
            f->absolute_offset = f->header->size;
            set_offset(f->iterator, sizeof(file_header) + f->absolute_offset);
        }
        truncate(f->iterator, bytes);
    }
}

void write_file(file* f, byte* bytes, int size) {
    if (f->header->property >= WRITABLE) {
        set_offset(f->iterator, sizeof(file_header));
        truncate(f->iterator, f->header->size - size);
        f->header->size = size;
        set_file_header(f->iterator, (byte *) f->header, sizeof(file_header));
        set_next_bytes(f->iterator, bytes, size);
        f->absolute_offset = size;
    }
}



void write_bytes(file* f, byte* bytes, int size, int offset) {
    if (f->absolute_offset + size > f->header->size) {
        f->header->size = f->absolute_offset + size;
        set_file_header(f->iterator, (byte *) f->header, sizeof(file_header));
    }
    add_offset(f->iterator, offset);
    set_next_bytes(f->iterator, bytes, size);
    f->absolute_offset += offset + size;
}





void add_file_to_dir(file* f, file* dir) {
    directory_entry* entry = malloc(sizeof(directory_entry));
    entry->file_offset = f->main_page;
    append_file(dir, (byte*) entry, sizeof(directory_entry));
    free(entry);
}

void delete_file_from_dir(file* f, file* dir) {
    if (f->header->is_directory) {
        file* current_file = null;
        directory_entry* entry = malloc(sizeof(directory_entry));
        int files_num = f->header->size / sizeof(directory_entry);

        for (int i = 0; i < files_num; ++i) {
            read_bytes(f, (byte*) entry, sizeof(directory_entry), 0);
            current_file = open_file(entry->file_offset);
            delete_file_from_dir(current_file, f);
        }
        free(entry);
        free(current_file);
    }

    directory_entry* entry = malloc(sizeof(directory_entry));
    int files_num = dir->header->size / sizeof(directory_entry);

    seek_to(dir, 0);
    boolean current_file_reached = false;
    int size = sizeof(directory_entry);
    for (int i = 0; i < files_num; ++i) {
        read_bytes(dir, (byte*) entry, size, 0);
        if (!current_file_reached && (entry->file_offset == f->main_page)) {
            current_file_reached = true;
            continue;
        }
        write_bytes(dir, (byte*) entry, size, -size);
    }
    free(entry);

    truncate_file(dir, size);
    delete_file(f);
}



file* recur_file_w(mod_string path, file* parent, action act) {
    int delimiter_pos = first_pos(path, delimiter);

    if (delimiter_pos == -1) {
        file* actual = find_file_in_dir(path, parent);
        if (actual == null) {
            if (act == CREATE) {
                actual = create_file(path, parent->main_page);
                add_file_to_dir(actual, parent);
            }
        } else if (act == DELETE) {
            delete_file_from_dir(actual, parent);
            return null;
        }
        return actual;

    } else {
        mod_string current_dir_path = malloc(delimiter_pos + 1);
        substring_end(path, current_dir_path, delimiter_pos);

        file* current_dir = find_file_in_dir(current_dir_path, parent);
        if (current_dir == null) {
            if (act == CREATE) {
                current_dir = create_file(current_dir_path, parent->main_page);
                current_dir->header->is_directory = true;
                flush_file(current_dir);
                add_file_to_dir(current_dir, parent);
            } else {
                free(current_dir_path);
                free(current_dir);
                return null;
            }
        }

        move_string_by(path, delimiter_pos + 1);
        file* result = recur_file_w(path, current_dir, act);
        free(current_dir_path);
        free(current_dir);

        return result;
    }
}

file* create_file_global(string path) {
    mod_string copy_path = malloc(len(path) + 1);
    copy_mod(path, copy_path);
    file* result = null;

    if (len(path) == 0) {
        result = open_file(get_root_dir_number());
    } else if (path[0] == delimiter) {
        file* root = open_file(get_root_dir_number());
        move_string_by(copy_path, 1);
        result = recur_file_w(copy_path, root, CREATE);
        close_file(root);
    }

    free(copy_path);
    return result;
}

void delete_file_global(string path) {
    mod_string copy_path = malloc(len(path) + 1);
    copy_mod(path, copy_path);

    if (path[0] == delimiter) {
        file* root = open_file(get_root_dir_number());
        move_string_by(copy_path, 1);
        recur_file_w(copy_path, root, DELETE);
        close_file(root);
    }

    free(copy_path);
}
