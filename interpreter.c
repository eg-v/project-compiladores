#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symtab.h"

#define IS_INTERPRETER 1

void interpreter(AST* n, SymTab *st) {
    switch (n->type) {
        case NODE_INT:
            // return n->info->ival;
            break;

        case NODE_BOOL:
            // return n->info->bval;
            break;

        case NODE_ID: {
            int found;
            int value = symtab_get_value(st, n->info->name, &found);
            if (found) {
                if (n->info->eval_type == TYPE_INT) {
                    n->info->ival = value;
                } else if (n->info->eval_type == TYPE_BOOL) {
                    n->info->bval = value;
                }
            } else {
                fprintf(stderr, "Error: variable '%s' used before initialization\n", n->info->name);
                exit(1);
            }
            TypeInfo t = symtab_lookup(st, n->info->name);
            n->info->eval_type = t;
            break;
        }


        case NODE_BINOP: {
            interpreter(n->left, st);
            interpreter(n->right, st);
            if (n->info->op=='+'||n->info->op=='-'||n->info->op=='*') {
                int lt = n->left->info->ival;
                int rt = n->right->info->ival;
                switch(n->info->op) {
                    case '+': {
                                  int op_result = lt + rt;
                                  n->info->ival = op_result;
                                  // return op_result;
                                  break;
                              }
                    case '-': {
                                  int op_result = lt - rt;
                                  n->info->ival = op_result;
                                  // return op_result;
                                  break;
                              }
                    case '*': {
                                  int op_result = lt * rt;
                                  n->info->ival = op_result;
                                  // return op_result;
                                  break;
                              }
                }
        }
        if (n->info->op=='|'||n->info->op=='&') {
            int lt = n->left->info->bval;
            int rt = n->right->info->bval;
            switch(n->info->op) {
                case '|': {
                              int op_result = lt || rt;
                              n->info->bval = op_result;
                              // return op_result;
                              break;
                          }
                case '&': {
                              int op_result = lt && rt;
                              n->info->bval = op_result;
                              // return op_result;
                              break;
                          }
            }
        }
        break;
     }

        case NODE_UNOP: {
            interpreter(n->left, st);
            int et = n->left->info->bval;
            int op_result;
            if (n->info->op=='!') {
                op_result = !et;
                n->info->bval = op_result;
                // n->info->eval_type = TYPE_BOOL;
            }
            // return op_result;
            break;
        }

        case NODE_ASSIGN: {
            interpreter(n->right, st);
            if (n->left && n->left->type == NODE_ID) {
                if (n->left->info->eval_type == TYPE_INT) {
                    int rhs = n->right->info->ival;
                    n->left->info->ival = rhs;
                    n->info->ival = rhs;
                    symtab_set_value(st, n->left->info->name, n->left->info->ival, IS_INTERPRETER);
                } else if (n->left->info->eval_type == TYPE_BOOL) {
                    int rhs = n->right->info->bval;
                    n->left->info->bval = rhs;
                    n->info->bval = rhs;
                    symtab_set_value(st, n->left->info->name, n->left->info->bval, IS_INTERPRETER);
                }
            }
            // return rhs;
            break;
        }

          case NODE_DECL: {
            if (n->info->name) {
                symtab_insert(st, n->info);
                if (n->right) {
                    interpreter(n->right, st);
                    if (n->left && n->left->type == NODE_ID) {
                        if (n->left->info->eval_type == TYPE_INT) {
                            int rhs = n->right->info->ival;
                            n->left->info->ival = rhs;
                            n->info->ival = rhs;
                            symtab_set_value(st, n->left->info->name, n->left->info->ival, IS_INTERPRETER);
                        } else if (n->left->info->eval_type == TYPE_BOOL) {
                            int rhs = n->right->info->bval;
                            n->left->info->bval = rhs;
                            n->info->bval = rhs;
                            symtab_set_value(st, n->left->info->name, n->left->info->bval, IS_INTERPRETER);
                        }
                    }
                    // return rhs;
                }
            }
            break;
          }

            case NODE_RETURN: {
              interpreter(n->left, st);
              int lhs = n->left->info->ival;
              if (n->left->info->eval_type == TYPE_BOOL) {
                  char* boolean_value = (n->left->info->bval ? "true" : "false");
                  printf("return: %s\n", boolean_value);
              } else {
                printf("return: %d\n", lhs);
              }
              return;
              break;
            }

          case NODE_FUNCTION: {
                interpreter(n->left, st);
                // interpreter(n->right, st);
              break;
            }
        default:
      break;
    }

    if (n->next) interpreter(n->next, st);
}
