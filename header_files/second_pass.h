#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include "../header_files/translation_unit.h"

#define MAX_SYMBOLS 100
#define MAX_MEM_SIZE 1024
#define A 4
#define R 2
#define E 1 
#define ARE_SHIFT        3
#define OPCODE_SHIFT     18
#define FUNCT_SHIFT       3
#define OPERAND_TYPE_SOURCE_SHIFT   16
#define OPERAND_TYPE_DEST_SHIFT  11
#define REG_SRC_SHIFT    13
#define REG_DEST_SHIFT    8


/**
 * Performs the second pass over the assembly source file.
 *
 * This pass:
 * - Resolves symbolic addresses (labels) in instruction operands.
 * - Completes instruction encoding using the symbol table and externals table.
 * - Handles .extern symbols and updates their usage addresses.
 * - Reports errors for undefined symbols or memory issues.
 *
 * @param prog Pointer to the translation unit (holds symbol table, memory, externals, etc.).
 * @param amFile Opened file pointer to the preprocessed .am source file.
 * @return 1 if any errors were encountered, 0 if successful.
 */
int secondPass(struct translation_unit *prog, FILE *amFile);

#endif

