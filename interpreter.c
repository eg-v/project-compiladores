#include <stdio.h>
#include "ast.h"
#include "symtab.h"

#include <stdio.h>
#include "ast.h"
#include "symtab.h"

int interpreter(AST* n, SymTab *st) {
    if (!n) return 0;
    switch (n->type) {
        case NODE_INT:
            return n->info->ival;
            break;

        case NODE_BOOL:
            return n->info->bval;
            break;

        case NODE_ID: {
            int found;
            int value = symtab_get_value(st, n->info->name, &found);
            if (found) {
                return value;
            } else {
                fprintf(stderr, "Error: undeclared variable '%s'\n", n->info->name);
            }
            break;
        }


        case NODE_BINOP: {
            int lt = interpreter(n->left, st);
            int rt = interpreter(n->right, st);
            if (n->info->op=='+'||n->info->op=='-'||n->info->op=='*') {
                switch(n->info->op) {
                    case '+': {
                                  int op_result = lt + rt;
                                  n->info->ival = op_result;
                                  printf("+ RESULT: %d\n", op_result);
                                  return op_result;
                              }
                    case '-': {
                                  int op_result = lt - rt;
                                  n->info->ival = op_result;
                                  printf("+ RESULT: %d\n", op_result);
                                  return op_result;
                              }
                    case '*': {
                                  int op_result = lt * rt;
                                  n->info->ival = op_result;
                                  printf("+ RESULT: %d\n", op_result);
                                  return op_result;
                              }
                }
        }
        if (n->info->op=='|'||n->info->op=='&') {
            switch(n->info->op) {
                case '|': {
                              int op_result = lt || rt;
                              n->info->bval = op_result;
                              return op_result;
                              break;
                          }
                case '&': {
                              int op_result = lt && rt;
                              n->info->bval = op_result;
                              return op_result;
                              break;
                          }
            }
        }
        break;
     }

        case NODE_UNOP: {
            if (!n->left) {
                fprintf(stderr, "Error: UNOP without operand\n");
                return 0;
            }
            int val = interpreter(n->left, st);
            int op_result = 0;
            if (n->info->op == '!') {
                op_result = !val;
                n->info->bval = op_result;
            } else {
                fprintf(stderr, "Unknown unary operator '%c'\n", n->info->op);
            }
            return op_result;
        }

        case NODE_ASSIGN: {
            int rhs = interpreter(n->right, st);
            if (n->left && n->left->type == NODE_ID) {
                symtab_set_value(st, n->left->info->name, rhs);
            }
            return rhs;
            break;
        }

          case NODE_DECL: {
            if (n->info->name) {
                if (symtab_scope(st, n->info->name) == TYPE_ERROR) {
                    symtab_insert(st, n->info);
                }
                if (n->right) {
                    int rhs = interpreter(n->right, st);
                    symtab_set_value(st, n->info->name, rhs);
                }
            }
            break;
          }

            case NODE_RETURN: {
                int result;
                if (n->left) {
                    result = interpreter(n->left, st);
                } else {
                    result = 0;
                }
                return result;
                break;
            }

          case NODE_FUNCTION: {
              return interpreter(n->left, st);
              break;
            }
        default:
      break;
    }

    if (n->next) interpreter(n->next, st);
    return 0;
}
