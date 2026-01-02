#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header_files/translation_unit.h"
#include "../header_files/preprocessor.h"
#include "../header_files/mem_alloc.h"

int ensure_symbol_table_capacity(struct translation_unit *prog) {
       /* Check if symbol table is full */
       if (prog->symCount >= prog->symCapacity) {
              /* Calculate new capacity: start with 4 or double the current */
              int new_capacity = (prog->symCapacity == 0) ? INITIAL_CAPASITY : prog->symCapacity * 2;

              /* Attempt to reallocate the symbol table with the new size */
              struct symbol *new_table = realloc(prog->symbol_table, new_capacity * sizeof(struct symbol));
              if (!new_table) {
                     /* Allocation failed */
                     printf("Failed to realloc symbol table\n");
                     return 0;
              }

              /* Update symbol table and capacity */
              prog->symbol_table = new_table;
              prog->symCapacity = new_capacity;
       }

       /* Table has sufficient capacity */
       return 1;
}


int ensure_externals_capacity(struct translation_unit *prog) {
       /* Check if externals array is full */
       if (prog->extCount >= prog->extCapacity) {
              /* Calculate new capacity: start with 4 or double the current */
              int new_capacity = (prog->extCapacity == 0) ? INITIAL_CAPASITY : prog->extCapacity * 2;

              /* Attempt to reallocate the externals array */
              struct ext *new_ext = realloc(prog->externals, new_capacity * sizeof(struct ext));
              if (!new_ext) {
                     /* Allocation failed */
                     printf("Failed to realloc externals\n");
                     return 0;
              }

              /* Update externals array and capacity */
              prog->externals = new_ext;
              prog->extCapacity = new_capacity;
       }

       /* Externals array has sufficient capacity */
       return 1;
}



int ensure_entries_capacity(struct translation_unit *prog) {
       /* Check if entries array is full */
       if (prog->entries_count >= prog->entries_capacity) {
              /* Calculate new capacity: start with 4 or double the current */
              int new_capacity = (prog->entries_capacity == 0) ? INITIAL_CAPASITY : prog->entries_capacity * 2;

              /* Attempt to reallocate the entries array */
              struct symbol **new_entries = realloc(prog->entries, new_capacity * sizeof(struct symbol *));
              if (!new_entries) {
                     /* Allocation failed */
                     printf("Failed to realloc entries\n");
                     return 0;
              }

              /* Update entries array and capacity */
              prog->entries = new_entries;
              prog->entries_capacity = new_capacity;
       }

       /* Entries array has sufficient capacity */
       return 1;
}

char *build_filename(const char *base_name, const char *extension)
{
       char *result;
       size_t base_len = strlen(base_name);
       size_t ext_len = strlen(extension);
       size_t total_len = base_len + ext_len;
       size_t null_terminator_size = 1;

       /* Allocate memory for the full filename including null terminator */
       result = (char *)malloc(total_len + null_terminator_size);
       if (result == NULL) {
              /* Allocation failed */
              return NULL;
       }

       /* Copy base name and append extension */
       strcpy(result, base_name);
       strcat(result, extension);

       return result;
}



int ensure_macro_table_capacity(struct MacroTable *table) {
       int new_capacity;
       struct Macro *new_macros;

       /* Check if macro table is full */
       if(table->count >= table->capacity) {
              /* Calculate new capacity: start with 4 or double the current */
              if(table->capacity == 0) 
                     new_capacity = INITIAL_CAPASITY;
              else
                     new_capacity = table->capacity * 2;

              /* Attempt to reallocate the macro array */
              new_macros = realloc(table->macros, new_capacity * sizeof(struct Macro));
              if (!new_macros) {
                     /* Allocation failed */
                     printf("Memory allocation failed while expanding macro table.\n");
                     return 0;
              }

              /* Update macro table and capacity */
              table->macros = new_macros;
              table->capacity = new_capacity;
       }

       /* Macro table has sufficient capacity */
       return 1;
}



int ensure_macro_lines_capacity(struct Macro *macro) {
       int new_capacity;
       char (*new_lines)[LINE_MAX_LEN];

       /* Check if macro has reached its current line capacity */
       if (macro->line_count >= macro->capacity) 
       {
              /* Calculate new capacity: start with 4 or double the current */
              if(macro->capacity == 0) 
                     new_capacity = INITIAL_CAPASITY; 
              else 
                     new_capacity = macro->capacity * 2;

              /* Attempt to reallocate the lines array */
              new_lines = realloc(macro->lines, sizeof(char[LINE_MAX_LEN]) * new_capacity);
              if (!new_lines) {
                     /* Allocation failed — abort program */
                     printf("Error: Failed to allocate memory for macro lines.\n");
                     return FALSE;
              }

              /* Update lines array and capacity */
              macro->lines = new_lines;
              macro->capacity = new_capacity;
       }
       return TRUE;
}


 
void free_macro_table(struct MacroTable *table) {
       int i;

       /* Check if table or its macros array is NULL */
       if (!table || !table->macros) return;

       /* Free memory for each macro's lines */
       for (i = 0; i < table->count; i++) {
              free(table->macros[i].lines);
       }

       /* Free the macros array itself */
       free(table->macros);
}
