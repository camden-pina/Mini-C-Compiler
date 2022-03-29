#include "instr.h"
#include "common.h"

// eax(0), ebx(1), ecx(2), edx(3)
// rax(4), rbx(5), rcx(6), rdx(7)
char instrGetMatchingSuffix(int sz) {
  if (sz < 4) {
    return 'l';
  } else if (sz >= 4 && sz < 8) {
    return 'q';
  } else {
    ERROR("Invalid instruction size '%i'", sz);
  }
}

