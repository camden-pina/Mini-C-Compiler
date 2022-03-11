#ifndef _AST_H_
#define _AST_H_

#include "token.h"

enum kAstType {
  AST_ID,
  AST_OP,
  AST_KYWD,
  AST_LIT,

  AST_EQ,
  AST_SEMI,

  AST_ASTERISK,
  AST_PLUS,
  AST_HYPH,

  AST_CHAR,
  AST_INT,
  AST_LONG,

  AST_RET,

  AST_LV_ID,
  AST_GLUE,

  AST_EOF,
  AST_NULL,
};

// struct Ast
// @l_ast - Left Ast.
// @r_ast - Right Ast.
//
// @type - Type for this Ast.
//
// @val (union)
// @val - Integer value for AST_LIT.
// @symbol_idx - Integer index for AST_ID.
//
struct Ast {
  struct Ast *l_ast;
  struct Ast *r_ast;

  enum kAstType type;

  union {
    int val;
    int symbol_idx;
  } val;
};

//
// astPrint()
// @root - Root of Ast to print.
//
// Prints all branches in @head.
//
void astPrint(struct Ast *root);

//
// astCreate()
// @l_ast - Left branch of Ast.
// @r_ast - Right branch of Ast.
// @type - Type of Ast.
// @val - Value to store in Ast.
//
// Creates a new Ast defined by the given parameters.
//
struct Ast *astCreate(struct Ast *l_ast, struct Ast *r_ast, enum kAstType type, 
                      int val);
 
//
// astCreateBranch()
// @type - Type of Ast.
// @val - Value to store in Ast.
//
// Create a new Ast Branch which can later be attatched to another Ast Branch
// to create a full Ast.
//
struct Ast *astCreateBranch(enum kAstType type, int val);

//
// astBinExpr()
// @head - Head of linked list containing expression.
// @op_prec - Used internaly. SET TO 0.
//
// Returns the recursively created Ast representing the expression in @head.
//
struct Ast *astBinExpr(struct Token **tok, int op_prec);

//
// astFree()
// @root - Root of Ast to be freed.
//
// Frees Ast from memory.
//
void astFree(struct Ast* root);

#endif // _AST_H_

