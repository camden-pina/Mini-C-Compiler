#include "symbols.h"

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct Symbols kSymbolsTable[MAX_SYMBOLS];
int kSymbolsCount = 0;

char *symbolRead(const int symbol_idx) {
  return kSymbolsTable[symbol_idx].name;
}

int symbolFind(const char *name) {
  for (int symbol_idx = 0; symbol_idx < kSymbolsCount; symbol_idx++) {
    if (strncmp(name, kSymbolsTable[symbol_idx].name, strlen(name)) == 0) {
      return symbol_idx;
    }
  }
  return -1;
}

//
// symbolGetNewIdx()
//
// Returns an integer representing a free index into the symbols table.
//
static int symbolGetNewIdx(void) {
  int symbol_idx;

  if ((symbol_idx = kSymbolsCount++) >= MAX_SYMBOLS) {
    ERROR("Usage of over '%i' identifiers is undefined", MAX_SYMBOLS);
  }
  return symbol_idx;
}

int symbolAdd(const char *name, int sz) {
  int symbol_idx;
  
  if ((symbol_idx = symbolFind(name)) != -1) {
    ERROR("Redefinition of identifier '%s'", name);
  }

  symbol_idx = symbolGetNewIdx();

#ifdef WIN32
  kSymbolsTable[symbol_idx].name = _strdup(name);
#else
  kSymbolsTable[symbol_idx].name = strdup(name);
#endif
  kSymbolsTable[symbol_idx].sz = sz;

  return symbol_idx;
}

void symbolsFree(void) {
  int symbol_idx = 0;

  while (symbolRead(symbol_idx) != NULL) {
    free(kSymbolsTable[symbol_idx].name);
    symbol_idx++;
  }
}

