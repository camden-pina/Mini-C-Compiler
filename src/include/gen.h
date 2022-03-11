#ifndef _GEN_H_
#define _GEN_H_

#include "ast.h"

//
// genGenerate()
// @root - Ast to generate code from.
// @filename - Name of output file.
//
// Generates AArch64 assembly code from the given Ast (@root).
// Results are stored in a file with name @filename.
//
void genGenerate(struct Ast *root, const char *filename);

#endif // _GEN_H_

