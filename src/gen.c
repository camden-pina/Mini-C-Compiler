#include "gen.h"
#include "common.h"
#include "symbols.h"
#include "reg.h"
#include "instr.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

extern struct Symbols kSymbolsTable[MAX_SYMBOLS];

static FILE *kOutFile;

// ########################################
//
// Preamble & Postamble
//
// ########################################

static const char *kPreamble = 
  ".global main\n\n"
  ".align 2\n\n"
  "main:\n"
  "\tpushq %rbp\n"
  "\tmovq %rsp, %rbp\n";

static const char *kPostamble =
  "\tpopq %rbp\n"
  "\tret\n";

void emitPreamble() {
    fprintf(kOutFile, "%s", kPreamble);
}

void emitPostamble() {
    fprintf(kOutFile, "%s", kPostamble);
}

// ########################################
//
// Return value
//
// ########################################

//
// emitReturn()
// @reg - ReigsterID storing return value.
//
// Sets '%rax' register to @reg if @reg isn't already the %rax or %eax register.
// Returns AST_NULL.
//
static int emitReturn(struct Ast *root, int reg) {
  if (root->r_ast->type == AST_LIT) {
    fprintf(kOutFile, "\tmovq $%i, %%rax\n", root->r_ast->val.val);
  } else {

  // Used for register equalization.
  if (reg != 0 && reg != 4) {
    int rax = 4;
    regEqualize(&reg, rax);

    fprintf(kOutFile, "\tmovq %s, %%rax\n", regGetID(reg));
  }
  regUnRsv(reg);
  }

  return AST_NULL;
}

// ########################################
// Memory Instructions
// ########################################

//
// emitMov()
// @suffix - suffix describing instruction size to use.
// @val1 | @val2 - f: 0 - @val1 = int lit | @val2 = register ID
//                 f: 1 - @val1 = int lit | @val2 = stack_offset.
//                 f: 2 - @val1 = stack_offset | @val2 = register ID.
//                 f: 3 - @val1 = register ID | @val2 = stack_offset.
//
// @f - f - flag that describes what the contents of @val should be.
//
// Emits x86_64 code to mov @val1 into @val2.
// Results are stored in @val2.
//
static void emitMov(char suffix, int val1, int val2, int f) {
  switch (f) {
    case 0:
      fprintf(kOutFile, "\tmov%c\t$%i, %s\n", suffix, val1, regGetID(val2));
      break;
    case 1:
      fprintf(kOutFile, "\tmov%c\t$%i, -%i(%%rbp)\n", suffix, val1, val2);
      break;
    case 2:
      fprintf(kOutFile, "\tmov%c\t-%i(%%rbp), %s\n", suffix, val1, regGetID(val2));
      break;
    case 3:
      fprintf(kOutFile, "\tmov%c\t%s, -%i(%%rbp)\n", suffix, regGetID(val1), val2);
      break;
    default:
      ERROR("Invalid arg(f)");
  }
}

//
// genEq()
// @root - Root of Ast binary operation.
// @l_reg - l_reg.
// @r_reg - r_reg.
//
// Emits code to mov @r_reg into @l_reg if @root is more than a binary equation.
// Returns AST_NULL.
//
static int genEq(struct Ast *root, int l_reg, int r_reg) {
  int stk_off;

  // Only do something if we're not doing a direct assignment.
  if (root->r_ast->type != AST_LIT) {
    char suffix = instrGetMatchingSuffix(r_reg);
    stk_off = l_reg;
    emitMov(suffix, r_reg, stk_off, 3);
    regUnRsv(r_reg);
  } else {
    int symbol_idx = root->l_ast->val.symbol_idx;
    struct Symbols *symbol = &kSymbolsTable[symbol_idx];
    char suffix = instrGetMatchingSuffix(symbol->sz - 1);

    int val = r_reg;
    stk_off = l_reg;

    emitMov(suffix, val, stk_off, 1);
  }
  return AST_NULL;
}

// ########################################
//
// Arithmetic
//
// ########################################

//
// emitMul()
// @suffix - suffix describing instruction size to use.
// @val - no_int: false - int lit.
//        no_int: true  - register id.
// @reg - register to store the product.
// @no_int - flag that describes what the contents of @val should be.
//
// Emits x86_64 code to get the product of @val and @reg.
// Results are stored in @reg.
//
static void emitMul(char suffix, int val, const char *reg, bool no_int) {
  if (no_int == true) {
    fprintf(kOutFile, "\timul%c\t%s, %s\n", suffix, regGetID(val), reg);
  } else {
    fprintf(kOutFile, "\timul%c\t$%i, %s\n", suffix, val, reg);
  }
}

