#include "string.h"


int len(string str) {
    int string_iterator = 0;
    while (str[string_iterator] != 0) string_iterator++;
    return string_iterator;
}

void copy_part(string str, mod_string storage, int size) {
    boolean source_end_reached = false;
    for (int i = 0; i < size; ++i) {
        source_end_reached |= str[i] == 0;
        if (source_end_reached) storage[i] = 0;
        else storage[i] = str[i];
    }
    storage[size] = 0;
}



int first_pos(string str, char ch) {
    int string_iterator = 0;
    while (str[string_iterator] != 0) {
        if (str[string_iterator] == ch) return string_iterator;
        string_iterator++;
    }
    return -1;
}

int last_pos(string str, char ch) {
    int string_iterator = len(str);
    while (string_iterator != 0) {
        if (str[string_iterator] == ch) return string_iterator;
        string_iterator--;
    }
    return -1;
}



boolean equals(string str1, string str2) {
    int string_iterator = 0;
    while ((str1[string_iterator] != 0) && (str2[string_iterator] != 0)) {
        if (str1[string_iterator] == str2[string_iterator]) string_iterator++;
        else return false;
    }
    if ((str1[string_iterator] == 0) && (str2[string_iterator] == 0)) return true;
    else return false;
}



void substring_beg(string str, mod_string substr, int begin) {
    substring_mid(str, substr, begin, len(str));
}

void substring_mid(string str, mod_string substr, int begin, int end) {
    for (int i = begin; i < end; ++i) substr[i - begin] = str[i];
    substr[end - begin] = 0;
}

void substring_end(string str, mod_string substr, int end) {
    substring_mid(str, substr, 0, end);
}



void copy_mod(string str, mod_string m_str) {
    int i = -1;
    do {
        i++;
        m_str[i] = str[i];
    } while (str[i] != 0);
}

void move_string_by(mod_string str, int step) {
    int i = step - 1;
    do {
        i++;
        str[i - step] = str[i];
    } while (str[i] != 0);
}



void concatenate(string str1, string str2, mod_string result) {
    int str1_length = len(str1), str2_length = len(str2);
    for (int i = 0; i < str1_length; ++i) result[i] = str1[i];
    for (int i = 0; i <= str2_length; ++i) result[i + str1_length] = str2[i];
}
