/* Вывод краткой справки по командам программы. */

#include <stdio.h>
#include "cli.h"

void print_help(void){
    printf("Use this command:\n");
    printf("   csvstat --help\n");
    printf("   csvstat schema <csv-file>\n");
    printf("   csvstat stats <csv-file>\n");
}