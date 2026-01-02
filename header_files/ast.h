#ifndef AST_H
#define AST_H
/**
 * @file ast.h
 * @brief Defines the abstract syntax tree (AST) structure for parsing a line in the assembler.
 * 
 * The AST captures all relevant information about a parsed line: whether it's an instruction, directive,
 * comment, or empty line. If it's a directive or instruction, the corresponding operands and metadata
 * are stored in detailed substructures. This data is used by the assembler in later stages of translation.
 */
#define MAX_NUMBER_OF_OPERANDS 2

#define MAX_LINE_LEN 80

#define TRUE 1
#define FALSE 0

#define STRCMP_TRUE 0
#define EMPTY 0


/**
 * @struct ast
 * @brief Represents the parsed structure of a single line in the assembly source file.
 */
struct ast
{
       /** Error message if parsing fails */
       char *error;

       /** Label defined at the beginning of the line, if any */
       char label_name[31];

       /** Type of line parsed: instruction, directive, comment, or empty */
       enum
       {
              instruction,
              directive,
              comment,
              empty
       } ast_type;

       /** Holds parsed data for instruction or directive depending on ast_type */
       union 
       {
              /**
               * Data specific to directive lines (e.g., .data, .string, .entry, .extern)
               */
              struct 
              {
                     /** Type of directive */
                     enum
                     {
                            ast_data,
                            ast_string,
                            ast_entry,
                            ast_extern
                     } directive_type;

                     /** Operands associated with the directive, varies by directive_type */
                     union 
                     {
                            struct
                            {
                                   int number_of_operands;   /**< Count of numeric operands for .data */
                                   int number[MAX_LINE_LEN];           /**< Array of numeric values */
                            } data;

                            char *string;  /**< String content for .string directive */
                            char *label;   /**< Label name for .entry or .extern directives */

                     } directive_options;

              } ast_directive;

              /**
               * Data specific to instruction lines (e.g., MOV, ADD, JMP, etc.)
               */
              struct
              {
                     int number_of_operands; /**< Number of operands used in the instruction */

                     /** Operation code enum representing instruction mnemonic */
                     enum 
                     {
                            OP_MOV  = 0,
                            OP_CMP  = 1,
                            OP_ADD  = 2,
                            OP_SUB  = 2,
                            OP_LEA  = 4,
                            OP_CLR  = 5,
                            OP_NOT  = 5,
                            OP_INC  = 5,
                            OP_DEC  = 5,
                            OP_JMP  = 9,
                            OP_BNE  = 9,
                            OP_JSR  = 9,
                            OP_RED  = 12,
                            OP_PRN  = 13,
                            OP_RTS  = 14,
                            OP_STOP = 15
                     } opCode;

                     /** Function code enum for more specific instruction variants */
                     enum
                     {
                            FUNCT_MOV  = 0,
                            FUNCT_CMP  = 1,
                            FUNCT_ADD  = 1,
                            FUNCT_SUB  = 2,
                            FUNCT_LEA  = 0,
                            FUNCT_CLR  = 1,
                            FUNCT_NOT  = 2,
                            FUNCT_INC  = 3,
                            FUNCT_DEC  = 4,
                            FUNCT_JMP  = 1,
                            FUNCT_BNE  = 2,
                            FUNCT_JSR  = 3,
                            FUNCT_RED  = 0,
                            FUNCT_PRN  = 0,
                            FUNCT_RTS  = 0,
                            FUNCT_STOP = 0
                     } funct;

                     /** Instruction operands (up to 2), with type and value */
                     struct
                     {
                            /** Operand addressing method */
                            enum
                            {
                                   ast_instant,
                                   ast_direct,
                                   ast_relative,
                                   ast_register
                            } oprand_type;

                            /** Data associated with operand depending on type */
                            union
                            {
                                   int number;              /**< Value for immediate operand */
                                   char *label;             /**< Label name for direct/relative operand */
                                   int register_number;     /**< Register number for register operand */
                            } oprand_options;

                     } oprand[MAX_NUMBER_OF_OPERANDS];

              } ast_instruction;

       } ast_options;
};

/**
 * @brief Parses a single line of assembly code into an abstract syntax tree (AST) structure.
 * 
 * @param line The input assembly line as a null-terminated string.
 * @return struct ast, a filled AST structure representing the parsed line.
 */
struct ast line_ast(char * line);

#endif /* AST_H */