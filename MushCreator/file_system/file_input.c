#include "file_input.h"
#include "../adapter/driver_proxy.h"
#include <stdlib.h>
#include <stdio.h>


int get_page_offset(int page_num) {
    return fs_header_offset + sizeof(system_header) + (page_num * page_size);
}

int get_page_content_offset(int page_num) {
    return fs_header_offset + sizeof(system_header) + (page_num * page_size) + sizeof(block_header);
}

boolean check_drive() {
    return magic == read_int(fs_header_offset);
}



int get_next_block_number(int current) {
    block_header* header = malloc(sizeof(block_header));
    read_struct(get_page_offset(current), (byte*) header, sizeof(block_header));
    int next = header->linker.next;
    free(header);
    return next;
}

int get_previous_block_number(int current) {
    block_header* header = malloc(sizeof(block_header));
    read_struct(get_page_offset(current), (byte*) header, sizeof(block_header));
    int next = header->linker.previous;
    free(header);
    return next;
}

int get_file_block_head_number(int file_page) {
    block_header* current = malloc(sizeof(block_header));
    read_struct(get_page_offset(file_page), (byte*) current, sizeof(block_header));
    int main = current->head;
    free(current);
    return main;
}



boolean data_step(data_iterator* iterator, boolean flush_page) {
    while (iterator->offset >= page_content_size) {
        int next = get_next_block_number(iterator->current);
        if (flush_page) write_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
        if (next == -1) return true;
        iterator->offset = 0;
        iterator->current = next;
        read_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
    }
    while (iterator->offset < 0) {
        int previous = get_previous_block_number(iterator->current);
        if (flush_page) write_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
        if (previous == -1) return true;
        iterator->offset = page_content_size - 1;
        iterator->current = previous;
        read_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
    }
    return false;
}



int get_root_dir_number() {
    system_header* header = malloc(sizeof(system_header));
    read_struct(fs_header_offset, (byte*) header, sizeof(system_header));
    int root = header->root_page;
    free(header);
    return root;
}

boolean is_eof(data_iterator* iterator) {
    int next = 0;
    if (iterator->offset >= page_content_size) next = get_next_block_number(iterator->current);
    else if (iterator->offset < 0) next = get_previous_block_number(iterator->current);
    return next == -1;
}

boolean set_offset(data_iterator* iterator, int skip) {
    iterator->current = get_file_block_head_number(iterator->current);
    read_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
    iterator->offset = skip;
    return data_step(iterator, false);
}

boolean add_offset(data_iterator* iterator, int skip) {
    iterator->offset += skip;
    return data_step(iterator, false);
}

void reset_and_reload(data_iterator* iterator) {
    *iterator = *load_data(get_file_block_head_number(iterator->current));
}



void get_file_header(data_iterator* iterator, byte* container, int length) {
    read_struct(get_page_content_offset(get_file_block_head_number(iterator->current)), container, length);
}

void get_file_page_header(int main_page, byte* container, int length) {
    read_struct(get_page_content_offset(main_page), container, length);
}



data_iterator* load_data(int main_page) {
    data_iterator* iterator = malloc(sizeof(data_iterator));
    iterator->offset = 0;
    iterator->current = main_page;
    iterator->page = malloc(page_content_size);
    read_struct(get_page_content_offset(iterator->current), iterator->page, page_content_size);
    return iterator;
}

void dismiss_data(data_iterator* container) {
    free(container->page);
    free(container);
}



boolean get_next_bytes(data_iterator* iterator, byte* container, int length) {
    for (int i = 0; i < length; ++i) {
        if (data_step(iterator, false)) return true;
        container[i] = iterator->page[iterator->offset];
        iterator->offset++;
    }
    return false;
}

boolean get_previous_bytes(data_iterator* iterator, byte* container, int length) {
    for (int i = length; i >= 0; --i) {
        if (data_step(iterator, false)) return true;
        container[i] = iterator->page[iterator->offset];
        iterator->offset--;
    }
    return false;
}



void check_root() {
    boolean drive_valid = check_drive();
    printf("Drive valid: %d\n", drive_valid);
    if (!drive_valid) return;

    system_header* header = malloc(sizeof(system_header));
    read_struct(fs_header_offset, (byte*) header, sizeof(system_header));
    printf("\nSystem header info:\n");
    printf("\tEmpty pages: %d\n", header->empty_pages);
    printf("\tRoot offset: %d\n", header->root_page);
    printf("\tFirst empty: %d\n", header->empty.previous);
    printf("\tLast empty: %d\n", header->empty.next);

    block_header* root_dir_block = malloc(sizeof(block_header));
    read_struct(get_page_offset(header->root_page), (byte*) root_dir_block, sizeof(block_header));
    printf("\nRoot dir block info:\n");
    printf("\tIs empty: %d\n", root_dir_block->is_occupied);
    printf("\tNext block: %d; previous block: %d\n", root_dir_block->linker.next, root_dir_block->linker.previous);
    printf("\tHead block: %d\n", root_dir_block->head);

    file_header* root_dir_file = malloc(sizeof(file_header));
    read_struct(get_page_content_offset(header->root_page), (byte*) root_dir_file, sizeof(file_header));
    printf("\nRoot dir file_header info:\n");
    printf("\tSize: %d\n", root_dir_file->size);
    printf("\tProperty: %d\n", root_dir_file->property);
    printf("\tParent page: %d\n", root_dir_file->parent_page);
    printf("\tPath: %s (", root_dir_file->file_name);
    for (int i = 0; i < 16; ++i) printf("%d ", root_dir_file->file_name[i]);
    printf(")\n");

    file_header* root_dir = malloc(sizeof(file_header));
    read_struct(get_page_content_offset(header->root_page) + sizeof(file_header), (byte*) root_dir, sizeof(file_header));
    printf("\nRoot dir info:\n");

    free(header);
    free(root_dir_block);
    free(root_dir_file);
    free(root_dir);
}





