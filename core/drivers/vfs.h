#ifndef MUSHOS_VFS_H
#define MUSHOS_VFS_H

#include "../../lib/base/generic.h"


#define FILE 0x01
#define DIRECTORY 0x02
#define MOUNTPOINT 0x03

#define READABLE 0x01
#define WRITABLE 0x02
#define EXECUTABLE 0x03

typedef struct {
   char name[128];
   byte type;
   byte flag;
   dword length;
} fs_node;

void* read(fs_node* node, dword offset, dword size, byte* buffer);
dword write(fs_node* node, dword offset, dword size, byte* buffer);
void open(fs_node* node, byte read, byte write);
void close(fs_node* node);
fs_node* readdir(fs_node* node, dword index);
fs_node* finddir(fs_node* node, char *name);

void initialize_vfs(byte* location);

#endif //MUSHOS_VFS_H
