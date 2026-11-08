/* Чтение CSV-файла построчно, разбиение строк по запятым
   и выполнение команд schema и stats. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv.h"
#include "types.h"
#include "stats.h"

/* Копирование строки в новую динамическую память */
static char *copy_str(const char *src) {
    size_t len = strlen(src);
    char *dest = malloc(len + 1);
    if (dest == NULL) {
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

/* Чтение одной строки произвольной длины до \n или EOF */
static char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (buf == NULL) {
        return NULL;
    }

    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            break;
        }

        /* Если места в буфере не хватает, увеличиваем в 2 раза */
        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (tmp == NULL) {
                free(buf);
                return NULL;
            }
            buf = tmp;
        }
        buf[len] = (char)c;
        len++;
    }

    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }

    buf[len] = '\0';
    return buf;
}

/* Разбиваем строку по запятым, заменяя ',' на '\0' */
static char **split_csv_line(char *line, size_t *cols_count) {
    size_t count = 1;
    for (char *p = line; *p != '\0'; p++) {
        if (*p == ',') {
            count++;
        }
    }

    char **words = malloc(count * sizeof(char *));
    if (words == NULL) {
        return NULL;
    }

    size_t idx = 0;
    words[idx] = line;
    idx++;

    for (char *p = line; *p != '\0'; p++) {
        if (*p == ',') {
            *p = '\0';
            words[idx] = p + 1;
            idx++;
        }
    }

    *cols_count = count;
    return words;
}

static void free_columns(Column *cols, size_t n) {
    for (size_t i = 0; i < n; i++) {
        free(cols[i].name);
    }
    free(cols);
}

/* Читаем первую строку файла и создаем массив столбцов */
static Column *read_header(FILE *file, size_t *cols_count) {
    char *header = read_line(file);
    if (header == NULL) {
        fprintf(stderr, "Error: File is empty.\n");
        return NULL;
    }

    size_t n = 0;
    char **words = split_csv_line(header, &n);
    if (words == NULL) {
        free(header);
        return NULL;
    }

    Column *cols = calloc(n, sizeof(Column));
    if (cols == NULL) {
        free(words);
        free(header);
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        cols[i].name = copy_str(words[i]);
        cols[i].type = TYPE_INT; /* по умолчанию считаем столбец целым */
        if (cols[i].name == NULL) {
            free_columns(cols, n);
            free(words);
            free(header);
            return NULL;
        }
    }

    free(words);
    free(header);
    *cols_count = n;
    return cols;
}

/* Первый проход по данным: считаем строки и определяем типы столбцов */
static int check_types(FILE *file, Column *cols, size_t n, long long *rows) {
    char *line = NULL;
    *rows = 0;

    while ((line = read_line(file)) != NULL) {
        size_t cnt = 0;
        char **words = split_csv_line(line, &cnt);
        if (words == NULL) {
            free(line);
            return 1;
        }
        if (cnt != n) {
            fprintf(stderr, "Error: Wrong number of fields in row %lld.\n", *rows + 1);
            free(words);
            free(line);
            return 1;
        }

        /* Если хотя бы одно значение с точкой, расширяем тип до DOUBLE */
        for (size_t i = 0; i < n; i++) {
            if (detect_value_type(words[i]) == TYPE_DOUBLE) {
                cols[i].type = TYPE_DOUBLE;
            }
        }

        (*rows)++;
        free(words);
        free(line);
    }
    return 0;
}

/* Второй проход по данным: собираем статистики по каждому столбцу */
static int calc_stats(FILE *file, Column *cols, size_t n) {
    char *line = NULL;
    while ((line = read_line(file)) != NULL) {
        size_t cnt = 0;
        char **words = split_csv_line(line, &cnt);
        if (words == NULL) {
            free(line);
            return 1;
        }

        for (size_t i = 0; i < n; i++) {
            if (cols[i].type == TYPE_DOUBLE) {
                add_double_val(&cols[i], strtod(words[i], NULL));
            } else {
                add_int_val(&cols[i], to_int(words[i]));
            }
        }

        free(words);
        free(line);
    }
    return 0;
}

int handle_schema(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'.\n", filename);
        return 1;
    }

    size_t n = 0;
    Column *cols = read_header(file, &n);
    if (cols == NULL) {
        fclose(file);
        return 1;
    }

    long long rows = 0;
    if (check_types(file, cols, n, &rows) != 0) {
        free_columns(cols, n);
        fclose(file);
        return 1;
    }
    fclose(file);

    printf("rows %lld\n", rows);
    printf("columns %zu\n", n);
    for (size_t i = 0; i < n; i++) {
        printf("%zu \"%s\" %s\n", i, cols[i].name, type_to_str(cols[i].type));
    }

    free_columns(cols, n);
    return 0;
}

int handle_stats(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'.\n", filename);
        return 1;
    }

    size_t n = 0;
    Column *cols = read_header(file, &n);
    if (cols == NULL) {
        fclose(file);
        return 1;
    }

    long long rows = 0;
    if (check_types(file, cols, n, &rows) != 0) {
        free_columns(cols, n);
        fclose(file);
        return 1;
    }

    /* Возвращаемся в начало файла и пропускаем строку заголовка */
    rewind(file);
    char *skip_header = read_line(file);
    free(skip_header);

    int err = calc_stats(file, cols, n);
    fclose(file);
    if (err != 0) {
        free_columns(cols, n);
        return 1;
    }

    printf("rows %lld\n", rows);
    printf("columns %zu\n", n);
    for (size_t i = 0; i < n; i++) {
        if (i > 0) {
            printf("\n");
        }
        print_col_stats(i, &cols[i]);
    }

    free_columns(cols, n);
    return 0;
}