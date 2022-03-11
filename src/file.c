#include "file.h"

#include "common.h"

#include <stdio.h>
#include <stdlib.h>

char *fileRead(const char *filename) {
  FILE *f;
  f = fopen(filename, "r");

  if (f == NULL) {
    ERROR("Couldn't open file for reading: '%s'", filename);
  }

  fseek(f, 0L, SEEK_END);
  size_t file_sz = (size_t)ftell(f);
  rewind(f);

  char *stream = calloc(1, file_sz + 1);
  if (stream == NULL) {
    ERROR("Couldn't allocate memory buffer for file '%s'", filename);
  }

  stream[file_sz] = '\0';
  size_t read_sz = fread(stream, 1, file_sz, f);
  if (file_sz != read_sz) {
    ERROR("Read error occured on file '%s'", filename);
  }

  fclose(f);

  return stream;
}

