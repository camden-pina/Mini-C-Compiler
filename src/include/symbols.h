#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#define MAX_SYMBOLS 1024

//
// struct symtable
// @name - Symbol name.
// @sz - Symbol size.
//
struct Symbols {
    char *name;
    int sz;
    int stk_off;
};

//
// symbolRead()
// @symbol_idx - Index into symbol table.
//
// Returns the name of the symbol
char *symbolRead(const int symbol_idx);

//
// symbolFind()
// @name - Name of the symbol to look for.
//
// Checks if a symbol called @name is defined.
// Returns -1 if no symbol is found.
//
int symbolFind(const char *name);

//
// symbolAdd()
// @name - Name of new symbol.
// @sz - Size of new symbol.
//
// Returns the index of a new symbol added to the symbols table.
//
int symbolAdd(const char *name, int sz);

//
// symbolsFree()
//
// Frees all symbols allocated by the compiler.
//
void symbolsFree(void);

#endif // _SYMBOL_H_
