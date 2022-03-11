#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define __COMPILER_NAME__ "Compiler"

#define ERROR(fmt, ...) \
  fprintf(stderr, "[%s] Error: ", __COMPILER_NAME__); \
  fprintf(stderr, fmt, ##__VA_ARGS__); \
  fprintf(stderr, "\n"); \
  exit(EXIT_FAILURE);

#define SUCCESS(fmt, ...) \
  printf("[%s] Success: ", __COMPILER_NAME__); \
  printf(fmt, ##__VA_ARGS__); \
  printf("\n"); \
  exit(EXIT_SUCCESS);

#endif // _COMMON_H_
