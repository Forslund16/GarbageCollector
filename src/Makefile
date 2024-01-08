CC         	= gcc
CFLAGS     	= -Wall -g
CUNIT_LINK 	= -lcunit
LDFLAGS		= -lm
COVERAGE   = -fprofile-arcs -ftest-coverage
VALG_ARGS  	= --leak-check=full
RM_FLAGS	= -rf 
RM_TYPES	= *.o *.exe vgcore.* *.out *.gch *.gcda *.gcno *.css *.html *.info

### GC ###

%.o: %.c
	$(CC) $(CFLAGS) $< -c

gc.o: gc.c gc.h
	$(CC) $(CFLAGS) $? -c

heap_test.o: heap_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c

heap.o: heap.c heap.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c

stack.o: stack.c heap.o stack.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -c

clean:
	rm $(RM_FLAGS) $(RM_TYPES)

