#ifndef _STACK_H_
#define _STACK_H_

//
// stackRsv()
// @id - Identifier to reserve space on stack for.
//
// Reserves space on the stack for @id.
//
void stackRsv(const char *id);

//
// stackGetSize()
//
// Returns the current stack size;
//
int stackGetSize(void);

#endif // _STACK_H_
