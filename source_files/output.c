#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../header_files/translation_unit.h"
#include "../header_files/mem_alloc.h"
#include "../header_files/output.h"

void print_24bit_as_hex(FILE *f, int value) {
       const char hexTable[] = {
              '0','1','2','3','4','5','6','7',
              '8','9','a','b','c','d','e','f'
       };

       int shift;
       const int shift_step = 4;
       const int start_shift = 20;
       const int end_shift = 0;
       const int nibble_mask = 0xF;

       /* Loop through each 4-bit nibble from highest to lowest */
       for (shift = start_shift; shift >= end_shift; shift -= shift_step) {
              /* Extract nibble using bitmask and shift */
              int nibble = (value >> shift) & nibble_mask;

              /* Write the corresponding hex character to the file */
              fputc(hexTable[nibble], f);
       }
}


void print_ob_file(const char *bname, const struct translation_unit *program) {
       char *obFileName;
       FILE *obFile;
       int i, word;
       int address;
       const int start_address = STARTING_ADDRESS;

       /* Build output filename with .ob extension */
       obFileName = build_filename(bname, ".ob");
       if (!obFileName) {
              fprintf(stderr, "Memory allocation failed.\n");
              return;
       }

       /* Open the .ob file for writing */
       obFile = fopen(obFileName, "w");
       if (!obFile) {
              fprintf(stderr, "Could not open file: %s\n", obFileName);
              free(obFileName);
              return;
       }

       /* Write the IC and DC values as a header line */
       fprintf(obFile, "%d %d\n", program->IC, program->DC);

       address = start_address;

       /* Write code section: each instruction word */
       for (i = 0; i < program->IC; i++, address++) {
              word = program->code_image[i];
              fprintf(obFile, "%07d ", address);
              print_24bit_as_hex(obFile, word);
              fputc('\n', obFile);
       }

       /* Write data section: each data word */
       for (i = 0; i < program->DC; i++, address++) {
              word = program->data_image[i];
              fprintf(obFile, "%07d ", address);
              print_24bit_as_hex(obFile, word);
              fputc('\n', obFile);
       }

       /* Close file and free filename memory */
       fclose(obFile);
       free(obFileName);
}


void print_ent_file(const char *bname, const struct translation_unit *program) {
       const char *ent_extension = ".ent";
       char *entFileName;
       FILE *entFile;
       int i;

       /* If there are no entries, do not create a file */
       if (program->entries_count == 0) {
              return;
       }
       entFileName = build_filename(bname, ent_extension);
       entFile = fopen(entFileName, "w");

       /* Check if file was opened successfully */
       if (!entFile) {
              printf("Error: Could not create file %s\n", entFileName);
              free(entFileName);
              return;
       }

       /* Write each entry symbol and its address */
       for (i = 0; i < program->entries_count; i++) {
              fprintf(entFile, "%s\t%07d\n",
                      program->entries[i]->symName,
                      program->entries[i]->address);
       }

       /* Close the file and free memory */
       fclose(entFile);
       free(entFileName);
}


void print_ext_file(const char *bname, const struct translation_unit *program) {
       const char *ext_extension = ".ext";  
       char *extFileName;
       FILE *extFile;
       int i, j;

       if (program->extCount == 0) {
                     return;
              }

       /* Create the filename by adding ".ext" to the base name */
       extFileName = build_filename(bname, ext_extension);

       /* Open the file for writing */
       extFile = fopen(extFileName, "w");

       if(extFile)
       {
              /* Loop over all external symbols */
              for (i = 0; i < program->extCount; i++) 
              {
                     /* For each symbol, loop over all its address occurrences */
                     for (j = 0; j < program->externals[i].address_count; j++) {
                            /* Write a line with the symbol name and its address */
                            fprintf(extFile, "%s\t%07d\n",
                                   program->externals[i].externalName,
                                   program->externals[i].addresses[j]);
                     }
              }
              /* Close the file after writing */
              fclose(extFile);
       }

       else
       {
              printf("Error: Could not create file %s\n", extFileName);
       }

    /* Free the allocated memory for the filename */
    free(extFileName);
}
