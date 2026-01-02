#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include "../header_files/translation_unit.h"
#include "../header_files/preprocessor.h"

#define INITIAL_CAPASITY 4

/**
 * Ensures the symbol table has enough capacity to store a new symbol.
 *
 * @param prog Pointer to the translation unit.
 * @return 1 if successful, 0 on memory allocation failure.
 */
int ensure_symbol_table_capacity(struct translation_unit *prog);

/**
 * Ensures the externals array has enough capacity to store a new external.
 *
 * @param prog Pointer to the translation unit.
 * @return 1 if successful, 0 on memory allocation failure.
 */
int ensure_externals_capacity(struct translation_unit *prog);

/**
 * Ensures the entries array has enough capacity to store a new entry.
 *
 * @param prog Pointer to the translation unit.
 * @return 1 if successful, 0 on memory allocation failure.
 */
int ensure_entries_capacity(struct translation_unit *prog);

/**
 * Builds a filename by appending the extension to the base name.
 * Allocates memory for the result.
 *
 * @param base_name The base name without extension.
 * @param extension The extension to add (e.g. ".am").
 * @return Pointer to newly allocated string, or NULL on failure.
 */
char *build_filename(const char *base_name, const char *extension);

/**
 * Ensures the macro table has enough capacity to store a new macro.
 *
 * @param table Pointer to the macro table.
 * @return 1 if successful, 0 on memory allocation failure.
 */
int ensure_macro_table_capacity(struct MacroTable *table);

/**
 * Ensures a macro has enough capacity to store another line.
 *
 * @param macro Pointer to the macro.
 */
int ensure_macro_lines_capacity(struct Macro *macro);

/**
 * Frees all memory associated with a macro table.
 *
 * @param table Pointer to the macro table.
 */
void free_macro_table(struct MacroTable *table);

#endif /* MEM_ALLOC_H */

