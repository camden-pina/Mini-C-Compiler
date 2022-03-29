#include "lexer.h"
#include "common.h"
#include "hash.h"

#include <stdlib.h>
#include <ctype.h>

struct Hashtable *kLexerHashtable;

//
// isKeyword()
// @tkn - Token to compare.
//
// Checks if @tkn is a recognized keyword.
//
static int lexerIsKeyword(struct Token *tkn) {
  return hashtableEntryGet(kLexerHashtable, tkn->data) != TOK_NULL ? 1 : 0;
}

struct Token *lexerTokenize(const char *stream) {
  const char *p = stream;

  struct Token *head = NULL;
  struct Token *token = NULL;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    } else if (*p == '/' && p[1] == '/') {
      p += 2;
      while (*p != '\n') {
        p++;
      }
      p++;
      continue;
    } else if (*p == '/' && p[1] == '*') {
      p += 2;
      while (*p != '*' && p[1] != '/') {
        p++;
      }
      p += 2;
      continue;
    } else if (isdigit(*p)) {
      token = tokenCreate(p, TOK_LIT);
    } else if (isalpha(*p)) {
      token = tokenCreate(p, TOK_ID);

      if (lexerIsKeyword(token)) {
        token->type = hashtableEntryGet(kLexerHashtable, token->data);
      }
    } else {
      // Handle operators and punctuators.
      switch (*p) {
        case '*':
          token = tokenCreate(p, TOK_ASTERISK);
          break;
        case '+':
          token = tokenCreate(p, TOK_PLUS);
          break;
        case '-':
          token = tokenCreate(p, TOK_HYPH);
          break;
        case '=':
          token = tokenCreate(p, TOK_EQ);
          break;
        case ';':
          token = tokenCreate(p, TOK_SEMI);
          break;
        default:
          ERROR("Unrecognized Token: '%c'", *p);
      }
    }

    // Append the newly created token to our linked list structure.
    tokenAppend(&head, token);
    p += token->len;
  }

  if (token == NULL) {
    ERROR("No input stream supplied");
  }

  // End of linked list must mark the end of the code.
  token = tokenCreate(p, TOK_EOF);
  tokenAppend(&head, token);

  return head;
}

void lexerInit(void) {
  kLexerHashtable = hashtableCreate(100);

  // Keywords
  hashtableEntryAdd(kLexerHashtable, "int", TOK_INT);
  hashtableEntryAdd(kLexerHashtable, "long", TOK_LONG);

  hashtableEntryAdd(kLexerHashtable, "return", TOK_RET);
}

void lexerFree(struct Token *head) {
  hashtableFree(kLexerHashtable);

  // Deallocate linked list.
  struct Token *tkn = head;

  while (tkn->prev) {
    tkn = tkn->prev;
    free(tkn->next);
  }
  free(tkn);
}

void lexerPrint(struct Token *head) {
  while (head->next) {
    fprintf(stderr, "[%u][%s]\n", head->type, head->data);
    head = head->next;
  }
  fprintf(stderr, "[%s]\n", head->data);
}
