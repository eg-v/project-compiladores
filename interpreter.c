#include <stdio.h>
#include "ast.h"
#include "symtab.h"

int interpreter(AST* n, SymTab *st) {
    switch (n->type) {
        case NODE_INT:
            return n->info->ival;
            break;

        case NODE_BOOL:
            return n->info->bval;
            break;

        case NODE_ID: {
            TypeInfo t = symtab_lookup(st, n->info->name);
            if (t == TYPE_ERROR) {
                fprintf(stderr, "Error: undeclared variable '%s'\n", n->info->name);
                t = TYPE_UNKNOWN;
            } else {
                int found;
                int value = symtab_get_value(st, n->info->name, &found);
                if (found) {
                    if (n->info->eval_type == TYPE_INT) {
                        return n->info->ival;
                    } else if (n->info->eval_type == TYPE_BOOL) {
                        return n->info->bval;
                    }
                }
            }
            n->info->eval_type = t;
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
                                  break;
                              }
                    case '-': {
                                  int op_result = lt - rt;
                                  n->info->ival = op_result;
                                  return op_result;
                                  break;
                              }
                    case '*': {
                                  int op_result = lt * rt;
                                  n->info->ival = op_result;
                                  return op_result;
                                  break;
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
            TypeInfo et = interpreter(n->left, st);
            int op_result;
            if (n->info->op=='!') {
                op_result = !et;
                n->info->bval = op_result;
            }
            return op_result;
            break;
        }

        case NODE_ASSIGN: {
            int rhs = interpreter(n->right, st);
            if (n->left && n->left->type == NODE_ID) {
                if (n->left->info->eval_type == TYPE_INT) {
                    n->left->info->ival = rhs;
                    n->info->ival = rhs;
                } else if (n->left->info->eval_type == TYPE_BOOL) {
                    n->left->info->bval = rhs;
                    n->info->bval = rhs;
                }
            }
            // return rhs;
            break;
        }

          case NODE_DECL: {
            if (n->info->name) {
                TypeInfo t = symtab_scope(st, n->info->name);
                if (t == TYPE_ERROR) {
                    symtab_insert(st, n->info);
                    if (n->right) {
                        int rhs = interpreter(n->right, st);
                        if (n->left && n->left->type == NODE_ID) {
                            if (n->left->info->eval_type == TYPE_INT) {
                                n->left->info->ival = rhs;
                                n->info->ival = rhs;
                            } else if (n->left->info->eval_type == TYPE_BOOL) {
                                n->left->info->bval = rhs;
                                n->info->bval = rhs;
                            }
                        }
                        // return rhs;
                    }
                }
            }
            break;
          }

            case NODE_RETURN: {
              int lhs = interpreter(n->left, st);
              printf("return: %d\n", lhs);
              return lhs;
              break;
            }

          case NODE_FUNCTION: {
              return 10;
              break;
            }
        default:
      break;
    }

    if (n->next) interpreter(n->next, st);

    return 50;
}
