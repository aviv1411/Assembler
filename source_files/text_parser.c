#include <ctype.h>
#include <stdio.h>
#include "../header_files/ast.h"
#include "../header_files/text_parser.h"
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define END_OF_WORD ", \t\v\f\""
#define MAX_NUMBER INT_MAX
#define MIN_NUMBER INT_MIN 

char * register_names[NUMBER_OF_REGISTERS]   = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char * directive_names[NUMBER_OF_DIRECTIVES]  = {"data", "string", "entry", "extern"};

struct instruction instruction_table[NUMBER_OF_INSTRACTIONS] = 
{
    {"mov",  0,   0, {"013", "13" }, 2},
    {"cmp",  1,   0, {"013", "013"}, 2},
    {"add",  2,   1, {"013", "13" }, 2},
    {"sub",  2,   2, {"013", "13" }, 2},
    {"lea",  4,   0, {"1"  , "13" }, 2},
    {"clr",  5,   1, {""   , "13" }, 1},
    {"not",  5,   2, {""   , "13" }, 1},
    {"inc",  5,   3, {""   , "13" }, 1},
    {"dec",  5,   4, {""   , "13" }, 1},
    {"jmp",  9,   1, {""   , "12" }, 1},
    {"bne",  9,   2, {""   , "12" }, 1},
    {"jsr",  9,   3, {""   , "12" }, 1},
    {"red",  12,  0, {""   , "13" }, 1},
    {"prn",  13,  0, {""   , "013"}, 1},
    {"rts",  14,  0, {""   , ""   }, 0},
    {"stop", 15,  0, {""   , ""   }, 0}
};




int legal_number(char *str, int min, int max, int *result, int allow_sign) {
        char *ptr = str;
        char *endptr;
        long val;

        /* Reject null or empty string */
        if (str == NULL || *str == '\0')
                return INVALID_NUMBER;

        /* Skip optional sign if allowed */
        if ((*ptr == '-' || *ptr == '+') && allow_sign)
                ptr++;

        /* Validate that the string contains only digits */
        if (!isdigit((unsigned char)*ptr))
                return INVALID_NUMBER;

        while (*ptr) {
                if (!isdigit((unsigned char)*ptr))
                        return INVALID_NUMBER;
                ptr++;
        }

        /* Convert to long and check if within range */
        val = strtol(str, &endptr, DECIMAL_BASE);
        if (val < min || val > max)
                return TOO_BIG_NUMBER;

        /* Valid number — store and return success */
        *result = (int)val;
        return VALID_NUMBER;
}


int legal_label(char *str) {
        int i;

        /* Label must start with a letter and be between 1–31 characters */
        if (str == NULL || strlen(str) > MAX_LABEL_LEN || strlen(str) < 1 || !isalpha(*str))
                return FALSE;

        /* All middle characters must be alphanumeric */
        for (i = 1; i < strlen(str) - 1; i++) {
                if (!isalpha(str[i]) && !isdigit(str[i])) {
                        return FALSE;
                }
        }

        /* Last character must be alphanumeric or newline */
        if (!isalpha(str[strlen(str) - 1]) &&
            !isdigit(str[strlen(str) - 1]) &&
            str[strlen(str) - 1] != '\n') {
                return FALSE;
        }

        /* Check for conflicts with instruction, register, or directive names */
        for (i = 0; i < NUMBER_OF_INSTRACTIONS; i++) {
                if (strcmp(instruction_table[i].name, str) == STRCMP_TRUE)
                        return FALSE;
                if (i < NUMBER_OF_REGISTERS && strcmp(register_names[i], str) == STRCMP_TRUE)
                        return FALSE;
                if (i < NUMBER_OF_DIRECTIVES && strcmp(directive_names[i], str) == STRCMP_TRUE)
                        return FALSE;
        }

        return TRUE;
}



