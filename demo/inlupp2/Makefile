CC         = gcc
CFLAGS     = -g
CUNIT_LINK = -lcunit
COVERAGE   = -fprofile-arcs -ftest-coverage
VALG_ARGS  = --leak-check=full
CODE_COVRAGE   = --coverage

### O-Files ###

user_interface.o: user_interface.c
	$(CC) $(CFLAGS) $< -c

hash_table.o: hash_table.c hash_table.h
	$(CC) $(CFLAGS) $< -c

linked_list.o: linked_list.c linked_list.h iterator.h common.h
	$(CC) $(CFLAGS) $< -c

utils.o: utils.c
	$(CC) $(CFLAGS) $< -c

unit_tests_wh.o: unit_tests_wh.c
	$(CC) $(CFLAGS) $< -c

warehouse.o: warehouse.c
	$(CC) $(CFLAGS) $< -c

cart.o: cart.c
	$(CC) $(CFLAGS) $< -c

ui_utils.o: ui_utils.c
	$(CC) $(CFLAGS) $< -c

clean:
	rm -f *.o *.gcno *gcda *.exe *.gcov *.out *.info *.bak gmon.out gprof-analysis.txt
	rm -rf res_wh
	rm -rf res_test
	rm -rf latex
	rm -rf html