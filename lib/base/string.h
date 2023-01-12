#ifndef MUSHLIB_STRING_UTILS_H
#define MUSHLIB_STRING_UTILS_H

#include "generic.h"


int len(string str);
int first_pos(string str, char ch);
int last_pos(string str, char ch);

boolean equals(string str1, string str2);

mod_string substring_beg(string str, int begin);
mod_string substring_mid(string str, int begin, int end);
mod_string substring_end(string str, int end);

void move_string_by(mod_string str, int step);
mod_string concatenate(string str1, string str2);

mod_string format(string template, ...);

#endif // MUSHLIB_STRING_UTILS_H
