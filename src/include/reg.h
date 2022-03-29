#ifndef _REG_H_
#define _REG_H_

//
// regRsv()
// @bytes - Size of bytes to reserve for register.
//
// Returns the index of reserved register of size @bytes.
//
int regRsv(int bytes);

//
// regUnRsv()
// @reg - Register to unreserve.
//
// Unreserves @reg for future use.
//
void regUnRsv(int reg);

//
// regEqualize()
// @reg1 - Register one.
// @reg2 - Register two.
//
// Changes the register size of @reg1 to the size of @reg2.
// (e.g. regEqualize(eax, rdx) -> rax)
//
void regEqualize(int *reg1, int reg2);

//
// Getters
//

//
// regGetID()
// @reg - Register index.
//
// Returns the name of the register base off @reg.
//
const char *regGetID(int reg);

#endif // _REG_H_
