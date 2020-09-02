#ifndef MUSHFS_STRING_UTILS_H
#define MUSHFS_STRING_UTILS_H

#include "generic.h"

int len(string str);
void copy_part(string str, mod_string storage, int size);
int first_pos(string str, char ch);
int last_pos(string str, char ch);

boolean equals(string str1, string str2);

void substring_beg(string str, mod_string substr, int begin);
void substring_mid(string str, mod_string substr, int begin, int end);
void substring_end(string str, mod_string substr, int end);

void copy_mod(string str, mod_string m_str);
void move_string_by(mod_string str, int step);

void concatenate(string str1, string str2, mod_string result);

#endif //MUSHFS_STRING_UTILS_H
