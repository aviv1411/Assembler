#ifndef TEXT_PARSER_H
#define TEXT_PARSER_H

#include "../header_files/ast.h"

/* Max number of tokens expected in a line */

#define MAX_LABEL_LEN 31
#define NUMBER_OF_INSTRACTIONS 16
#define NUMBER_OF_DIRECTIVES 4
#define NUMBER_OF_REGISTERS 8

#define MAX_SIGNED_DATA  ((1 << 23) - 1)    /*  2^23 - 1 = 8,388,607 */
#define MIN_SIGNED_DATA  (-(1 << 23))       /* -2^23     = -8,388,608 */

#define MAX_SIGNED_INST  ((1 << 20) - 1)    /* 2^20 - 1 = 1,048,575 */
#define MIN_SIGNED_INST  (-(1 << 20))       /* -2^20    = -1,048,576 */


#define MAX_REGISTE_NUMBERT 7
#define MIN_REGISTE_NUMBER  0

#define DECIMAL_BASE 10

#define VALID_NUMBER 0
#define TOO_BIG_NUMBER 2
#define INVALID_NUMBER 1

#define DATA 0
#define STRING 1
#define ENTRY 2
#define EXTERN 3
#define NOT_A_DIRECTIVE -1




/**
 * @struct string_seperation_result
 * @brief Holds the result of separating a line into tokens.
 */
struct string_seperation_result {
       char *strings[MAX_LINE_LEN];  /* Pointers to individual tokens */
       int strings_count;          /* Number of tokens found */
};

/**
 * @struct instruction
 * @brief Represents an instruction's metadata.
 */
struct instruction {
       char *name;                              /* Instruction name (e.g., "mov") */
       int opCode;                              /* Opcode value */
       int funct;                               /* Function code */
       const char *operands_legal_types[MAX_NUMBER_OF_OPERANDS];     /* Legal addressing types for src and dest */
       int number_of_operands;                  /* Expected number of operands */
};

/* Global arrays */
extern char *register_names[NUMBER_OF_REGISTERS];                 /* Valid register names: r0 to r7 */
extern char *directive_names[NUMBER_OF_DIRECTIVES];                /* Valid directive names: data, string, entry, extern */
extern struct instruction instruction_table[NUMBER_OF_INSTRACTIONS];/* Table of supported instructions */

/**
 * @brief Appends a new error message to a dynamically allocated error string.
 *
 * @param error Pointer to error string (may be NULL).
 * @param new_msg The error message to append.
 */
void append_error(char **error, const char *new_msg);

/**
 * @brief Checks if a label is valid (not a keyword, starts with letter, etc.).
 *
 * @param str The label to validate.
 * @return 1 if valid, 0 otherwise.
 */
int legal_label(char *str);

/**
 * @brief Validates a label definition (ends with ':'), returns clean label name.
 *
 * @param str Label string including colon.
 * @param label_out Output buffer to hold label name without ':'.
 * @return 1 if valid definition, 0 otherwise.
 */
int legal_label_def(char *str, char *label_out);

/**
 * @brief Checks if a string is a valid immediate number operand (starts with '#').
 *
 * @param str Operand string (e.g., "#42").
 * @param num Output pointer to store parsed number (optional).
 * @return 1 if valid immediate number, 0 otherwise.
 */
int inst_number_operand(char *str, int *num);

/**
 * @brief Checks if a string is a valid register operand (e.g., "r3").
 *
 * @param str Operand string.
 * @param num Output pointer to store register number (optional).
 * @return 1 if valid register, 0 otherwise.
 */
int register_operand(char *str, int *num);

/**
 * @brief Checks if a string is a valid relative label operand (starts with '&').
 *
 * @param str Operand string (e.g., "&LABEL").
 * @param result Output pointer to label name (without '&').
 * @return 1 if valid relative operand, 0 otherwise.
 */
int relative_operand(char *str, char **result);

/**
 * @brief Validates a string literal (must start and end with a quote).
 *
 * @param str The string to validate.
 * @return 1 if valid string literal, 0 otherwise.
 */
int legal_string(char *str);

/**
 * @brief Returns the directive type based on the token (e.g., ".data", ".entry").
 *
 * @param str The directive token.
 * @return Integer code: 0=data, 1=string, 2=entry, 3=extern, or -1 if invalid.
 */
int check_directive(char *str);

/**
 * @brief Searches the instruction table for a matching name.
 *
 * @param str Instruction name to look for.
 * @return Pointer to the matching instruction, or NULL if not found.
 */
struct instruction *check_instruction(char *str);

/**
 * @brief Splits a line into tokens (operands, commas, etc.).
 *
 * @param str The input line.
 * @return A struct containing token pointers and their count.
 */
struct string_seperation_result seperate_string(char *str);

void parse_instruction_operand(char *operand, int operand_number, struct instruction *inst, struct ast *ast);

void parse_instruction_operands(char **operands_array, int size_of_operands_array, struct instruction *inst, struct ast *ast);

void parse_directive_operands(char **operands_array, int size_of_operands_array, int directive_type, struct ast *ast);

#endif /* TEXT_PARSER_H */

