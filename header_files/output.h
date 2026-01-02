#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

#include <stdio.h>
#include "../header_files/translation_unit.h"



/*
 * Prints a 24-bit value to the given file in hexadecimal format (6 hex characters).
 *
 * Parameters:
 *   f     - The file to write to.
 *   value - The 24-bit integer to print.
 */
void print_24bit_as_hex(FILE *f, int value);

/*
 * Generates the object file (.ob) containing instruction and data memory.
 *
 * Parameters:
 *   bname   - The base name of the file.
 *   program - Pointer to the translation unit containing IC, DC, and memory images.
 */
void print_ob_file(const char *bname, const struct translation_unit *program);

/*
 * Generates the entry file (.ent) containing all entry symbols and their addresses.
 *
 * Parameters:
 *   bname   - The base name of the file.
 *   program - Pointer to the translation unit containing entries.
 */
void print_ent_file(const char *bname, const struct translation_unit *program);

/*
 * Generates the extern file (.ext) listing external symbol references and addresses.
 *
 * Parameters:
 *   bname   - The base name of the file.
 *   program - Pointer to the translation unit containing external symbol references.
 */
void print_ext_file(const char *bname, const struct translation_unit *program);

#endif /* OUTPUT_FILES_H */
