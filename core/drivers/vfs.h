#ifndef MUSHOS_VFS_H
#define MUSHOS_VFS_H

#include "../../lib/base/generic.h"


#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

typedef struct fs_node {
   char name[128];    // The filename.
   dword mask;        // The permissions mask.
   dword uid;         // The owning user.
   dword gid;         // The owning group.
   dword flags;       // Includes the node type. See #defines above.
   dword inode;       // This is device-specific - provides a way for a filesystem to identify files.
   dword length;      // Size of the file, in bytes.
   dword impl;        // An implementation-defined number.
   fs_node *ptr; // Used by mountpoints and symlinks.
} fs_node_t;

typedef struct {  // One of these is returned by the readdir call, according to POSIX.
  char name[128]; // Filename.
  dword ino;     // Inode number. Required by POSIX.
} dirent;

dword read_fs(fs_node* node, dword offset, dword size, byte* buffer);
dword write_fs(fs_node* node, dword offset, dword size, byte* buffer);
void open_fs(fs_node* node, byte read, byte write);
void close_fs(fs_node* node);
dirent* readdir_fs(fs_node* node, dword index);
fs_node* finddir_fs(fs_node* node, char *name);

#endif //MUSHOS_VFS_H
