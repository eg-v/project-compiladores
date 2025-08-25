#include <stdio.h>
#include "ast.h"
#include "symtab.h"

TypeInfo check_types(AST* n, SymTab *st) {

    if (!n) return TYPE_UNKNOWN;

    TypeInfo result = TYPE_UNKNOWN;

    switch (n->type) {
        case NODE_INT:
            n->eval_type = TYPE_INT;
            result = TYPE_INT;
            break;

        case NODE_BOOL:
            n->eval_type = TYPE_BOOL;
            result = TYPE_BOOL;
            break;

        case NODE_ID: {
            TypeInfo t = symtab_lookup(st, n->name);
            if (t == TYPE_ERROR) {
                fprintf(stderr, "Error: undeclared variable '%s'\n", n->name);
                t = TYPE_UNKNOWN;
            }
            n->eval_type = t;
            result = t;
            break;
        }

        case NODE_BINOP: {
            TypeInfo lt = check_types(n->left, st);
            TypeInfo rt = check_types(n->right, st);
            if (n->op=='+'||n->op=='-'||n->op=='*') {
                if (lt!=TYPE_INT || rt!=TYPE_INT)
                    fprintf(stderr,"Type error: arithmetic needs int\n");
                n->eval_type = TYPE_INT;
                result = TYPE_INT;
            } else {
                if (lt!=TYPE_BOOL || rt!=TYPE_BOOL)
                    fprintf(stderr,"Type error: logical needs bool\n");
                n->eval_type = TYPE_BOOL;
                result = TYPE_BOOL;
            }
            break;
        }

        case NODE_UNOP: {
            TypeInfo et = check_types(n->left, st);
            if (n->op=='!' && et!=TYPE_BOOL)
                fprintf(stderr,"Type error: ! needs bool\n");
            n->eval_type = (n->op=='!') ? TYPE_BOOL : et;
            result = TYPE_BOOL;
            break;
        }

        case NODE_ASSIGN: {
            TypeInfo lhs = check_types(n->left, st);
            TypeInfo rhs = check_types(n->right, st);
            if (lhs != rhs)
                fprintf(stderr,"Type error: assignment mismatch\n");
            n->eval_type = lhs;
            result = lhs;
            break;
        }

        case NODE_DECL:
              if (n->name) {
                  // insert variable into symbol table
                  symtab_insert(st, n->name, n->eval_type); // for now assume int, or store type if you parsed it
              }
              if (n->right) check_types(n->right, st); // check initializer
              result = TYPE_UNKNOWN;
              break;

        case NODE_RETURN:
            result = check_types(n->left, st);
            break;

        case NODE_BLOCK:
            check_types(n->left, st);  // statements inside block
            break;

        case NODE_FUNCTION:
            check_types(n->left, st);  // function body (block)
            break;

        default:
            result = TYPE_UNKNOWN;
    }

    // ✅ important: keep walking through program
    if (n->extra) check_types(n->extra, st);
    if (n->next) check_types(n->next, st);

    return result;
}
