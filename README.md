# Taller y Diseño de Software
This repository has a Makefile that you can use to compile our compiler. You can run this Makefile script with the command

```bash
make
```

This command will create an executable file `executable` in the root directory. This is the compiler program.

If you run this program with no arguments it receives an input where you can type your program, or you can pass as argument a file path. Example:
```bash
./executable program.c
```
This will show in the console the ast, symbol table, interpreter and the assembly code corresponding to the input program.
