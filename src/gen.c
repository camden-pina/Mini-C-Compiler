#include "gen.h"
#include "common.h"
#include "symbols.h"
#include "reg.h"
#include "stack.h"
#include "instr.h"

#include <stdio.h>
#include <stdlib.h>

// https://i.stack.imgur.com/j8hpC.png

extern struct Symbols kSymbolsTable[MAX_SYMBOLS];

static FILE *kOutFile;

//
// genSetReturn()
// @reg - Reigster storing calculated return value.
//
// Sets '%rax' register to @reg.
//
static int genSetReturn(int reg) {
  // Used for register equalization.
  int rax = 4;
  regEqualize(&reg, &rax);

  fprintf(kOutFile, "\tmovq %s, %%rax\n", regGetID(reg));
  return 0;
}

//
// Arithmetic
//

static int genMul(int reg1, int reg2) {
  regEqualize(&reg1, &reg2);

  fprintf(kOutFile, "\timul %s, %s\n", regGetID(reg2), regGetID(reg1));

  regUnRsv(reg2);
  return reg1;
}

static int genAdd(int reg1, int reg2) {
  regEqualize(&reg1, &reg2);
   
  fprintf(kOutFile, "\tadd%c\t%s, %s\n", instrGetMatchingSuffix(reg2 + 1),
          regGetID(reg2), regGetID(reg1));
  
  regUnRsv(reg2);
  return reg1;
}

static int genSub(int reg1, int reg2) {
  regEqualize(&reg1, &reg2);
 
  fprintf(kOutFile, "\tsub%c\t%s, %s\n", instrGetMatchingSuffix(reg2 + 1),
          regGetID(reg2), regGetID(reg1));

  regUnRsv(reg2);
  return reg1;
}

//
// Symbols
//

//
// genLoadLit()
// @val - Value to put on stack.
//
// Loads @val onto reserved space on the stack.
//
static int genLoadLit(int val) {
  // Set from func call 'genStoreSymbol()'
  // Set to the size of identifier storing @val.
  extern int kStackNewAllocSz;

  fprintf(kOutFile, "\tmov%c\t$%i, -%i(%%rbp)\n", instrGetMatchingSuffix(kStackNewAllocSz), val, stackGetSize());
  return 0;
}

//
// genLoadSymbol()
// @id - Identifer retrieve from stack.
//
// Retrieves the value @id from the stack.
//
static int genLoadSymbol(char *id) {
  int stk_alloc = 0;

  int sym_idx = symbolFind(id);
  for (int i = 0; i <= sym_idx; i++) {
    stk_alloc += kSymbolsTable[i].sz;
  }

  int sym_sz = kSymbolsTable[sym_idx].sz;
  int reg = regRsv(sym_sz);
  fprintf(kOutFile, "\tmov%c\t-%i(%%rbp), %s\n", instrGetMatchingSuffix(sym_sz), stk_alloc, regGetID(reg));

  return reg;
}

//
// genStoreSymbol()
// @id - Identifier to reserve on the stack.
//
// Reserves an amount of bytes equal to the size of the @id.
//
static int genStoreSymbol(char *id) {
  stackRsv(id);
  return 0;
}

//
// genGenerateCode()
// @ast - Ast to generate code for.
//
// Converts @ast to x86_64 code. Results after each operation are immediately
// stored into @kOutFile.
//
static int genGenerateCode(struct Ast *ast) {
  int l_reg = 0, r_reg = 0;

  if (ast->l_ast) {
    l_reg = genGenerateCode(ast->l_ast);
  }

  if (ast->r_ast) {
    r_reg = genGenerateCode(ast->r_ast);
  }

  switch (ast->type) {
    case AST_LIT:
      return genLoadLit(ast->val.val);
    case AST_ID:
      return genLoadSymbol(kSymbolsTable[ast->val.symbol_idx].name);
    case AST_LV_ID:
      return genStoreSymbol(kSymbolsTable[ast->val.symbol_idx].name);
    case AST_EQ:
      return r_reg;
    case AST_ASTERISK:
      return genMul(l_reg, r_reg);
   case AST_PLUS:
      return genAdd(l_reg, r_reg);
    case AST_HYPH:
      return genSub(l_reg, r_reg);
    case AST_RET:
      return genSetReturn(r_reg);
    case AST_GLUE:
      return 0;
    default:
      ERROR("Unknown AST Operator '%i'", ast->type);
  }
  return 0;
}

static const char *kPreamble = 
  ".global main\n\n"
  ".align 2\n\n"
  "main:\n"
  "\tpushq %rbp\n"
  "\tmovq %rsp, %rbp\n";

static const char *kPostamble =
  "\tpopq %rbp\n"
  "\tret\n";

void genGenerate(struct Ast *root, const char *filename) {
  kOutFile = fopen(filename, "w");

  if (kOutFile == NULL) {
    ERROR("Can't open file for R/W '%s'", filename);
  }

  // Generate assembly code

  {
    fprintf(kOutFile, "%s", kPreamble);

    if (root != NULL) {
     genGenerateCode(root);
    }

    fprintf(kOutFile, "%s", kPostamble);
  }

  fclose(kOutFile);
}
