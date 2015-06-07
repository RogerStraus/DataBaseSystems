#include "read.h"
#include <cstdio>
#include <iostream>

using namespace std;

const char delimeter = '-';

my_string read_text(FILE* fp) {
    int count = 0;
    char ch;
    while ((ch = fgetc(fp)) != '"') {
        ++count;
    }

    char str[count + 1];
    fseek(fp, -(count + 1), SEEK_CUR);
    fread(str, count, 1, fp);
    str[count] = '\0';
    my_string s;
    s = str;
    ch = fgetc(fp); // "
    return s;
}

my_string read_int(FILE* fp) {
    int count = 0;
    char ch;
    fseek(fp, -1, SEEK_CUR);
    while ((ch = fgetc(fp)) >= '0' && (ch <= '9')) {
        ++count;
    }
    char str[count + 1];
    fseek(fp, -(count + 1), SEEK_CUR);
    fread(str, count, 1, fp);
    str[count] = '\0';
    my_string s;
    s = str;
    return s;
}

bool read_bool(FILE* fp) {
    bool k;
    char ch;
    int skip;
    fseek(fp, -1, SEEK_CUR);
    ch = fgetc(fp);
    assert('f' == ch || 't' == ch);
    if ('f' == ch) {
        k = false;
        skip = 4;
    } else if ('t' == ch) {
        k = true;
        skip = 3;
    }
    for (int i = 0; i < skip; i++) {
        ch = fgetc(fp);
    }
    return k;
}


my_string read_nested_arr(FILE* fp) {
    int count = 0;
    char ch;
    my_string s, s_next;

    while ((ch = fgetc(fp)) != ']') {
        if ('"' == ch) {
            s_next = read_text(fp);
            if (0 == s.size()) {
                s = s_next;
            } else {
                char divide[0];
                divide[0] = delimeter;
                s = s + divide;
                s = s + s_next;
            }
        }
    }
    return s;
}

tuple* read_tuple(FILE* fp, catalog* CATALOG) {
    char ch;
    tuple *t = new tuple;
    initial(t);
    while (1) {
        // read a key name
        ch = fgetc(fp); // "
        my_string key_name;
        key_name = read_text(fp);
        ch = fgetc(fp);  // :
        ch = fgetc(fp);  // space
        my_string key_type;
        int id = 1;
        ch = fgetc(fp);
        if ('"' == ch) {
            my_string key_value;
            key_value = read_text(fp);
            key_type = "text";
            id = CATALOG->index(key_type.content(), key_name.content());
            add_text(t, id, key_value);
        } else if (ch >= '0' && ch <= '9') {
            my_string key_value;
            key_value = read_int(fp);
            key_type = "int";
            id = CATALOG->index(key_type.content(), key_name.content());
            add_int(t, id, key_value);
        } else if ('t' == ch || 'f' == ch) {
            bool key_value;
            key_value = read_bool(fp);
            key_type = "bool";
            id = CATALOG->index(key_type.content(), key_name.content());
            add_bool(t, id, key_value);
        } else if ('[' == ch) {
            my_string key_value;
            key_value = read_nested_arr(fp);
            key_type = "nested_arr";
            id = CATALOG->index(key_type.content(), key_name.content());
            add_nested_arr(t, id, key_value);
        } else if ('{' == ch) {
            tuple* key_value;
            key_value = read_tuple(fp, CATALOG);
            key_type = "nested_obj";
            id = CATALOG->index(key_type.content(), key_name.content());
            add_nested_obj(t, id, key_value);
            delete key_value;
        }
        ch = fgetc(fp);  // colon
        if ('}' == ch) {
            break;
        }
        ch = fgetc(fp);  // space
    }
    return t;
}