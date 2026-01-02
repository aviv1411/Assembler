#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../header_files/preprocessor.h"
#include "../header_files/mem_alloc.h"






struct Macro * searchMacro(struct Macro * macro_table, const int tableSize, const char *name) {
       int i;

       /* Iterate over all macros in the table */
       for(i = 0; i < tableSize; i++) {
              /* Compare current macro name with the requested name */
              if(strcmp(macro_table[i].mName, name) == STRCMP_TRUE) {
                     /* Macro found, return its address */
                     return &macro_table[i];
              }
       }

       /* Macro not found */
       return NULL;
}




int determine_line_type(char *trimmed_line, struct Macro **macro_pointer, struct MacroTable *macro_table, int * error_flag,int line_counter) {
	/*If it's the end of a macro definition, return macro_end_def*/
	if (is_macro_end_def(trimmed_line,*macro_pointer, error_flag, line_counter)) {
		return macro_end_def;
	}
	/*If it's a macro definition, return macro_def*/
	else if (is_macro_def(trimmed_line, macro_pointer, macro_table, error_flag, line_counter)) {
		return macro_def;
	}
	/*If it's a call to a macro, return macro_call*/
	else if (is_macro_call(trimmed_line, macro_pointer, macro_table)) {
		return macro_call;
	}
	/* If it's any other line type, return any_other_line_type*/
	return any_other_line_type;
	
}

char * skip_leading_whitespace(char *str) {
       /* Advance pointer while current character is a whitespace */
       while (*str == ' ' || *str == '\t' || *str == '\v' || *str == '\r' || *str == '\f' || *str == '\n') {
              str++;
       }

       /* Return pointer to the first non-whitespace character */
       return str;
}



void preprocessor(char *basename, int * error) {
       int error_flag = FALSE;
       FILE *am_file;
       FILE *as_file;
       char line_buffer[LINE_MAX_LEN] = {0};  /* Buffer to store each line read from the input file */
       struct MacroTable macro_table = {NULL, INITIAL_NUMBER_OF_LINES, INITIAL_LINES_CAPASITY}; /* Struct to manage the dynamic macro array */
       struct Macro *macro_pointer = NULL;     /* Pointer to the current macro (if any) */
       char *am_file_name = build_filename(basename, ".am");
       char *as_file_name = build_filename(basename, ".as");
       int i;
       int line_type;
       char * trimmed_line;
       int line_counter = 0;

	

	/*Open the output file (.am) for writing and the input file (.as) for reading*/
	am_file = fopen(am_file_name, "w");  /*Write mode*/
	as_file = fopen(as_file_name, "r");  /*Read mode*/

	if (as_file == NULL) {
              printf("Error: Could not open input file %s\n", as_file_name);
              error_flag = TRUE;
              return;
	}
	/*Loop through each line of the input file*/

	while (fgets(line_buffer, sizeof(line_buffer), as_file) != NULL) {
              line_counter++;
		trimmed_line = skip_leading_whitespace(line_buffer);
		/*Determine the line type (macro definition, macro call, etc.)*/
		line_type = determine_line_type(trimmed_line, &macro_pointer, &macro_table, &error_flag, line_counter);

		switch (line_type) { /*Process the line based on its type*/
			case macro_def:
				break; /*Nothing to do here, macro_pointer is set by macro definition*/
				    
			case macro_end_def:
				/*End of macro definition, clear the macro_pointer*/
				macro_pointer = NULL;
				break;
				    
			case macro_call:
				/*Write the macro's lines to the output file*/
				for (i = 0; i < macro_pointer->line_count; i++) {
					fputs(macro_pointer->lines[i], am_file);  /*Write each line of the macro*/
				}
				/*After calling the macro, reset the macro_pointer*/
				macro_pointer = NULL;
				break;

			case any_other_line_type:
				/*If there's an active macro, add this line to the macro's definition*/
				if (macro_pointer != NULL) {
					add_line_to_macro(macro_pointer, line_buffer, &error_flag);
				}
				else
					fputs(line_buffer, am_file);
				break;
			
		}
	}
	

	/*Close the files after processing*/
	fclose(am_file);
	fclose(as_file);
	free_macro_table(&macro_table);
	*error =  error_flag;
}


void add_line_to_macro(struct Macro *macro_pointer, const char *trimmed_line, int * error_flag) {
       /* Ensure the macro has enough capacity to store a new line */
       if (!ensure_macro_lines_capacity(macro_pointer)) {
                     *error_flag = TRUE;
                     return;  
              }

       /* Copy the line into the next available slot in the macro's lines array */
       strncpy(macro_pointer->lines[macro_pointer->line_count], trimmed_line, LINE_MAX_LEN - 1);

       /* Manually ensure null-termination */
       macro_pointer->lines[macro_pointer->line_count][LINE_MAX_LEN - 1] = '\0';

       /* Increment the macro's line count */
       macro_pointer->line_count++;
}


