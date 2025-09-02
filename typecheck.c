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
            if (lhs != rhs)
                fprintf(stderr,"Type error: assignment mismatch\n");

            n->info->eval_type = lhs;
            result = lhs;
            break;
        }

          case NODE_DECL:
            if (n->info->name) {
                if (symtab_scope(st, n->info->name) == TYPE_ERROR) {
                    symtab_insert(st, n->info);
                    if (n->right) {
                        TypeInfo rhs_type = check_types(n->right, st);
                        if (n->info->eval_type != rhs_type) {
                             fprintf(stderr,"Type error: declaration mismatch for '%s'\n", n->info->name);
                        }
                    }
                } else {
                    fprintf(stderr, "%s: variable already declared.\n", n->info->name);
                }
            }
            result = TYPE_UNKNOWN;
            break;

            case NODE_RETURN: {
                TypeInfo ret_type = TYPE_UNKNOWN;
                if (n->left) {
                    ret_type = check_types(n->left, st);
                } else {
                    if (type_main == TYPE_VOID)
                        ret_type = TYPE_VOID;
                    else
                        ret_type = TYPE_UNKNOWN;
                }

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
            if (declared_type == TYPE_UNKNOWN) {
                type_main = TYPE_VOID;
            } else {
                type_main = declared_type;
            }
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