//
// emitAdd()
// @suffix - suffix describing instruction size to use.
// @val - no_int: false - int lit.
//        no_int: true  - register id.
// @reg - register to store the sum.
// @no_int - flag that describes what the contents of @val should be.
//
// Emits x86_64 code to get the sum of @val and @reg.
// Results are stored in @reg.
//
static void emitAdd(char suffix, int val, const char *reg, bool no_int) {
  if (no_int == true) {
    fprintf(kOutFile, "\tadd%c\t%s, %s\n", suffix, regGetID(val), reg);
  } else {
    fprintf(kOutFile, "\tadd%c\t$%i, %s\n", suffix, val, reg);
  }
}

//
// emitSub()
// @suffix - suffix describing instruction size to use.
// @val - no_int - flag that describes what the contents of @val should be.
//        no_int: false - @val = int lit.
//        no_int: true  - @val = register id.
// @reg - register to store the difference.
//
// Emits x86_64 code to get the difference of @val and @reg.
// Results are stored in @reg.
//
static void emitSub(char suffix, int val, const char *reg, bool no_int) {
  if (no_int == true) {
    fprintf(kOutFile, "\tsub%c\t%s, %s\n", suffix, regGetID(val), reg);
  } else {
    fprintf(kOutFile, "\tsub%c\t$%i, %s\n", suffix, val, reg);
  }
}

//
// genMul()
// @root - Root of Ast binary operation.
// @l_reg - l_reg.
// @r_reg - r_reg.
//
// Returns the registerID storing the product of @l_reg and @r_reg.
//
static int genMul(struct Ast *root, int l_reg, int r_reg) {
  char suffix;

  if (root->l_ast->type == AST_LIT && root->r_ast->type == AST_LIT) {
    // We need to add a check here to ensure we are using a register large
    // enough to fit the requested value.
    int reg = regRsv(4);
    suffix = instrGetMatchingSuffix(4 - 1);
    emitMov(suffix, l_reg, reg, 0);
    emitMul(suffix, r_reg, regGetID(reg), false);
    return reg;
  } else if (root->l_ast->type == AST_LIT) {
    suffix = instrGetMatchingSuffix(r_reg);
    emitMul(suffix, l_reg, regGetID(r_reg), false);
    return r_reg;
  } else if (root->r_ast->type == AST_LIT) {
    suffix = instrGetMatchingSuffix(l_reg);
    emitMul(suffix, r_reg, regGetID(l_reg), false);
  } else {
    regEqualize(&l_reg, r_reg);
    suffix = instrGetMatchingSuffix(r_reg);
    emitMul(suffix, r_reg, regGetID(l_reg), true);
    regUnRsv(r_reg);
  }
  return l_reg;
}

//
// genAdd()
// @root - Root of Ast binary operation.
// @l_reg - l_reg.
// @r_reg - r_reg.
//
// Returns the registerID storing the sum of @l_reg and @r_reg.
//
static int genAdd(struct Ast *root, int l_reg, int r_reg) {
  char suffix;

  if (root->l_ast->type == AST_LIT && root->r_ast->type == AST_LIT) {
    int reg = regRsv(4);
    suffix = instrGetMatchingSuffix(4 - 1);
    emitMov(suffix, l_reg, reg, 0);
    emitAdd(suffix, r_reg, regGetID(reg), false);
    return reg;
  } else if (root->l_ast->type == AST_LIT) {
    suffix = instrGetMatchingSuffix(r_reg);
    emitAdd(suffix, l_reg, regGetID(r_reg), false);
    return r_reg;
  } else if (root->r_ast->type == AST_LIT) {
    suffix = instrGetMatchingSuffix(l_reg);
    emitAdd(suffix, r_reg, regGetID(l_reg), false);
  } else {
    regEqualize(&l_reg, r_reg);
    suffix = instrGetMatchingSuffix(r_reg);
    emitAdd(suffix, r_reg, regGetID(l_reg), true);
    regUnRsv(r_reg);
  }
  return l_reg;
}

