#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header_files/second_pass.h"
#include "../header_files/first_pass.h"
#include "../header_files/ast.h"
#include "../header_files/translation_unit.h"
#include "../header_files/mem_alloc.h"


int secondPass(struct translation_unit *prog, FILE *amFile) {
       char line[MAX_LINE_LEN + 1] = {0};
       int errorFlag = FALSE;
       int lineC = 1;
       struct ast line_struct = {0};
       struct symbol * SymFind;
       struct ext *extFind;
       int i;
       int instruction_address;

       /* Read each line from the .am file */
       while (fgets(line, sizeof(line), amFile)) {
              remove_newline(line); 
              line_struct = line_ast(line);

              /* Process only instruction lines */
              if (line_struct.ast_type == instruction) {

                     instruction_address = prog->IC;

                     /* Encode first word: opcode, funct, A-bit */
                     prog->code_image[prog->IC] = 0;
                     prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.funct << FUNCT_SHIFT);
                     prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.opCode << OPCODE_SHIFT);
                     prog->code_image[prog->IC] |= A;

                     /* Encode source/destination addressing and registers (2 operands) */
                     if (line_struct.ast_options.ast_instruction.number_of_operands == 2) {
                            prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.oprand[0].oprand_type << OPERAND_TYPE_SOURCE_SHIFT);
                            prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.oprand[1].oprand_type << OPERAND_TYPE_DEST_SHIFT);

                            if (line_struct.ast_options.ast_instruction.oprand[0].oprand_type == ast_register) {
                                   prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.oprand[0].oprand_options.register_number << REG_SRC_SHIFT);
                            }

                            if (line_struct.ast_options.ast_instruction.oprand[1].oprand_type == ast_register) {
                                   prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.oprand[1].oprand_options.register_number << REG_DEST_SHIFT);
                            }
                     }

                     /* Encode destination addressing/register (1 operand) */
                     if (line_struct.ast_options.ast_instruction.number_of_operands == 1) {
                            prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.oprand[0].oprand_type << OPERAND_TYPE_DEST_SHIFT);

                            if (line_struct.ast_options.ast_instruction.oprand[0].oprand_type == ast_register) {
                                   prog->code_image[prog->IC] |= (line_struct.ast_options.ast_instruction.oprand[0].oprand_options.register_number << REG_DEST_SHIFT);
                            }
                     }
                     /* Advance to next word in memory */
                     prog->IC++;

                     /* Encode additional words for operands */
                     for (i = 0; i < line_struct.ast_options.ast_instruction.number_of_operands; i++) {

                            /* Direct addressing (label) */
                            if (line_struct.ast_options.ast_instruction.oprand[i].oprand_type == ast_direct) {

                                   SymFind = symbolLookUp(prog->symbol_table, prog->symCount,
                                                          line_struct.ast_options.ast_instruction.oprand[i].oprand_options.label);

                                   if (SymFind) {
                                          prog->code_image[prog->IC] = SymFind->address << ARE_SHIFT;

                                          /* Handle extern symbol */
                                          if (SymFind->symType == symExtern) {
                                                 prog->code_image[prog->IC] |= E;
                                                 extFind = extSearch(prog->externals, prog->extCount, SymFind->symName);
                                                 if (extFind) {
                                                        extFind->addresses[extFind->address_count] = prog->IC + STARTING_ADDRESS;
                                                        extFind->address_count++;
                                                 } else {
                                                        if (!ensure_externals_capacity(prog)) {
                                                               printf("Memory error: Could not expand externals table.\n");
                                                               errorFlag = TRUE;
                                                               break;
                                                        }
                                                        prog->externals[prog->extCount].externalName = SymFind->symName;
                                                        prog->externals[prog->extCount].addresses[0] = prog->IC + STARTING_ADDRESS;
                                                        prog->externals[prog->extCount].address_count = 1;
                                                        prog->extCount++;
                                                 }
                                          } 
                                          else {
                                                 prog->code_image[prog->IC] |= R;
                                          }
                                   } else {
                                          printf("error in line %d: undefined label \"%s\"\n", lineC,
                                                 line_struct.ast_options.ast_instruction.oprand[i].oprand_options.label);
                                          errorFlag = TRUE;
                                   }

                                   prog->IC++;
                            }

                            /* Relative addressing (label - current address) */
                            else if (line_struct.ast_options.ast_instruction.oprand[i].oprand_type == ast_relative) {
                                   SymFind = symbolLookUp(prog->symbol_table, prog->symCount,
                                                          line_struct.ast_options.ast_instruction.oprand[i].oprand_options.label);

                                   if (SymFind) {
                                          prog->code_image[prog->IC] = (SymFind->address - (instruction_address + STARTING_ADDRESS)) << ARE_SHIFT;
                                          prog->code_image[prog->IC] |= A;

                                          if (SymFind->symType == symExtern) {
                                                 printf("error in line %d: undefined label(extern label) \"%s\"\n", lineC,
                                                        line_struct.ast_options.ast_instruction.oprand[i].oprand_options.label);
                                                 errorFlag = TRUE;
                                          }
                                   } else {
                                          printf("error in line %d: undefined label \"%s\"\n", lineC,
                                                 line_struct.ast_options.ast_instruction.oprand[i].oprand_options.label);
                                          errorFlag = TRUE;
                                   }

                                   prog->IC++;
                            }

                            /* Immediate addressing (#number) */
                            else if (line_struct.ast_options.ast_instruction.oprand[i].oprand_type == ast_instant) {
                                   prog->code_image[prog->IC] = (line_struct.ast_options.ast_instruction.oprand[i].oprand_options.number << ARE_SHIFT);
                                   prog->code_image[prog->IC] |= A;
                                   prog->IC++;
                            }
                     }
              }

              lineC++;
       }

       return errorFlag;
}

