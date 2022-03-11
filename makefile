CC=gcc
CFLAGS=-c -I src/include -g -O2 -Wall -Wextra -Werror -Wshadow -Wdouble-promotion -Wformat=2 -Wformat-truncation -Wundef -fno-common -Wconversion

TARGET=results.o
OBJS=src/main.o src/lexer.o src/token.o src/hash.o src/parser.o src/ast.o src/symbols.o src/file.o src/gen.o src/reg.o src/stack.o src/instr.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lm

.PHONY: test
test:
	valgrind -s \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		./results.o

.PHONY: clean
clean:
	rm -f *.o
	rm -f out.s
	rm -f out

	rm -f src/*.o