file* open_file(int page) {
    file* f = malloc(sizeof(file));
    f->iterator = load_data(page);
    f->absolute_offset = 0;
    f->main_page = get_file_block_head_number(f->iterator->current);
    f->header = malloc(sizeof(file_header));
    get_file_header(f->iterator, (byte*) f->header, sizeof(file_header));
    set_offset(f->iterator, sizeof(file_header));
    return f;
}

void close_file(file* f) {
    dismiss_data(f->iterator);
    free(f);
}



void seek_to(file* f, int absolute_position) {
    set_offset(f->iterator, sizeof(file_header) + absolute_position);
    f->absolute_offset = absolute_position;
}

void seek_by(file* f, int absolute_position) {
    add_offset(f->iterator, sizeof(file_header) + absolute_position);
    f->absolute_offset += absolute_position;
}

void read_bytes(file* f, byte* bytes, int size, int offset) {
    add_offset(f->iterator, offset);
    get_next_bytes(f->iterator, bytes, size);
    f->absolute_offset += offset + size;
}





file* find_file_in_dir(string file_name, file* dir) {
    int current_file_page = 0;
    file* current_file = nullptr;
    boolean exists = false;
    directory_entry* entry = malloc(sizeof(directory_entry));
    file_header* header = malloc(sizeof(file_header));
    int files_num = dir->header->size / sizeof(directory_entry);

    for (int i = 0; i < files_num; ++i) {
        read_bytes(dir, (byte*) entry, sizeof(directory_entry), 0);
        get_file_page_header(entry->file_offset, (byte*) header, sizeof(file_header));
        if (equals(header->file_name, file_name)) {
            exists = true;
            current_file_page = entry->file_offset;
            break;
        }
    }
    if (exists) current_file = open_file(current_file_page);

    free(header);
    free(entry);
    return current_file;
}

file* open_check_link(file* f) {
    return f;
}



file* recur_file_r(mod_string path, file* parent) {
    int delimiter_pos = first_pos(path, delimiter);

    if (delimiter_pos == -1)
        return open_check_link(find_file_in_dir(path, parent));
    else {
        mod_string current_dir_path = malloc(delimiter_pos + 1);
        substring_end(path, current_dir_path, delimiter_pos);

        file* current_dir = find_file_in_dir(current_dir_path, parent);
        if (current_dir == nullptr) {
            free(current_dir_path);
            free(current_dir);
            return nullptr;
        }

        move_string_by(path, delimiter_pos + 1);
        file* result = recur_file_r(path, current_dir);
        free(current_dir_path);
        free(current_dir);

        return result;
    }
}

file* open_file_global(string path) {
    mod_string copy_path = malloc(len(path) + 1);
    copy_mod(path, copy_path);
    file* result = nullptr;

    if (len(path) == 0) {
        result = open_file(get_root_dir_number());
    } else if (path[0] == delimiter) {
        file* root = open_file(get_root_dir_number());
        move_string_by(copy_path, 1);
        result = recur_file_r(copy_path, root);
        close_file(root);
    }

    free(copy_path);
    return result;
}

file* get_root_dir() {
    return open_file(get_root_dir_number());
}



boolean file_exists(string path) {
    file* f = open_file_global(path);
    boolean answer = f != nullptr;
    close_file(f);
    return answer;
}



int num_files(string path) {
    file* dir = open_file_global(path);
    int files_num = -1;
    if (dir->header->is_directory) files_num = dir->header->size / sizeof(directory_entry);
    close_file(dir);
    return files_num;
}

mod_string* list_files(string dir_path) {
    file* dir = open_file_global(dir_path);
    int files_num = dir->header->size / sizeof(directory_entry);

    mod_string* list = malloc(files_num * sizeof(mod_string));
    directory_entry* entry = malloc(sizeof(directory_entry));
    file_header* header = malloc(sizeof(file_header));

    for (int i = 0; i < files_num; ++i) {
        read_bytes(dir, (byte*) entry, sizeof(directory_entry), 0);
        get_file_page_header(entry->file_offset, (byte*) header, sizeof(file_header));

        int name_length = len(header->file_name);
        list[i] = malloc(name_length + 1);
        substring_end(header->file_name, list[i], name_length);
    }

    free(entry);
    free(header);
    close_file(dir);
    return list;
}

mod_string get_name(file* f) {
    int name_length = len(f->header->file_name);
    mod_string name = malloc(name_length);

    for (int i = 0; i < name_length; ++i) name[i] = f->header->file_name[i];
    name[name_length] = 0;

    return name;
}

mod_string get_path(file* f) {
    if (f->header->parent_page != -1) {
        file* parent = open_file(f->header->parent_page);
        mod_string parent_name = get_path(parent);
        close_file(parent);

        int concat_name_length = len(f->header->file_name) + 1;
        mod_string concat_name = malloc(concat_name_length);
        concatenate("/", f->header->file_name, concat_name);

        int result_name_length = len(parent_name) + concat_name_length;
        mod_string result_name = malloc(result_name_length);
        concatenate(parent_name, concat_name, result_name);
        free(concat_name);
        free(parent_name);

        return result_name;

    } else {
        int name_length = len(f->header->file_name);
        mod_string name = malloc(name_length + 1);
        substring_end(f->header->file_name, name, name_length);
        return name;
    }
}

mod_string get_extension(file* f) {
    mod_string name = get_name(f);
    int extension_length = len(name) - last_pos(name, '.');
    mod_string extension = malloc(extension_length);
    substring_beg(name, extension, len(name) - extension_length + 1);
    return extension;
}
