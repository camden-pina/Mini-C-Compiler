CC=gcc
CFLAGS=-c -I src/include -g -O2 -Wall -Wextra -Wshadow -Wdouble-promotion -Wformat=2 -Wformat-truncation -Wundef -fno-common -Wconversion

TARGET=mcc
OBJS=src/main.o src/lexer.o src/token.o src/hash.o src/parser.o src/ast.o src/symbols.o src/file.o src/gen.o src/reg.o src/instr.o

%.o: %.c
	@$(CC) $(CFLAGS) -o $@ $<

all: $(OBJS)
	@$(CC) -o $(TARGET) $(OBJS) -lm

.PHONY: test
test:
	@valgrind -s \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--error-exitcode=1 \
		./$(TARGET) test.c -o test.s
	@gcc test.s -o test
	@echo "\nExecutable file 'test' has been generated!"

.PHONY: clean
clean:
	@rm -f src/*.o
	@rm -f $(TARGET)
	@rm -f test.s
	@rm -f test
