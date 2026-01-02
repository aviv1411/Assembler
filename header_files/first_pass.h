#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <stdio.h>
#include "../header_files/translation_unit.h"

void remove_newline(char * str); 

/**
 * Searches for a symbol by name in the symbol table.
 *
 * @param symbol_table Pointer to the symbol table array.
 * @param sym_Count Number of currently stored symbols in the table.
 * @param name Name of the symbol to search for.
 * @return Pointer to the symbol struct if found, NULL otherwise.
 */
struct symbol *symbolLookUp(struct symbol *symbol_table, const int sym_Count, const char *name);

/**
 * Searches for an external reference by name.
 *
 * @param externals Pointer to the externals array.
 * @param externals_size Number of currently stored external references.
 * @param name Name of the external reference to find.
 * @return Pointer to the ext struct if found, NULL otherwise.
 */
struct ext *extSearch(struct ext *externals, const int externals_size, const char *name);

/**
 * Performs the first pass of the assembler over the source file.
 * Parses each line to build the symbol table, populate the data image,
 * resolve .extern/.entry directives, and validate instructions.
 * Also calculates final instruction and data counters.
 *
 * @param prog Pointer to the main translation_unit structure containing program state.
 * @param amFileName Name of the input file (used for error reporting).
 * @param amFile File pointer to the opened .am source file.
 * @return 1 if any errors occurred during the pass, 0 if successful.
 */
int firstPass(struct translation_unit *prog, const char *amFileName, FILE *amFile);


/**
 * @brief Removes the trailing newline character from a string, if present.
 *
 * This function checks whether the given string ends with the newline character (`\n`)
 * and replaces it with a null terminator (`\0`). This is commonly used when reading
 * lines using `fgets`, which retains the newline.
 *
 * @param str Pointer to the input string. Must be null-terminated.
 */
void remove_newline(char *str);
#endif

