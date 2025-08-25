#include <stdio.h>
#include "ast.h"

extern int yyparse(void);
extern AST *root;

int main() {
    if (yyparse() == 0) {
        printf("\nÁrbol sintáctico con tipos:\n");
        print_ast(root, 0, 1);
    }
    return 0;
}

