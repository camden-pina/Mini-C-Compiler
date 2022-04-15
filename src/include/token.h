#ifndef _TOKEN_H_
#define _TOKEN_H_

#define MAX_TOK_LEN 24

enum kTokenType {
  TOK_ID,
  TOK_OP,
  TOK_KYWD,
  TOK_LIT,

  TOK_EQ,
  TOK_SEMI,

  TOK_ASTERISK,
  TOK_PLUS,
  TOK_HYPH,

  TOK_LT,
  TOK_GT,

  TOK_CHAR,
  TOK_INT,
  TOK_LONG,

  TOK_RET,

  UNUSED_LV_ID,
  UNUSED_GLUE,

  TOK_EOF,
  TOK_NULL,
};

// struct Token
// @prev - Pointer to prev token.
// @next - Pointer to next token.
//
// @type - Type for this token.
// @data - Value for this token.
// @len - Length of data for this token.
//
struct Token {
  struct Token *prev;
  struct Token *next;
  
  enum kTokenType type;

  char data[MAX_TOK_LEN];
  int len;
};

//
// tokenCreate()
// @p - Pointer to token data.
// @type - Type for token.
//
// Returns a new token defined by the given parameters.
//
struct Token *tokenCreate(const char *p, enum kTokenType type);

//
// tokenAppend()
// @head - Head of list to append token to.
// @tok - Token to append to @head.
//
// Appends @tok to @head.
//
void tokenAppend(struct Token **head, struct Token *tok);

#endif // _TOKEN_H_
