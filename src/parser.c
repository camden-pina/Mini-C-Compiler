#include "parser.h"
#include "common.h"
#include "symbols.h"

#include <stdio.h>
#include <stdlib.h>

//
// parserMatch()
// @tok - Token to be compared.
// @type - Type to be compared against.
//
// Ensures the type of @tok against @type are the same, then sets @tok to the
// next element in the linked list.
//
static void parserMatch(struct Token **tok, enum kTokenType type) {
  extern const char *kAstTypeStr[];

  if ((*tok)->type != type) {
    ERROR("Expected '%s', got '%s'", kAstTypeStr[type], kAstTypeStr[(*tok)->type]);
  }
  *tok = (*tok)->next;
}

static struct Ast *parserParseDefinition(struct Token **head);

//
// parserParseDeclaration()
// @head - Head of the list of tokens.
//
// Returns an Ast of the parsed 'declaration' expression found in @head.
//
static struct Ast *parserParseDeclaration(struct Token **head) {
  struct Ast *ast = NULL;

  switch ((*head)->type) {
    case TOK_INT:
      *head = (*head)->next;
      symbolAdd((*head)->data, 4);

      if ((*head)->next->type == TOK_EQ) {
        ast = parserParseDefinition(head);
       *head = (*head)->prev;
       *head = (*head)->prev;
      }
      break;
    case TOK_LONG:
      *head = (*head)->next;
      symbolAdd((*head)->data, 8);
      break;
    default:
      ERROR("Unknown type: '%s'", (*head)->data);
  }

  *head = (*head)->next;
  parserMatch(head, TOK_SEMI);

  return ast;
}

//
// parserParseDefinition()
// @head - Head of the list of tokens.
//
// Returns an Ast of the parsed 'definition' expression found in @head.
//
static struct Ast *parserParseDefinition(struct Token **head) {
  struct Ast *l_ast, *r_ast;

  int symbol_idx;

  if ((symbol_idx = symbolFind((*head)->data)) == -1) {
    ERROR("Use of undeclared variable '%s'", (*head)->data);
  }

  *head = (*head)->next;

  parserMatch(head, TOK_EQ);

  l_ast = astCreateBranch(AST_LV_ID, symbol_idx);

  r_ast = astBinExpr(head, 0);

  parserMatch(head, TOK_SEMI);

  return astCreate(l_ast, r_ast, (enum kAstType)TOK_EQ, 0);
}

//
// parserParseReturn()
// @head - Head of the list of tokens.
//
// Returns an Ast of the parsed 'return' expression found in @head.
//
static struct Ast *parserParseReturn(struct Token **head) {
  parserMatch(head, TOK_RET);

  struct Ast *l_ast;

  l_ast = astBinExpr(head, 0);

  struct Ast* r_ast;
  r_ast = astCreateBranch(AST_GLUE, 0);
  l_ast = astCreate(r_ast, l_ast, AST_RET, 0);
  parserMatch(head, TOK_SEMI);

  return l_ast;
}

//
// parserParseExpr()
// @head - Head of the list of tokens.
//
// Returns an Ast of the parsed expression found in @head.
//
static struct Ast *parserParseExpr(struct Token **head) {

  switch ((*head)->type) {
    case TOK_ID:
      return parserParseDefinition(head);
    case TOK_INT:
    case TOK_LONG:
      return parserParseDeclaration(head);
    case TOK_RET:
      return parserParseReturn(head);
    default:
      ERROR("Unexpected token: '%i'", (*head)->type);
  }
  return NULL;
}

struct Ast *parserParse(struct Token **head) {
  struct Ast *l_ast = NULL;
  struct Ast *r_ast = NULL;

  while ((*head)->type != TOK_EOF) {
    r_ast = parserParseExpr(head);

    if (r_ast == NULL) {
      continue;
    }

    if (l_ast == NULL) {
      l_ast = r_ast;
    } else {
      l_ast = astCreate(l_ast, r_ast, AST_GLUE, 0);
    }
  }

  return l_ast;
}

