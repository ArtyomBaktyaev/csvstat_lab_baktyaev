/* Описание типов столбцов (INT и DOUBLE) и структуры Column.
   Объявляет функции для проверки типа и перевода строки в число. */

#ifndef TYPES_H
#define TYPES_H

enum ColType {
    TYPE_INT,
    TYPE_DOUBLE
};

typedef struct {
    char *name;
    enum ColType type;
    long long count;
    long long min;
    long long max;
    long long positive;
    long long negative;
    long long even;
    long long sum_billions;
    long long sum_rem;
    double dmin;
    double dmax;
    double dsum;
    long long integer;
} Column;

enum ColType detect_value_type(const char *str);
const char *type_to_str(enum ColType type);
long long to_int(const char *s);

#endif