int legal_label_def(char *str, char *label_out) {
        char temp_label[MAX_LABEL_LEN + 1];

        /* Must be at least 2 characters and end with ':' */
        if (!str || strlen(str) < 2 || str[strlen(str) - 1] != ':')
                return FALSE;

        /* Copy label without ':' into temporary buffer */
        strncpy(temp_label, str, strlen(str) - 1);
        temp_label[strlen(str) - 1] = '\0';

        /* Validate label structure and name conflicts */
        if (!legal_label(temp_label))
                return FALSE;

        /* Store validated label in output */
        strcpy(label_out, temp_label);
        return TRUE;
}



int legal_string(char *str) {
        /* Valid string must be at least 2 chars and wrapped in double quotes */
        if(str != NULL && strlen(str) >= 2 && str[0] == '"' && str[strlen(str) - 1] == '"')
                return TRUE;

        return FALSE;
}


int register_operand(char *str, int *num) {
        int result;

        /* Must start with 'r' and be followed by a number */
        if (str == NULL || str[0] != 'r' || str[1] == '\0')
                return FALSE;

        /* Validate number is in range 0–7 */
        if (legal_number(&str[1], MIN_REGISTE_NUMBER, MAX_REGISTE_NUMBERT, &result, FALSE) == VALID_NUMBER) {
                if (num != NULL)
                        *num = result;
                return TRUE;
        }

        return FALSE;
}





int inst_number_operand(char *str, int *num)
{
	int result;

	/* Check that the string starts with '#' and has more characters after it */
	if (str == NULL || str[0] != '#' || str[1] == '\0') {
		return FALSE;
	}

	/* Check if the rest of the string is a legal number within range */
	if (legal_number(&str[1], MIN_SIGNED_INST, MAX_SIGNED_INST, &result, TRUE) == 0) {
		/* If the output pointer is provided, store the result */
		if (num != NULL) { 
			*num = result; 
		}
		return TRUE;
	}

	/* Not a valid immediate number operand */
	return FALSE;
}

int relative_operand(char *str, char **result) {
        /* Must start with '&' followed by a valid label */
	if (str == NULL || str[0] != '&' || str[1] == '\0')
                return FALSE;

        /* Validate label and return pointer to it */
        if (legal_label(&str[1])) {
                *result = &str[1];
                return TRUE;
        }

        return FALSE;
}

int check_commas_structure(char **arr, int size) {
        int i;

        /* NULL array is considered valid (no operands) */
        if (arr == NULL)
                return TRUE;

        /* Cannot start or end with a comma */
        if (strcmp(arr[0], ",") == STRCMP_TRUE || strcmp(arr[size - 1], ",") == STRCMP_TRUE)
                return FALSE;

        /* Must alternate between operand and comma */
        for (i = 0; i < size - 1; i++) {
                if (strcmp(arr[i], ",") == STRCMP_TRUE && strcmp(arr[i + 1], ",") == STRCMP_TRUE)
                        return FALSE;

                if (strcmp(arr[i], ",") != STRCMP_TRUE && strcmp(arr[i + 1], ",") != STRCMP_TRUE)
                        return FALSE;
        }

        return TRUE;
}



void parse_instruction_operand(char *operand, int operand_number, struct instruction *inst, struct ast *ast)
{
       int destination = 1;
       int number;
       char *label;
       int register_number;
       char operand_type = '4';

       /* Determine the operand type by checking format and content */
       if (inst_number_operand(operand, &number))
              operand_type = '0';
       else if (legal_label(operand))
              operand_type = '1';
       else if (relative_operand(operand, &label))
              operand_type = '2';
       else if (register_operand(operand, &register_number))
              operand_type = '3';

       /* If the instruction has two operands and this is the first, it's a source operand */
       if (inst->number_of_operands == 2 && operand_number == 0)
              destination = 0;

       /* If the operand type is legal for this instruction and position, encode it in AST */
       if (strchr(inst->operands_legal_types[destination], operand_type))
       {
              switch (operand_type)
              {
                     case '0':
                            /* Encode immediate value */
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_type = ast_instant;
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_options.number = number;
                            break;

                     case '1':
                            /* Encode direct label */
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_type = ast_direct;
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_options.label = operand;
                            break;

                     case '2':
                            /* Encode relative label */
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_type = ast_relative;
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_options.label = label;
                            break;

                     case '3':
                            /* Encode register number */
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_type = ast_register;
                            ast->ast_options.ast_instruction.oprand[operand_number].oprand_options.register_number = register_number;
                            break;
              }
       }
       else
       {
              /* Operand type is invalid for this instruction */
              append_error(&ast->error, "illigal type of operand");
       }
}



