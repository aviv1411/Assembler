#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header_files/preprocessor.h"
#include "../header_files/first_pass.h"
#include "../header_files/second_pass.h"
#include "../header_files/mem_alloc.h"
#include "../header_files/translation_unit.h"

int main(int argc, char const *argv[]) {
    int i;

    if (argc < 2) {
        printf("Usage: assembler file1 [file2 ...]\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        int error = 0;
        char *am_filename = NULL;
        FILE *am_file = NULL;
        struct translation_unit prog = {0};

        printf("Processing file: %s\n", argv[i]);


        preprocessor((char *)argv[i], &error);
        if (error) {
            printf("Preprocessor failed on file: %s\n\n", argv[i]);
            continue;
        }


        am_filename = build_filename(argv[i], ".am");
        am_file = fopen(am_filename, "r");
        if (!am_file) {
            fprintf(stderr, "Error opening file %s\n", am_filename);
            free(am_filename);
            continue;
        }

        error = firstPass(&prog, argv[i], am_file);
        fclose(am_file);  


        am_file = fopen(am_filename, "r");
        if (!am_file) {
            fprintf(stderr, "Error reopening file %s for second pass.\n", am_filename);
            free(am_filename);
            continue; 
        }

        error |= secondPass(&prog, am_file);  
        fclose(am_file);

        
        if (error) {
            free(am_filename);
            continue;
        }

        free(am_filename);
    }

    return 0;
}