#include "string.h"

#include "heap.h"


int len(string str) {
    int string_iterator = 0;
    while (str[string_iterator] != 0) string_iterator++;
    return string_iterator;
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



mod_string substring_beg(string str, int begin) {
    return substring_mid(str, begin, len(str));
}

mod_string substring_mid(string str, int begin, int end) {
    mod_string result = ralloc(end - begin + 1);
    for (int i = begin; i < end; ++i) result[i - begin] = str[i];
    result[end - begin] = 0;
    return result;
}

mod_string substring_end(string str, int end) {
    return substring_mid(str, 0, end);
}



void move_string_by(mod_string str, int step) {
    int i = step - 1;
    do {
        i++;
        str[i - step] = str[i];
    } while (str[i] != 0);
    str[i - step + 1] = 0;
}

mod_string concatenate(string str1, string str2) {
    int str1_length = len(str1), str2_length = len(str2);
    mod_string result = ralloc(str1_length + str2_length + 1);
    for (int i = 0; i < str1_length; ++i) result[i] = str1[i];
    for (int i = 0; i < str2_length; ++i) result[i + str1_length] = str2[i];
    result[str1_length + str2_length] = 0;
    return result;
}