void parse_instruction_operands(char **operands_array, int size_of_operands_array, struct instruction *inst, struct ast *ast)
{
       int operand_counter = 0;
       int inst_number_of_operands = inst->number_of_operands;
       int i;

       /* Check if there are any operands and validate comma structure */
       if (size_of_operands_array > 0)
       {
              if (!check_commas_structure(operands_array, size_of_operands_array))
              {
                     append_error(&ast->error, "iilagal use of commas");
              }
       }

       /* Iterate through the operands array and parse each actual operand */
       for (i = 0; i < size_of_operands_array; i++)
       {
              if (strcmp(operands_array[i], ",") != STRCMP_TRUE)
              {
                     operand_counter++;

                     /* If number of operands exceeds expected amount, mark error */
                     if (operand_counter > inst_number_of_operands)
                     {
                            append_error(&ast->error, "too many operands");
                            break;
                     }

                     /* Parse the operand and store its info in AST */
                     parse_instruction_operand(operands_array[i], operand_counter - 1, inst, ast);
              }
       }

       /* If not enough operands were provided, mark error */
       if (operand_counter < inst_number_of_operands)
       {
              append_error(&ast->error, "too few operands");
       }

       /* Store the final number of parsed operands in the AST */
       ast->ast_options.ast_instruction.number_of_operands = operand_counter;
}


void parse_directive_operands(char **operands_array, int size_of_operands_array, int directive_type, struct ast *ast)
{
        int i;
        int result;
        int counter = 0;

        /* Check if operand list is empty */
        if(size_of_operands_array == 0)
        {
                append_error(&ast->error, "too few operands");
        }
        else
        {
                /* Check for invalid comma usage */
                if(!(check_commas_structure(operands_array, size_of_operands_array)))
                {
                        append_error(&ast->error, "iilagal use of commas");
                }
        }

        /* Only .data (type 0) can have more than one operand */
        if(directive_type != DATA && size_of_operands_array > 1)
        {
                append_error(&ast->error, "to many operands");
        }

        switch(directive_type)
        {
        case DATA:
                /* Handle .data directive */
                for(i = 0; i < size_of_operands_array; i++)
                {
                        if(strcmp(operands_array[i], ",") != STRCMP_TRUE)
                        {
                                /* Parse and store number if valid */
                                if(legal_number(operands_array[i], MIN_SIGNED_DATA, MAX_SIGNED_DATA, &result, TRUE) == 0)
                                {
                                        ast->ast_options.ast_directive.directive_options.data.number[counter] = result;
                                        ast->ast_options.ast_directive.directive_options.data.number_of_operands++;
                                }
                                else
                                {
                                        append_error(&ast->error, "illegal number");
                                }
                                counter++;
                        }
                }
                break;

        case STRING:
                /* Handle .string directive */
                if(legal_string(operands_array[0]))
                {
                        ast->ast_options.ast_directive.directive_options.label = operands_array[0];
                }
                else
                {
                        append_error(&ast->error, "illigal string");
                }
                break;

        case ENTRY:
        case EXTERN:
                /* Handle .entry and .extern */
                if(legal_label(operands_array[0]))
                {
                        if(operands_array[0][strlen(operands_array[0]) - 1] == '\n')
                                operands_array[0][strlen(operands_array[0]) - 1] = '\0';

                        ast->ast_options.ast_directive.directive_options.label = operands_array[0];
                }
                else
                {
                        append_error(&ast->error, "illigal label");
                }
                break;
        }
}







