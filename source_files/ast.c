#include <string.h>
#include "../header_files/ast.h"
#include "../header_files/text_parser.h"



struct ast line_ast(char * line)
{
       struct ast ast = {EMPTY};
       /* Separate the input line into words */
       struct string_seperation_result result = seperate_string(line);
       char * commend = result.strings[0];
       int contains_label = FALSE;
       struct instruction *check_inst;
       int check_dir;
       char label[MAX_LABEL_LEN];
       
       /* Handle empty or comment lines */
       if(result.strings_count == 0)
              ast.ast_type = empty;
       else if(result.strings[0][0] == ';')
              ast.ast_type = comment;
       else 
       {
              /* Check if the first word is a legal label */
              if(legal_label_def(result.strings[0], label))
              {
                     /* Store label and update command pointer */
                     strcpy(ast.label_name, label);
                     commend = result.strings[1];
                     contains_label = TRUE;
              }
              else if(result.strings[0][strlen(result.strings[0]) - 1] == ':')
              {
                     append_error(&ast.error, "illigal label");
                     commend = result.strings[1];
                     contains_label = TRUE;
              }

              /* Check if command is a directive */
              check_dir = check_directive(commend);
              if(check_dir != NOT_A_DIRECTIVE)
              {
                     ast.ast_type = directive;
                     ast.ast_options.ast_directive.directive_type = check_dir;

                     /* Parse directive operands */
                     parse_directive_operands(&result.strings[contains_label + 1], result.strings_count - contains_label - 1, check_dir, &ast);
              }
              else 
              {
                     /* Check if command is a valid instruction */
                     check_inst = check_instruction(commend);
                     if(check_inst)
                     {
                            ast.ast_type = instruction;
                            ast.ast_options.ast_instruction.opCode = check_inst->opCode;
                            ast.ast_options.ast_instruction.funct = check_inst->funct;

                            /* Parse instruction operands */
                            parse_instruction_operands(&result.strings[contains_label + 1], result.strings_count - contains_label - 1, check_inst, &ast);
                     }
                     else
                     {
                            /* Unrecognized command */
                            append_error(&ast.error, "illigal commend");
                     }
              }
       }
       return ast;
}

