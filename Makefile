test: ./b1
	./b1

SOURCES = b1.c arm.c string.c value.c rtmap.c unix.c
b1: $(SOURCES)
	cc $(SOURCES) -I. -g -o b1

clean:
	rm -rf b1.dSYM b1 *~

