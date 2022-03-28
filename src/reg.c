#include "reg.h"
#include "common.h"

#define REG_COUNT 12

// Registers
static char *kGenRegList[REG_COUNT] = {
  "%eax", "%ebx", "%ecx", "%edx",
  "%rax", "%rbx", "%rcx", "%rdx",
};

static int kGenRegListFree[REG_COUNT];

int regRsv(int bytes) {
  for (int i = bytes-4; i < bytes; i++) {
    if (kGenRegListFree[i] == 0) {
      if (bytes == 4) {
        kGenRegListFree[i] = 1;
        kGenRegListFree[i + 4] = 1;
//        fprintf(stderr, "Allocating register '%s'\n", kGenRegList[i]);
        return i;
      }
      if (bytes == 8) {
        kGenRegListFree[i] = 1;
        kGenRegListFree[i - 4] = 1;
//        fprintf(stderr, "Allocating register '%s'\n", kGenRegList[i]);
        return i;
      }
    }
  }

  ERROR("Out of available registers");
  return 0;
}

inline void regEqualize(int *reg1, int reg2) {
  if (*reg1 < 4 && reg2 >= 4) {
    *reg1 += 4;
  } else if (*reg1 >= 4 && reg2 < 4) {
    *reg1 -= 4;
  }
}

void regUnRsv(int reg) {
  if (reg < 4 && kGenRegListFree[reg] == 1) {
    kGenRegListFree[reg] = 0;
    kGenRegListFree[reg + 4] = 0;
//    fprintf(stderr, "Freeing register '%s'\n", kGenRegList[reg]);
  } else if (reg >= 4 && kGenRegListFree[reg] == 1) {
    kGenRegListFree[reg] = 0;
    kGenRegListFree[reg - 4] = 0;
//    fprintf(stderr, "Freeing register '%s'\n", kGenRegList[reg]);
  } else {
    ERROR("Attempting to free an already free register");
  }
}

// Getters
const char *regGetID(int reg) {
    return kGenRegList[reg];
}
