#include "token.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

struct Token *tokenCreate(const char *p, enum kTokenType type) {
  struct Token *token = calloc(1, sizeof *token);
  if (token == NULL) {
    ERROR("Error allocating memory for token");
  }

  token->type = type;
  token->len = 0;
  token->prev = NULL;
  token->next = NULL;

  if (type == TOK_EOF) {
    return token;
  }

  if (type == TOK_LIT || type == TOK_ID) {
    for (; isdigit(*p) || isalpha(*p); token->len++, p++) {
      token->data[token->len] = *p;
    }
  } else {
    token->data[0] = *p;
    token->len = 1;
  }

 return token;
}

void tokenAppend(struct Token **head, struct Token *tkn) {
  if (*head == NULL) {
    *head = tkn;
    return;
  }

  struct Token *cur_tkn = *head;
  while (cur_tkn->next != NULL) {
    cur_tkn = cur_tkn->next;
  }

  cur_tkn->next = tkn;
  tkn->prev = cur_tkn;
}
