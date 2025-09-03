#include <stdio.h>
#include "ast.h"
#include "symtab.h"

void codegen(AST *n, SymTab *st) {
    switch (n->type) {
        case NODE_INT:
            printf("    mov eax, %d\n", n->info->ival);
            break;

        case NODE_BOOL:
            printf("    mov eax, %d  ; bool\n", n->info->bval ? 1 : 0);
            break;

        case NODE_ID:
            printf("    mov eax, [%s]\n", n->info->name);
            break;

        case NODE_BINOP:
            codegen(n->left, st);
            printf("    push eax\n");
            codegen(n->right, st);
            printf("    mov ebx, eax\n");
            printf("    pop eax\n");
            switch (n->info->op) {
                case '+': printf("    add eax, ebx\n"); break;
                case '-': printf("    sub eax, ebx\n"); break;
                case '*': printf("    imul eax, ebx\n"); break;
                case '|': printf("    or eax, ebx\n"); break;
                case '&': printf("    and eax, ebx\n"); break;
            }
            break;

        case NODE_UNOP:
            codegen(n->left, st);
            if (n->info->op == '!') {
                printf("    cmp eax, 0\n");
                printf("    sete al\n");
                printf("    movzx eax, al\n");
            }
            break;

        case NODE_ASSIGN:
            codegen(n->right, st);
            printf("    mov [%s], eax\n", n->left->info->name);
            break;

        case NODE_DECL:
            if (n->info->name) {
                printf("    ; declarar %s\n", n->info->name);
                if (n->right) {
                    codegen(n->right, st);
                    printf("    mov [%s], eax\n", n->info->name);
                }
            }
            break;

        case NODE_RETURN:
            codegen(n->left, st);
            printf("    ret\n");
            break;

        case NODE_FUNCTION:
            printf("%s:\n", n->info->name);
            printf("    push ebp\n");
            printf("    mov ebp, esp\n");
            codegen(n->left, st);
            printf("    pop ebp\n");
            printf("    ret\n");
            break;

        default:
            break;
    }

    if (n->next) codegen(n->next, st);
}
