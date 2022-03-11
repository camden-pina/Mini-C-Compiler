#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "lexer.h"
#include "parser.h"
#include "gen.h"
#include "symbols.h"

int main(void) {
  char *stream = fileRead("test.c");
  char *out = "out.s";

  // Lexer
  lexerInit();

  struct Token *tokens;
  tokens = lexerTokenize(stream);

  // Parser
  struct Ast *root = parserParse(&tokens);

  // Code Generation
  genGenerate(root, out);

  // Free
  free(stream);
  lexerFree(tokens);
  astFree(root);
  free(root);

  symbolsFree();

  SUCCESS("Compilation process finished");
}
