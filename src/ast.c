#include "ast.h"
#include "common.h"
#include "symbols.h"

#include <stdio.h>
#include <stdlib.h>

const char* kAstTypeStr[] = {
  "AST_ID",
  "AST_OP",
  "AST_KYWD",
  "AST_LIT",

  "AST_EQ",
  "AST_SEMI",

  "AST_ASTERISK",
  "AST_PLUS",
  "AST_HYPH",

  "AST_CHAR",
  "AST_INT",
  "AST_LONG",

  "AST_RET",

  "AST_LV_ID",
  "AST_GLUE",

  "AST_EOF",
  "AST_NULL",
};

void astPrint(struct Ast *root) {
  struct Ast *ast = root;

  if (ast != NULL) {
    astPrint(ast->l_ast);

    if (ast->type == AST_ID || ast->type == AST_LV_ID) {
      printf("[AST][%s]:[%s]\n", kAstTypeStr[ast->type], symbolRead(ast->val.symbol_idx));
    }
    else if (ast->type == AST_LIT) {
      printf("[AST][%s]:[%i]\n", kAstTypeStr[ast->type], ast->val.val);
    } else {
      printf("[AST][%s]:[%s]\n", kAstTypeStr[ast->type], kAstTypeStr[ast->type]);
    }

    astPrint(ast->r_ast);
  }
}

struct Ast *astCreate(struct Ast *l_ast, struct Ast *r_ast, enum kAstType type, 
                      int val) {
  struct Ast *ast = malloc(sizeof *ast);
  if (ast == NULL) {
    ERROR("Error allocating memory for Ast");
  }

  ast->l_ast = l_ast;
  ast->r_ast = r_ast;
  ast->type = type;
  ast->val.val = val;

  return ast;
}

struct Ast *astCreateBranch(enum kAstType type, int val) {
  return astCreate(NULL, NULL, type, val);
}

//
// astOperand()
// @tok - Token representing the operand in an expression.
//
// Returns an Ast Branch of @tok.
//
static struct Ast* astOperand(struct Token **tok) {
  struct Ast* ast;
  int symbol_idx;

  switch ((*tok)->type) {
    case TOK_ID:
      symbol_idx = symbolFind((*tok)->data);
      if (symbol_idx == -1) {
        ERROR("Use of undeclared variable '%s'", (*tok)->data);
      }
      ast = astCreateBranch(AST_ID, symbol_idx);
      break;
    case TOK_LIT:
      ast = astCreateBranch(AST_LIT, atoi((*tok)->data));
      break;
    default:
      ERROR("Unexpected Symbol '%i'", (*tok)->type);
  }
  *tok = (*tok)->next;

  return ast;
}

//
// kOpPrec[]
//
// Array defining the precedence for each operator to be used in an expression.
//
static int kOpPrec[] = {
  0, 0, 0, 0,
  0, 0,
  2, 2, 1, 1, // M, D, A, S
};

//
// astOpPrec()
// @op - Token type to get operator precedence value from.
//
// Returns the operator precedence of @op.
//
static int astOpPrec(enum kTokenType op) {
  int prec = kOpPrec[op];

  if (prec == 0) {
    ERROR("Garbage token in astBinExpr() '%i'", op);
  }

  return prec;
}

// This function is a bit complex. Work through it slowly and carefully!
struct Ast *astBinExpr(struct Token **tok, int op_prec) {
    struct Ast *l_ast, *r_ast;

    // Get operand for new Ast.
    l_ast = astOperand(tok);

    enum kTokenType op = (*tok)->type;
    if (op == TOK_SEMI) {
      return l_ast;
    }

    // Ensure we are dealing with most important operators first
    // (PEMDAS/BEMDAS).
    while (astOpPrec(op) > op_prec) {
      *tok = (*tok)->next;

      // Create Ast with proper operator.
      r_ast = astBinExpr(tok, kOpPrec[op]);
      l_ast = astCreate(l_ast, r_ast, (enum kAstType)op, 0);

      op = (*tok)->type;

      if (op == TOK_SEMI) {
        return l_ast;
      }
    }
    return l_ast;
}

void astFree(struct Ast* root) {
  struct Ast *ast = root;

  if (ast != NULL && ast->l_ast != NULL) {
    astFree(ast->l_ast);

    free(ast->l_ast);
    
    if (ast != NULL && ast->r_ast != NULL) {
      astFree(ast->r_ast);
    }
    free(ast->r_ast);
  }
}

