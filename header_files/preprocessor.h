#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include <string.h>

/* Maximum lengths for macro names and lines */
#define MAX_MACRO_LEN 31
#define LINE_MAX_LEN 80

#define INITIAL_NUMBER_OF_LINES 0
#define INITIAL_LINES_CAPASITY 0

#define MACRO_DEF_SIZE 4
#define MACRO_END_DEF_SIZE 7

#define TRUE 1
#define FALSE 0
#define STRCMP_TRUE 0

#define NUMBER_OF_INVALID_NAMES 25

/**
 * @struct Macro
 * @brief Represents a single macro definition.
 */
struct Macro {
       char mName[MAX_MACRO_LEN + 1];           /* Macro name */
       char (*lines)[LINE_MAX_LEN];             /* Dynamic array of lines in the macro */
       int line_count;                          /* Number of lines in the macro */
       int capacity;                            /* Allocated capacity for lines */
};

/**
 * @enum type
 * @brief Enumeration of possible line types during preprocessing.
 */
enum type {
       macro_def = 1,           /* Start of a macro definition */
       macro_end_def,           /* End of a macro definition */
       macro_call,              /* Line is a macro call */
       any_other_line_type      /* Regular line (not macro related) */
};

/**
 * @struct MacroTable
 * @brief Represents the macro table holding all defined macros.
 */
struct MacroTable {
       struct Macro *macros;    /* Dynamic array of macros */
       int count;               /* Number of macros stored */
       int capacity;            /* Allocated size of the macros array */
};

/**
 * @brief Searches for a macro by name in the macro table.
 *
 * @param macro_table Pointer to the macro array.
 * @param tableSize Number of macros in the table.
 * @param name Name of the macro to search for.
 * @return Pointer to the macro if found, NULL otherwise.
 */
struct Macro *searchMacro(struct Macro *macro_table, const int tableSize, const char *name);

/**
 * @brief Determines whether a line starts a macro definition.
 *
 * @param trimmed_line Line to analyze.
 * @param macro_pointer Pointer to update with the new macro if defined.
 * @param macro_table Pointer to the macro table to add the macro to.
 * @param error_flag Pointer to an error flag to set if needed.
 * @param line_count counter of the lines.
 * @return 1 if line defines a macro, 0 otherwise.
 */
int is_macro_def(char *trimmed_line, struct Macro **macro_pointer, struct MacroTable *macro_table, int *error_flag, int line_count);
/**
 * @brief Validates a macro name against reserved keywords.
 *
 * @param macro_name The name of the macro to validate.
 * @param line_count counter of the lines.
 * @return 1 if valid, 0 if name conflicts with reserved keywords or registers.
 */
int is_valid_macro_name(char *macro_name, int line_count);

/**
 * @brief Checks if a line marks the end of a macro definition ("mcroend").
 *
 * @param trimmed_line The line to check.
 * @param macro_pointer The current macro being defined.
 * @param error_flag Pointer to an error flag to set if needed.
 * @param line_count counter of the lines.
 * @return 1 if line is "mcroend", 0 otherwise.
 */
int is_macro_end_def(char *trimmed_line, struct Macro *macro_pointer, int *error_flag, int line_count);

/**
 * @brief Checks if a line is a macro call.
 *
 * @param line_buffer The line to analyze.
 * @param macro_pointer Output pointer to the matched macro (if found).
 * @param macro_table Pointer to the macro table.
 * @return 1 if the line is a macro call, 0 otherwise.
 */
int is_macro_call(char *line_buffer, struct Macro **macro_pointer, struct MacroTable *macro_table);

/**
 * @brief Adds a line of code to the body of the current macro.
 *
 * @param macro_pointer Pointer to the macro to add the line to.
 * @param trimmed_line The line to add.
 * @param error_flag Pointer to an error flag to set if needed.
 */
void add_line_to_macro(struct Macro *macro_pointer, const char *trimmed_line, int * error_flag);

/**
 * @brief Runs the preprocessor stage: expands macros and outputs a cleaned .am file.
 *
 * @param basename Base name of the source file (without extension).
 * @param error Pointer to an int that will be set to 1 if any error occurred.
 */
void preprocessor(char *basename, int *error);

/**
 * @brief Determines the type of a given line: macro definition, call, end, or regular line.
 *
 * @param trimmed_line The line to classify.
 * @param macro_pointer Pointer to the current macro being defined or called.
 * @param macro_table Pointer to the macro table.
 * @param error_flag Pointer to an error flag to set if needed.
 * @param line_count counter of the lines.
 * @return A value from enum type indicating the kind of line.
 */
int determine_line_type(char *trimmed_line, struct Macro **macro_pointer, struct MacroTable *macro_table, int *error_flag, int line_count);

/**
 * @brief Skips leading whitespace characters in a string.
 *
 * @param str The string to trim.
 * @return Pointer to the first non-whitespace character in the string.
 */
char *skip_leading_whitespace(char *str);

#endif /* PREPROCESSOR_H */

