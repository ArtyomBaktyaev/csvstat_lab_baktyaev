/* Заголовочный файл модуля CSV.
   Объявляет функции запуска режимов schema и stats. */

#ifndef CSV_H
#define CSV_H

int handle_schema(const char *filename);
int handle_stats(const char *filename);

#endif