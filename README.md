# Mini-C-Compiler
This is an early stage C compiler. It is experimental and shouldn't be used in any professional environment.
## Prerequisites
Compiler has been tested stable with the following environment.
* Ubuntu Linux 20.04 LTS<br>
* GCC 11.2

## Build
Simply run 
`
make all
`

## How to use it
After running ```make all```, an executable named ```mcc``` will be generated.</br>

Syntax:</br>
`./mcc <input_file> -o <output_file>`</br>

When successful, the compiler will print a success message.</br>
It's now safe to assume that ```<output_file>``` can be assembled.
