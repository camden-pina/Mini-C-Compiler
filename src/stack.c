#include "stack.h"
#include "symbols.h"

extern struct Symbols kSymbolsTable[MAX_SYMBOLS];

// Total size of the stack.
static int kStackSz = 0;

// Most recent allocation size on the stack.
int kStackNewAllocSz = 0;

void stackRsv(const char *id) {
  kStackNewAllocSz = kSymbolsTable[symbolFind(id)].sz;
  kStackSz += kStackNewAllocSz;
}

// Getters
int stackGetSize(void) {
    return kStackSz;
}
