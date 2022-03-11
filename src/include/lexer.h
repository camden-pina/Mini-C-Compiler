#ifndef _LEXER_H_
#define _LEXER_H_

#include "token.h"
#include <stdio.h>

//
// lexerPrint()
// @head
//
// Prints the sequential data values of @head.
//
void lexerPrint(struct Token *head);

//
// lexerTokenize()
// @stream - Char sequence to be tokenized.
//
// Returns the tokenized version of @stream complying to the programming
// language standard (See BNF.md).
//
struct Token *lexerTokenize(const char *stream);

//
// lexerInit()
//
// Initializes internal data structures (hashtable) to be used by lexer.
//
void lexerInit(void);

//
// lexerFree()
// @head - Head of linked list of tokens.
//
// Frees tokens resources allocated by lexer (hashtable, @head).
//
void lexerFree(struct Token *head);

#endif // _LEXER_H_