//
// genSub()
// @root - Root of Ast binary operation.
// @l_reg - l_reg.
// @r_reg - r_reg.
//
// Returns the registerID storing the difference of @l_reg and @r_reg.
//
static int genSub(struct Ast *root, int l_reg, int r_reg) {
  char suffix;

  if (root->l_ast->type == AST_LIT && root->r_ast->type == AST_LIT) {
    int reg = regRsv(4);
    suffix = instrGetMatchingSuffix(4 - 1);
    emitMov(suffix, l_reg, reg, 0);
    emitSub(suffix, r_reg, regGetID(reg), false);
    return reg;
  } else if (root->l_ast->type == AST_LIT) {
    suffix = instrGetMatchingSuffix(r_reg);
    emitSub(suffix, l_reg, regGetID(r_reg), false);
    return r_reg;
  } else if (root->r_ast->type == AST_LIT) {
    suffix = instrGetMatchingSuffix(l_reg);
    emitSub(suffix, r_reg, regGetID(l_reg), false);
  } else {
    regEqualize(&l_reg, r_reg);
    suffix = instrGetMatchingSuffix(r_reg);
    emitSub(suffix, r_reg, regGetID(l_reg), true);
    regUnRsv(r_reg);
  }
  return l_reg;
}

// ########################################
//
// Symbols
//
// ########################################

//
// genLoadSymbol()
// @id - Identifer name to load from the stack.
//
// Loads the value at the stack position representing @id into a new register.
// Returns the register ID of the newly allocated register.
//
static int genLoadSymbol(char *id) {
  int sym_idx = symbolFind(id);

  int stk_off = kSymbolsTable[sym_idx].stk_off;
  int sym_sz = kSymbolsTable[sym_idx].sz;
  int reg = regRsv(sym_sz);

  char suffix = instrGetMatchingSuffix(sym_sz - 1);

  emitMov(suffix, stk_off, reg, 2);

  return reg;
}

//
// genStoreSymbol()
// @id - Identifier to set the stack offset of.
//
// Sets the stack offset of @id.
// Returns the stack offset of @id.
//
static int genStoreSymbol(char *id) {
  int sym_idx = symbolFind(id);

  if (sym_idx == -1) {
    ERROR("Wierd symbol found, not sure if we're going to keep this error");
  }

  struct Symbols *symbol = &kSymbolsTable[sym_idx];
  static int last_alloc_var_idx = 0;

  if (symbol->stk_off == 0) {
    symbol->stk_off = kSymbolsTable[last_alloc_var_idx].stk_off + symbol->sz;

    last_alloc_var_idx = sym_idx;
  }

  return symbol->stk_off;
}

//
// genGenerateCode()
// @root - Ast to convert into x86_64 code.
// @parent_type - Used internally for recusrion.
//                SET TO AST_NULL.
//
// Converts @root to x86_64 ATT-style code.
// Results after each operation are immediately stored into @kOutFile.
//
static int genGenerateCode(struct Ast *root) {
  int l_val = 0, r_val = 0;

  if (root->l_ast) {
    l_val = genGenerateCode(root->l_ast);
  }

  if (root->r_ast) {
    r_val = genGenerateCode(root->r_ast);
  }

  switch (root->type) {
    case AST_LIT:
      return root->val.val;
    case AST_ID:
      return genLoadSymbol(kSymbolsTable[root->val.symbol_idx].name);
    case AST_LV_ID:
      return genStoreSymbol(kSymbolsTable[root->val.symbol_idx].name);
    case AST_EQ:
      return genEq(root, l_val, r_val);
    case AST_ASTERISK:
      return genMul(root, l_val, r_val);
    case AST_PLUS:
      return genAdd(root, l_val, r_val);
    case AST_HYPH:
      return genSub(root, l_val, r_val);
    case AST_RET:
      return emitReturn(root, r_val);
    case AST_NULL:
    case AST_GLUE:
      return 0;
    default:
      ERROR("Unknown AST Operator '%i'", root->type);
  }
  return 0;
}

void genGenerate(struct Ast *root, const char *filename) {
  kOutFile = fopen(filename, "w");

  if (kOutFile == NULL) {
    ERROR("Can't open file for R/W '%s'", filename);
  }

  // Generate assembly code

  {
    emitPreamble();

    if (root != NULL) {
     genGenerateCode(root);
    }

    emitPostamble();
  }

  fclose(kOutFile);
}