int is_macro_def(char *trimmed_line, struct Macro **macro_pointer, struct MacroTable *macro_table, int * error_flag, int line_count) 
{
       /* Declare all variables at the top of the block */
       char *macro_name;
       char *after_macro_name;
       struct Macro *new_macro;

       /* Allocate memory for the macro name */
       macro_name = (char *)malloc((MAX_MACRO_LEN + 1) * sizeof(char));
       if (macro_name == NULL) 
       {
              printf("line %d: Memory allocation failed for macro_name.\n", line_count);
              *error_flag = TRUE;
              return FALSE;
       }

       /* Check if the line starts with "mcro" and is not "mcroend" */
       if (strncmp(trimmed_line, "mcro", MACRO_DEF_SIZE) == STRCMP_TRUE && strncmp(trimmed_line, "mcroend", MACRO_END_DEF_SIZE) != STRCMP_TRUE) 
       {
              /* Extract macro name after "mcro" */
              if (sscanf(trimmed_line + MACRO_DEF_SIZE, "%31s", macro_name) != TRUE) {
                     printf("line %d: Error: Missing macro name after 'mcro'.\n", line_count);
                     *error_flag = TRUE;
                     free(macro_name);
                     return FALSE;
              } 

              /* Validate macro name (should not be a reserved instruction) */
              if (!is_valid_macro_name(&macro_name[0], line_count)) 
              {
                     *error_flag = TRUE;
              }

              /* Check for extra characters after the macro name */
              after_macro_name = strstr(trimmed_line + MACRO_DEF_SIZE, macro_name);  
              after_macro_name += strlen(macro_name); 
              after_macro_name = skip_leading_whitespace(after_macro_name);
              if (*after_macro_name != '\0') 
              {
                     *error_flag = TRUE;
                     printf("line %d: Error: Extra characters after macro name '%s'.\n", line_count, macro_name);
              }

              /* Ensure there is space in the macro table */
              if (!ensure_macro_table_capacity(macro_table)) {
                     free(macro_name);
                     *error_flag = TRUE;
                     return FALSE;  
              }

              /* Initialize new macro in table */
              new_macro = &macro_table->macros[macro_table->count];
              strcpy(new_macro->mName, macro_name);
              new_macro->line_count = 0;
              new_macro->lines = NULL;

              *macro_pointer = new_macro;
              macro_table->count++;
              free(macro_name);
              return TRUE;
       }

       /* Line does not define a macro */
       free(macro_name);
       return FALSE;
}


int is_valid_macro_name(char *macro_name, int line_count) {
       /* List of reserved keywords (like instructions) to prevent name conflicts */ 
       const char *invalid_names[NUMBER_OF_INVALID_NAMES] = {
              "mov", "cmp", "add", "sub", "lea",
              "clr", "not", "inc", "dec",
              "jmp", "bne", "jsr",
              "red", "prn", "rts", "stop",
              "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8"
       };
       int i;

       /* Check if macro name matches any reserved word */
       for (i = 0; i < NUMBER_OF_INVALID_NAMES; i++) {
              if (strcmp(macro_name, invalid_names[i]) == STRCMP_TRUE) {
                     printf("line %d: Error: macro name conflicts with an instraction '%s'.\n", line_count, macro_name);
                     return FALSE;  /* Invalid macro name (conflicts with an instruction) */
              }
       }


       if (!isalpha(macro_name[0]) && macro_name[0] != '_') 
       {
              printf("line %d: Error: Invalid macro name '%s'.\n", line_count, macro_name);
              return FALSE; /* the first character is not '_' or a letter*/
       }

       for (i = 1; macro_name[i] != '\0'; i++) {
              if (!isalnum(macro_name[i]) && macro_name[i] != '_') 
              {
                     printf("line %d: Error: Invalid macro name '%s'.\n", line_count, macro_name);
                     return FALSE; /*there is an illigal character*/
              }
       }

       return TRUE;  /* Valid macro name */
}



int is_macro_end_def(char *trimmed_line,struct Macro *macro_pointer, int *error_flag, int line_count) {
	char *after_macro_end_def;
	/*Check if the line is "mcroend", marking the end of the macro definition*/
	if (strncmp(trimmed_line, "mcroend", 7) == STRCMP_TRUE) {
		after_macro_end_def = &trimmed_line[7];
		after_macro_end_def = skip_leading_whitespace(after_macro_end_def);
		if (*after_macro_end_def != '\0') {
			printf("line %d: Error: Extra characters after 'mcroend' in macro definition.\n", line_count);
			*error_flag = TRUE;
		}
		return TRUE;  /*It's the end of a macro definition*/
	}
	return FALSE;  /* Not the end of a macro definition*/
}



int is_macro_call(char *trimmed_line, struct Macro **macro_pointer, struct MacroTable *macro_table) 
{
	/* Look through the macro table to find a matching macro name in the line*/
	int i;
       char *after_macro_name;
	for (i = 0; i < macro_table->count; i++) {
		if (strncmp(trimmed_line, macro_table->macros[i].mName, strlen(macro_table->macros[i].mName)) == STRCMP_TRUE) {
                     after_macro_name = skip_leading_whitespace(trimmed_line + strlen(macro_table->macros[i].mName));

                     if (*after_macro_name == '\0') 
                     {
                            *macro_pointer = &macro_table->macros[i];
			       return TRUE;  /* It's a macro call*/
		       }
	       }
       }
	return FALSE;
}
