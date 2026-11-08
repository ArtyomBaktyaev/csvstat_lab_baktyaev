/* Заголовочный файл модуля статистики.
   Функции добавления чисел в статистику столбца и вывода результата. */

#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include "types.h"

void add_int_val(Column *c, long long v);
void add_double_val(Column *c, double d);
void print_col_stats(size_t index, const Column *c);

#endif