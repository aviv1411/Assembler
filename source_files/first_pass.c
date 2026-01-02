#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header_files/first_pass.h"
#include "../header_files/ast.h"
#include "../header_files/translation_unit.h"
#include "../header_files/mem_alloc.h"



void remove_newline(char *str) {
       size_t len = strlen(str);
       /* Check if the string ends with a newline character */
       if (len > 0 && str[len - 1] == '\n') {
              /* Replace newline with null terminator */
              str[len - 1] = '\0';
       }
}




int firstPass(struct translation_unit *prog, const char* amFileName, FILE *amFile) {
        char line[MAX_LINE_LEN + 1] = {0};
        int ic = 100, dc = 0;
        int errorFlag = FALSE;
        int lineC = 1;
        int i;
        struct ast line_struct = {0};
        struct symbol *SymFind;

        while (fgets(line, sizeof(line), amFile)) {
                remove_newline(line); 
                line_struct = line_ast(line);

                /** If the line contains a syntax error, print it and skip to the next line */
                if (line_struct.error != NULL && line_struct.error[0] != '\0') {
                        printf("%s: line: %d: syntax error: %s\n", amFileName, lineC, line_struct.error);
                        lineC++;
                        errorFlag = TRUE;
                        continue;
                }

                if(line_struct.ast_type == comment || line_struct.ast_type == empty)
                {
                        lineC++;
                        continue;
                }
                /** Handle .extern directive: add symbol to the symbol table with address 0 */
                if (line_struct.ast_type == directive &&
                    line_struct.ast_options.ast_directive.directive_type == ast_extern) {

                        SymFind = symbolLookUp(prog->symbol_table, prog->symCount,
                                line_struct.ast_options.ast_directive.directive_options.label);

                        /** Add symbol only if it's not already in the table */
                        if (!SymFind) 
                        {
                                if (!ensure_symbol_table_capacity(prog)) 
                                {
                                        printf("Memory error: Could not expand symbol table.\n");
                                        errorFlag = TRUE;
                                        break; 
                                }
                                strcpy(prog->symbol_table[prog->symCount].symName, line_struct.ast_options.ast_directive.directive_options.label);
                                prog->symbol_table[prog->symCount].symType = symExtern;
                                prog->symbol_table[prog->symCount].address = 0;
                                prog->symCount++;
                        }

                        lineC++;
                        continue;
                } 

                /** If the line contains a label, add it to the symbol table with appropriate type and address */
                if (line_struct.label_name[0] != '\0' &&
                    (line_struct.ast_type == instruction ||
                     (line_struct.ast_type == directive &&
                      (line_struct.ast_options.ast_directive.directive_type == ast_data ||
                       line_struct.ast_options.ast_directive.directive_type == ast_string)))) {

                        SymFind = symbolLookUp(prog->symbol_table, prog->symCount, line_struct.label_name); 

                        if (SymFind) {
                                /**
                                 * If the symbol was previously marked as .entry,
                                 * assign the correct type and update its address
                                 */
                                if (SymFind->symType == symEntry) {
                                        SymFind->symType = (line_struct.ast_type == instruction) ? symEntryCode : symEntryData;
                                        SymFind->address = (line_struct.ast_type == instruction) ? ic : dc;
                                } else {
                                        /** Otherwise, it's a redefinition error */
                                        printf("%s: error in line %d redefinition of symbol: \"%s\"\n", amFileName, lineC, line_struct.label_name);
                                        errorFlag = TRUE;
                                }
                        } 
                        else {
                                /** Add new symbol with appropriate type and address */
                                if (!ensure_symbol_table_capacity(prog)) {
                                        printf("Memory error: Could not expand symbol table.\n");
                                        errorFlag = TRUE;
                                        break;
                                }
                                strcpy(prog->symbol_table[prog->symCount].symName, line_struct.label_name);
                                prog->symbol_table[prog->symCount].symType = (line_struct.ast_type == instruction) ? symCode : symData;
                                prog->symbol_table[prog->symCount].address = (line_struct.ast_type == instruction) ? ic : dc;
                                prog->symCount++;
                        }
                }

                /** Update instruction counter based on the type of operands */
                if (line_struct.ast_type == instruction) {
                        ic++; /** One word for the instruction itself */
                        if(line_struct.ast_options.ast_instruction.number_of_operands > 0)
                                ic += (line_struct.ast_options.ast_instruction.oprand[0].oprand_type < ast_register);
                        if(line_struct.ast_options.ast_instruction.number_of_operands > 1)
                                ic += (line_struct.ast_options.ast_instruction.oprand[1].oprand_type < ast_register);
                        
                }

                /** Handle .data directive: copy numeric values into the data image */
                else if (line_struct.ast_type == directive &&
                         line_struct.ast_options.ast_directive.directive_type == ast_data) {

                        memcpy(&prog->data_image[prog->DC],
                               line_struct.ast_options.ast_directive.directive_options.data.number,
                               line_struct.ast_options.ast_directive.directive_options.data.number_of_operands * sizeof(int));
                        dc += line_struct.ast_options.ast_directive.directive_options.data.number_of_operands;
                        prog->DC = dc;
                }

                else if (line_struct.ast_type == directive &&
                        line_struct.ast_options.ast_directive.directive_type == ast_string) {

                        /* Extract the string from the AST node */
                        const char *str = line_struct.ast_options.ast_directive.directive_options.string;
                        int len = strlen(str);
                        int i;

                        /* Copy characters of the string (excluding the quotation marks) into the data image */
                        for (i = 1; i < len - 1; i++) /* skipping the quotation marks */ 
                        {
                                if (prog->DC >= MAX_MEMORY_SIZE) {
                                        printf("%s:%d: Error: Data memory overflow while handling .string\n", amFileName, lineC);
                                        errorFlag = TRUE;
                                        break;
                                }
                                prog->data_image[prog->DC++] = (int)str[i];
                                dc++;
                        }

                        /* Add null terminator at the end of the string */
                        if (prog->DC < MAX_MEMORY_SIZE) 
                        {
                                prog->data_image[prog->DC++] = 0;
                                dc++;
                        } 
                        else 
                        {
                                printf("%s:%d: Error: No space for null terminator in .string\n", amFileName, lineC);
                                errorFlag = TRUE;
                        }
                }

                /* Handle .entry directive: mark the symbol as entry if already in the table, or add it */
                else if (line_struct.ast_type == directive &&
                        line_struct.ast_options.ast_directive.directive_type == ast_entry) {

                        /* Look up the symbol in the symbol table */
                        SymFind = symbolLookUp(prog->symbol_table, prog->symCount,
                                        line_struct.ast_options.ast_directive.directive_options.label);

                        if (SymFind) {
                                /* Update the symbol type if it was previously defined as code or data */
                                if (SymFind->symType == symCode) {
                                        SymFind->symType = symEntryCode;
                                } else if (SymFind->symType == symData) {
                                        SymFind->symType = symEntryData;
                                } else {
                                        /* Error: .entry redefinition on existing entry or extern */
                                        printf("%s: error in line %d redefinition of label type: \"%s\"\n",
                                        amFileName, lineC,
                                        line_struct.ast_options.ast_directive.directive_options.label);
                                        errorFlag = TRUE;
                                }
                        } 
                        else {
                                /* If not found, add the symbol as an entry with no address yet */
                                if (!ensure_symbol_table_capacity(prog)) 
                                {
                                        printf("Memory error: Could not expand symbol table.\n");
                                        errorFlag = TRUE;
                                        break; 
                                }
                                strcpy(prog->symbol_table[prog->symCount].symName,
                                line_struct.ast_options.ast_directive.directive_options.label);
                                prog->symbol_table[prog->symCount].symType = symEntry;
                                prog->symCount++;
                        }
                
                }

                lineC++;
        }

        /** Final pass over the symbol table after reading all lines */
        for (i = 0; i < prog->symCount; i++) {
                /** If a symbol was marked as .entry but never defined, raise an error */
                if (prog->symbol_table[i].symType == symEntry) {
                        printf("%s: error symbol: \"%s\" declared entry but was never defined.\n",
                               amFileName, prog->symbol_table[i].symName);
                        errorFlag = TRUE;
                }

                /** For data symbols, add IC to shift them after the code segment */
                if (prog->symbol_table[i].symType == symData ||
                    prog->symbol_table[i].symType == symEntryData) {
                        prog->symbol_table[i].address += ic;
                }

                /** Add all valid entry symbols to the entry list */
                if (prog->symbol_table[i].symType == symEntryCode || prog->symbol_table[i].symType == symEntryData) 
                    {
                        if (!ensure_entries_capacity(prog)) 
                        {
                                printf("Memory error: Could not expand entries array.\n");
                                errorFlag = TRUE;
                                break; 
                        }
                        prog->entries[prog->entries_count] = &prog->symbol_table[i];
                        prog->entries_count++;
                }
        }

        return errorFlag;
}




struct symbol *symbolLookUp(struct symbol *symbol_table, int count, const char *name) {
       int i;
       /* Iterate over all symbols in the table */
       for (i = 0; i < count; i++) {
              /* Compare current symbol name to target name */
              if (strcmp(symbol_table[i].symName, name) == STRCMP_TRUE) {
                     /* Return pointer to the matching symbol */
                     return &symbol_table[i];
              }
       }
       /* Symbol not found */
       return NULL;
}


struct ext *extSearch(struct ext *externals, int externals_size, const char *name) {
       int i;
       /* Iterate over all external references */
       for (i = 0; i < externals_size; i++) {
              /* Compare current external name to target name */
              if (strcmp(name, externals[i].externalName) == STRCMP_TRUE) {
                     /* Return pointer to the matching external */
                     return &externals[i];
              }
       }
       /* External not found */
       return NULL;
}

