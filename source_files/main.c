/**
 * @file main.c
 * @brief Main entry point for the assembler program.
 * 
 * This program processes `.as` assembly files using a two-pass assembler.
 * It performs macro preprocessing, a first pass to build the symbol table and data image,
 * and a second pass to generate object and auxiliary files (.ob, .ent, .ext).
 * 
 * Authors: Omer Orr, Aviv Dan  
 * Date: April 9, 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header_files/mem_alloc.h"
#include "../header_files/preprocessor.h"
#include "../header_files/first_pass.h"
#include "../header_files/second_pass.h"
#include "../header_files/translation_unit.h"
#include "../header_files/output.h"




/**
 * @brief Main function to run the assembler on provided input files.
 * 
 * @param argc Argument count.
 * @param argv Argument vector containing input file base names (without extension).
 * @return int Returns 0 on successful completion.
 */
int main(int argc, char const *argv[]) {
    int i;

    /* Check for required input files */
    if (argc < 2) {
        printf("Usage: assembler file1 [file2 ...]\n");
        return 1;
    }

    /* Process each input file */
    for (i = 1; i < argc; i++) 
    {
        int error = 0;
        char *am_filename = NULL;
        FILE *am_file = NULL;
        struct translation_unit prog = {0}; /* Holds state for processing this file */

        printf("Processing file: %s\n", argv[i]);

        /* === Preprocessing Phase === */
        preprocessor((char *)argv[i], &error);
        if (error) {
            printf("Preprocessor failed on file: %s\n\n", argv[i]);
            continue;
        }

        /* === Open .am file after preprocessing === */
        am_filename = build_filename(argv[i], ".am");
        am_file = fopen(am_filename, "r");
        if (!am_file) {
            printf("Error opening file %s\n", am_filename);
            free(am_filename);
            continue;
        }

        /* === First Pass === */
        error = firstPass(&prog, am_filename, am_file);
        fclose(am_file);  

        /* === Re-open file for Second Pass === */
        am_file = fopen(am_filename, "r");
        if (!am_file) {
            printf("Error reopening file %s for second pass.\n", am_filename);
            free(am_filename);
            continue; 
        }

        /* === Second Pass === */
        error |= secondPass(&prog, am_file);  
        fclose(am_file);

        /* If any error occurred, skip file */
        if (error) {
            free(am_filename);
            continue;
        }

        /* === Output Files === */
        print_ob_file(argv[i], &prog);
        print_ent_file(argv[i], &prog);
        print_ext_file(argv[i], &prog);

        /* === Free resources === */
        free(am_filename);
        free(prog.externals);
        free(prog.entries);
        free(prog.symbol_table);
    }

    return 0;
}