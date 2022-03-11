#include "instr.h"
#include "common.h"

char instrGetMatchingSuffix(int sz) {
  if (sz <= 4) {
    return 'l';
  } else if (sz > 4 && sz <= 8) {
    return 'q';
  } else {
    ERROR("Invalid instruction size '%i'", sz);
  }
}

