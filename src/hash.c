#include "hash.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static uint32_t hashFNV1(const uint8_t *key) {
  // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

  uint32_t fnv_offset_bias = 0x811c9dc5;
  uint32_t fnv_prime = 0x01000193;

  uint32_t hash = fnv_offset_bias;

  while (*key) {
    hash ^= *key;
    hash *= fnv_prime;
    key++;
  }

  return hash;
}

static struct HashtableEntry *hashtableEntryCreate(const char *key, 
                                                   enum kTokenType val) {
  struct HashtableEntry *entry;
  entry = calloc(1, sizeof *entry);
  if (entry == NULL) {
    ERROR("Error allocating memory for hashtable entry");
  }
  entry->prev = NULL;
  entry->next = NULL;
  entry->key = calloc(1, strlen(key) + 1);
  entry->val = val;

  memcpy(entry->key, key, strlen(key) + 1);

  return entry;
}

enum kTokenType hashtableEntryGet(struct Hashtable *hashtable, 
                                  const char *key) {
  uint32_t idx = hashFNV1((uint8_t *)key) % hashtable->entry_cnt;
  struct HashtableEntry *cur_entry = hashtable->entries[idx];

  while (cur_entry != NULL) {
    if (strncmp(cur_entry->key, key, strlen(key) + 1) == 0) {
      return cur_entry->val;
    } else if (cur_entry->next) {
      cur_entry = cur_entry->next;
    } else {
      return TOK_NULL;
    }
  }

  return TOK_NULL;
}

void hashtableEntryAdd(struct Hashtable *hashtable, const char *key, 
                       enum kTokenType val) {
  uint32_t idx = hashFNV1((uint8_t *)key) % hashtable->entry_cnt;
  struct HashtableEntry **entry = &hashtable->entries[idx];

  struct HashtableEntry *new_entry = hashtableEntryCreate(key, val);

  if (*entry == NULL) {
    *entry = new_entry;
    return;
  }

  // Handle Collision
  struct HashtableEntry *cur_entry = *entry;

  while (cur_entry->next != NULL) {
    cur_entry = cur_entry->next;
  }
  cur_entry->next = new_entry;
  new_entry->prev = cur_entry;
}

struct Hashtable *hashtableCreate(uint32_t entry_cnt) {
  struct Hashtable *hashtbl;
  hashtbl = malloc(sizeof *hashtbl);
  if (hashtbl == NULL) {
    ERROR("Allocating memory for hashtable");
  }

  hashtbl->entries = calloc(1, sizeof *hashtbl->entries * entry_cnt);
  if (hashtbl->entries == NULL) {
    ERROR("Allocating memory for hashtable entries");
  }

  hashtbl->entry_cnt = entry_cnt;

  return hashtbl;
}

void hashtableFree(struct Hashtable *hashtable) {
  for (uint32_t idx = 0; idx < hashtable->entry_cnt; idx++) {
    if (hashtable->entries[idx] != NULL) {
      struct HashtableEntry *cur_entry = hashtable->entries[idx];

      while (cur_entry != NULL) {
        struct HashtableEntry *next_entry = cur_entry->next;

        free(cur_entry->key);
        free(cur_entry);
        cur_entry = next_entry;
      }
    }
  }
  free(hashtable->entries);
  free(hashtable);
}

