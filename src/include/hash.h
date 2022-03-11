#ifndef _HASH_
#define _HASH_

#include "token.h"

#include <stdint.h>

//
// struct HashtableEntry
//
// Used to handle collision.
// @prev - Pointer to prev entry.
// @next - Pointer to next extry.
//
// @key - Key used to access entry value.
// @val - Value stored within entry.
//
struct HashtableEntry {
  struct HashtableEntry *prev;
  struct HashtableEntry *next;
  char *key;
  enum kTokenType val;
};

//
// struct Hashtable
// @entries - Array of allocated hashtable entries.
// @entry_cnt - Count of hashtable entries.
//
struct Hashtable {
  struct HashtableEntry **entries;
  uint32_t entry_cnt;
};

//
// hashtableEntryGet():
// hashtable - Hashtable to retrieve value from.
// key - Key associated with value.
//
// Returns the value associated with the given key.
//
enum kTokenType hashtableEntryGet(struct Hashtable *hashtable, const char *key);

//
// hashtableEntryAdd():
// @hashtable - Hashtable to add data to.
// @key - Key to associate with data.
// @val - Data to associate with key.
//
// Creates a new entry for the given hashtable with the specified key and
// value.
//
// Returns nothing.
//
void hashtableEntryAdd(struct Hashtable *hashtable, const char *key, 
                       enum kTokenType val);

//
// hashtableCreate():
// @entries_cnt - Number of original entries to give the hashtable.
//
// Initializes a hashtable with the specified number of entries.
//
// Returns the newly created hashtable.
//
struct Hashtable *hashtableCreate(uint32_t entries_cnt);

//
// hashtableFree()
// @hashtable - Hashtable to be freed.
//
// Frees hashtable from memory.
//
void hashtableFree(struct Hashtable *hashtable);

#endif // _HASH_
