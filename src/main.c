/* Точка входа в программу. Проверяет аргументы командной строки
   и запускает нужный режим работы (--help, schema или stats). */

#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "csv.h"

int main(int argc, char *argv[]) {
    if(argc < 2){
        fprintf(stderr, "Error: Arguments required. Try '--help'.\n");
        return 2;
    }

    if(strcmp(argv[1], "--help") == 0){
        if(argc != 2){
            fprintf(stderr, "Error: too many arguments for '--help'.\n");
            return 2;
        }
        print_help();
        return 0;
    }

    if(strcmp(argv[1], "schema") == 0){
        if(argc != 3){
            fprintf(stderr, "Error: Not CSV file for 'schema' command.\n");
            return 2;
        }
        return handle_schema(argv[2]);
    }

    if(strcmp(argv[1], "stats") == 0){
        if(argc != 3){
            fprintf(stderr, "Error: Not CSV file for 'stats' command.\n");
            return 2;
        }
        return handle_stats(argv[2]);
    }

    fprintf(stderr, "Error: Unknown command '%s'. Try '--help'.\n", argv[1]);
    return 2;
}   