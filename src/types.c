/* Функции распознавания типа ячейки (INT или DOUBLE)
   и перевода строки в целое число long long. */

#include <string.h>
#include "types.h"

enum ColType detect_value_type(const char *str) {
    if (strchr(str, '.') != NULL) {
        return TYPE_DOUBLE;
    }
    return TYPE_INT;
}

const char *type_to_str(enum ColType type) {
    if (type == TYPE_DOUBLE) {
        return "DOUBLE";
    }
    return "INT";
}

long long to_int(const char *s) {
    int is_neg = 0;
    if (*s == '-') {
        is_neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    unsigned long long res = 0;
    while (*s >= '0' && *s <= '9') {
        res = res * 10 + (unsigned long long)(*s - '0');
        s++;
    }

    if (is_neg && res > 0) {
        return -(long long)(res - 1) - 1;
    }
    return (long long)res;
}