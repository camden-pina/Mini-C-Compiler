#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "lexer.h"
#include "parser.h"
#include "gen.h"
#include "symbols.h"
#include "reg.h"

struct Opt {
  char *in_file;
  char *out_file;
};

static struct Opt kOpt;

//
// parseArgs()
// @argc - Arg count.
// @argv - Arg list.
//
// Parses and stores arg values into kOpt.
//
void parseArgs(int argc, char *argv[]) {
  int i;

  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 'o':
          i++;
          if (argv[i] == NULL) {
            break;
          }

          kOpt.out_file = argv[i];
          break;
        default:
          ERROR("Unknown argument '-%c'", argv[i][1]);
      }
    }
    else {
      kOpt.in_file = argv[i];
    }   
  }

  if (kOpt.in_file == NULL || kOpt.out_file == NULL) {
    ERROR("Syntax is './results.o <input_file> -o <output_file>'");
  }
}

int main(int argc, char *argv[]) {
  parseArgs(argc, argv);

  char *stream = fileRead(kOpt.in_file);
  char *out = kOpt.out_file;

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

  if (regCheckFreeAll() != 1) {
    ERROR("Failed to free all allocated registers.");
  }

  SUCCESS("Compilation process finished");
}
