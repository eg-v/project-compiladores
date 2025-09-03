#include <stdio.h>
#include "ast.h"
#include "symtab.h"

TypeInfo type_main;

TypeInfo check_types(AST* n, SymTab *st) {

    if (!n) return TYPE_UNKNOWN;

    TypeInfo result = TYPE_UNKNOWN;

    switch (n->type) {
        case NODE_INT:
            n->info->eval_type = TYPE_INT;
            result = TYPE_INT;
            break;

        case NODE_BOOL:
            n->info->eval_type = TYPE_BOOL;
            result = TYPE_BOOL;
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
                    // printf("FOUND\n");
                    if (n->info->eval_type == TYPE_INT) {
                        n->info->ival = value;
                    } else if (n->info->eval_type == TYPE_BOOL) {
                        n->info->bval = value;
                    }
                }// else {
                    // printf("NOT FOUND\n");
                // }
            }
            n->info->eval_type = t;
            result = t;
            break;
        }


        case NODE_BINOP: {
            TypeInfo lt = check_types(n->left, st);
            TypeInfo rt = check_types(n->right, st);
            if (n->info->op=='+'||n->info->op=='-'||n->info->op=='*') {
                if (lt!=TYPE_INT || rt!=TYPE_INT)
                    fprintf(stderr,"Type error: arithmetic needs int\n");
                n->info->eval_type = TYPE_INT;
                result = TYPE_INT;
            } else {
                if (lt!=TYPE_BOOL || rt!=TYPE_BOOL)
                    fprintf(stderr,"Type error: logical needs bool\n");
                n->info->eval_type = TYPE_BOOL;
                result = TYPE_BOOL;
            }
            break;
        }

        case NODE_UNOP: {
            TypeInfo et = check_types(n->left, st);
            if (n->info->op=='!' && et!=TYPE_BOOL)
                fprintf(stderr,"Type error: ! needs bool\n");
            n->info->eval_type = (n->info->op=='!') ? TYPE_BOOL : et;
            result = TYPE_BOOL;
            break;
        }

        case NODE_ASSIGN: {
            TypeInfo lhs = check_types(n->left, st);
            TypeInfo rhs = check_types(n->right, st);
            if (lhs == TYPE_ERROR && n->left->type == NODE_ID) {
                lhs = symtab_lookup(st, n->left->info->name);
            }
            if (lhs != rhs)
                fprintf(stderr,"Type error: assignment mismatch\n");

            // Set the value of the variable if right side is evaluable
            if (n->left && n->left->type == NODE_ID) {
                if (n->left->info->eval_type == TYPE_INT) {
                    symtab_set_value(st, n->left->info->name, n->right->info->ival);
                } else if (n->left->info->eval_type == TYPE_BOOL) {
                    symtab_set_value(st, n->left->info->name, n->right->info->bval);
                }
            }

            n->info->eval_type = lhs;
            result = lhs;
            break;
        }

          case NODE_DECL:
            if (n->info->name) {
                TypeInfo t = symtab_scope(st, n->info->name);
                if (t == TYPE_ERROR) {
                    symtab_insert(st, n->info);
                    if (n->right) {
                        check_types(n->right, st);
                        if (n->right->type == NODE_INT) {
                            n->info->ival = n->right->info->ival;
                            symtab_set_value(st, n->info->name, n->info->ival);
                        }
                        if (n->right->type == NODE_BOOL) {
                            n->info->bval = n->right->info->bval;
                            symtab_set_value(st, n->info->name, n->info->bval);
                        }
                    }
                } else {
                    printf("%s: variable already declared.\n", n->info->name);
                }
            }
            result = TYPE_UNKNOWN;
            break;

            case NODE_RETURN: {
                TypeInfo ret_type = check_types(n->left, st);
                if (ret_type != type_main) {
                    fprintf(stderr, "Type error: return type mismatch (expected %s, got %s)\n",
                        type_to_string(type_main),
                        type_to_string(ret_type));
                }
                result = ret_type;
                break;
            }

          case NODE_FUNCTION: {
            TypeInfo declared_type = n->info->eval_type;
            type_main = declared_type;
            check_types(n->left, st);
            symtab_print(st);
            break;
        }


        default:
            result = TYPE_UNKNOWN;
    }

    if (n->next) check_types(n->next, st);

    return result;
}
