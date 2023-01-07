#include "vfs.h"


fs_node* root;

void initialize_vfs(byte* location) {
    root = (fs_node*) location;
}


void* read(fs_node* node, dword offset, dword size, byte* buffer) {

}

dword write(fs_node* node, dword offset, dword size, byte* buffer) {

}

void open(fs_node* node, byte read, byte write) {

}

void close(fs_node* node) {

}

fs_node* readdir(fs_node* node, dword index) {

}

fs_node* finddir(fs_node* node, char *name) {

}