int check_directive(char *str)
{
        if(str == NULL)
                return -1;
        /* Check if the directive is ".data" */
        if (strcmp(str, ".data") == STRCMP_TRUE)
        {
                return DATA; /* Return 0 for .data directive */
        }
        /* Check if the directive is ".string" */
        else if (strcmp(str, ".string") == STRCMP_TRUE)
        {
                return STRING; /* Return 1 for .string directive */
        }
        /* Check if the directive is ".entry" */
        else if (strcmp(str, ".entry") == STRCMP_TRUE)
        {
                return ENTRY; /* Return 2 for .entry directive */
        }
        /* Check if the directive is ".extern" */
        else if (strcmp(str, ".extern") == STRCMP_TRUE)
        {
                return EXTERN; /* Return 3 for .extern directive */
        }

        /* If none of the directives match, return -1 */
        return NOT_A_DIRECTIVE;
}



struct instruction *check_instruction(char *str) {
        int i;
        if(str != NULL)
        {
                /* Loop through all instructions in the instruction_table */
                for (i = 0; i < NUMBER_OF_INSTRACTIONS; i++) {
                        /* Compare the input string with the instruction name */
                        if (strcmp(str, instruction_table[i].name) == STRCMP_TRUE) {
                                return &instruction_table[i];  /* Return the matching instruction */
                        }
                }
        }
        return NULL;  /* Return NULL if no match is found */
}





struct string_seperation_result seperate_string(char *str) {
       int strings_counter = 0;
       char *s;
       struct string_seperation_result result = {EMPTY};

       /* Skip leading whitespace */
       while (isspace(*str)) str++;

       /* If the string is empty, return the result as is */
       if (*str == '\0') return result;

       do {
              /* Handle comma separation */
              if (*str == ',') {
                     result.strings[strings_counter++] = ",";
                     str++;
                     while (isspace(*str)) str++; 
              } 
              else {
                     result.strings[strings_counter++] = str;

                     /* Find the next delimiter (comma, space, or quote) */
                     s = strpbrk(str, END_OF_WORD);

                     if (s) {
                            char delimiter = *s;

                            /* If it's a quote, skip over the quoted section */
                            if (delimiter == '"') {
                                   s = strpbrk(strrchr(str, '\"') + 1, END_OF_WORD);
                                   if (s == NULL) break;
                                   if (*s == ',') {
                                          result.strings[strings_counter++] = ",";
                                   }
                            }

                            *s = '\0';  /* Null-terminate the string */
                            s++;
                            while (isspace(*s)) s++;

                            /* If the delimiter is a comma, add it to the list */
                            if (delimiter == ',') {
                                   result.strings[strings_counter++] = ",";
                            }

                            /* If the string ends, break the loop */
                            if (*s == '\0') break;
                            str = s;  /* Move the pointer to the next token */
                     } 
                     else {
                            break;
                     }
              }
       } while (*str != '\0' && strings_counter < MAX_LINE_LEN);

       /* Set the count of strings found */
       result.strings_count = strings_counter;
       return result;
}

void append_error(char **error, const char *new_msg) {
       /* Get the current length of the error message (0 if NULL) */
       size_t current_len = (*error) ? strlen(*error) : 0;

       /* Calculate the new length for the error message (including \n and \0) */
       size_t new_len = current_len + strlen(new_msg) + 2; /* +2 for \n + \0 */

       /* Attempt to reallocate memory for the error message */
       char *temp = realloc(*error, new_len);
       if (!temp) return;  /* If realloc fails, do nothing */

       /* Update the original pointer with the new memory */
       *error = temp;

       /* If this is the first error message, copy it directly */
       if (current_len == 0)
              strcpy(*error, new_msg);  /* First message */
       else {
              /* Add a newline before appending the new message */
              strcat(*error, "\t");
              strcat(*error, new_msg);  /* Append the new error message */
       }
}


