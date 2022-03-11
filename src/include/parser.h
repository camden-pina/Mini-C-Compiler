#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast.h"
#include "token.h"

//
// parserParser()
// head - Head of list of tokens.
//
// Returns Ast version of @head.
//
struct Ast *parserParse(struct Token **head);

#endif // _PARSER_H_
