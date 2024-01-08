CC         	= gcc
CFLAGS     	= -g
CUNIT_LINK 	= -lcunit
LDFLAGS		= -lm
COVERAGE   = -fprofile-arcs -ftest-coverage --coverage
VALG_ARGS  	= --leak-check=full
RM_FLAGS	= -rf 
RM_TYPES	= *.o *.exe vgcore.* *.out *.gch *.gcda *.gcno *.css *.html *.info *.gcov
TEST_DIR 	= test
CODE_DIR 	= src
DEMO_DIR	= demo
ITER_LIST_DIR	= $(DEMO_DIR)/iterator-and-linked-list
INLUPP2_DIR		= $(DEMO_DIR)/inlupp2
CODE_REQ				= $(CODE_DIR)/gc.o $(CODE_DIR)/heap.o $(CODE_DIR)/stack.o $(CODE_DIR)/layout.o
CODE_C_REQ				= $(CODE_DIR)/gc.c $(CODE_DIR)/heap.c $(CODE_DIR)/stack.c $(CODE_DIR)/layout.c
DEMO_ITERATOR_REQ		= $(ITER_LIST_DIR)/iterator.o $(ITER_LIST_DIR)/linked_list.o $(ITER_LIST_DIR)/iterator_tests.o
DEMO_ITERATOR_C_REQ		= $(ITER_LIST_DIR)/iterator.c $(ITER_LIST_DIR)/linked_list.c $(ITER_LIST_DIR)/iterator_tests.c
DEMO_LINKED_LIST_REQ	= $(ITER_LIST_DIR)/iterator.o $(ITER_LIST_DIR)/linked_list.o $(ITER_LIST_DIR)/linked_list_tests.o
DEMO_LINKED_LIST_C_REQ	= $(ITER_LIST_DIR)/iterator.c $(ITER_LIST_DIR)/linked_list.c $(ITER_LIST_DIR)/linked_list_tests.c
DEMO_HASH_TABLE_REQ		= $(INLUPP2_DIR)/hash_table.o $(INLUPP2_DIR)/linked_list.o $(INLUPP2_DIR)/hash_table_tests.o
DEMO_HASH_TABLE_C_REQ	= $(INLUPP2_DIR)/hash_table.c $(INLUPP2_DIR)/linked_list.c $(INLUPP2_DIR)/hash_table_tests.c
DEMO_INLUPP2_UI			= $(INLUPP2_DIR)/user_interface.o $(INLUPP2_DIR)/hash_table.o $(INLUPP2_DIR)/linked_list.o $(INLUPP2_DIR)/warehouse.o $(INLUPP2_DIR)/cart.o $(INLUPP2_DIR)/utils.o $(INLUPP2_DIR)/ui_utils.o
DEMO_WAREHOUSE_REQ		= $(INLUPP2_DIR)/unit_tests_wh.o $(INLUPP2_DIR)/warehouse.o $(INLUPP2_DIR)/hash_table.o $(INLUPP2_DIR)/linked_list.o $(INLUPP2_DIR)/cart.o
DEMO_WAREHOUSE_C_REQ	= $(INLUPP2_DIR)/unit_tests_wh.c $(INLUPP2_DIR)/warehouse.c $(INLUPP2_DIR)/hash_table.c $(INLUPP2_DIR)/linked_list.c $(INLUPP2_DIR)/cart.c

####	TEST	####

test: compile_test
	./$(TEST_DIR)/$@.out

compile_test: $(TEST_DIR)/unit_tests.o $(CODE_REQ) 
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) -o $(TEST_DIR)/test.out $(LDFLAGS) 

compile_test_coverage: $(TEST_DIR)/unit_tests.c $(CODE_DIR)/gc.c $(CODE_DIR)/heap.c $(CODE_DIR)/stack.c $(CODE_DIR)/layout.c
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) $(COVERAGE) -o $(CODE_DIR)/test.out $(LDFLAGS)

perf: $(TEST_DIR)/perf.c $(CODE_REQ)
	$(CC) $(CFLAGS) $^ -o $(TEST_DIR)/$@.out $(LDFLAGS)
	./$(TEST_DIR)/perf.out

####################
memtest: compile_test
	valgrind $(VALG_ARGS) ./$(TEST_DIR)/test.out

test_gcov: compile_test_coverage
	./$(CODE_DIR)/test.out
	gcovr -r . --html-details coverage.html

####################


####	DEMO	####

demo_iterator: $(CODE_REQ) $(DEMO_ITERATOR_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) -o $(DEMO_DIR)/$@.out $(LDFLAGS)

demo_linked_list: $(CODE_REQ) $(DEMO_LINKED_LIST_REQ) 
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) -o $(DEMO_DIR)/$@.out $(LDFLAGS)

demo_hash_table: $(CODE_REQ) $(DEMO_HASH_TABLE_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) -o $(DEMO_DIR)/$@.out $(LDFLAGS)

demo_warehouse: $(CODE_REQ) $(DEMO_WAREHOUSE_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) -o $(DEMO_DIR)/$@.out $(LDFLAGS)

demo_test: demo_iterator demo_linked_list demo_hash_table demo_warehouse
	./$(DEMO_DIR)/demo_iterator.out
	./$(DEMO_DIR)/demo_linked_list.out
	./$(DEMO_DIR)/demo_hash_table.out
	./$(DEMO_DIR)/demo_warehouse.out

demo_memtest: demo_iterator demo_linked_list demo_hash_table demo_warehouse
	valgrind $(VALG_ARGS) ./$(DEMO_DIR)/demo_iterator.out
	valgrind $(VALG_ARGS) ./$(DEMO_DIR)/demo_linked_list.out
	valgrind $(VALG_ARGS) ./$(DEMO_DIR)/demo_hash_table.out
	valgrind $(VALG_ARGS) ./$(DEMO_DIR)/demo_warehouse.out

demo_inlupp2: $(CODE_REQ) $(DEMO_INLUPP2_UI)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) -o $(DEMO_DIR)/$@.out $(LDFLAGS)

demo: demo_inlupp2
	./$(DEMO_DIR)/$<.out

demo_inlupp2_memtest: demo_inlupp2
	valgrind $(VALG_ARGS) ./$(DEMO_DIR)/$<.out

####################


####	DEMO COVERAGE	####

compile_wh_coverage: $(CODE_C_REQ) $(DEMO_WAREHOUSE_C_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) $(COVERAGE) -o $(INLUPP2_DIR)/wh_gcov.out $(LDFLAGS)

demo_wh_gcov: compile_wh_coverage
	./$(INLUPP2_DIR)/wh_gcov.out
	gcovr -r . --html-details coverage.html

compile_ht_coverage: $(CODE_C_REQ) $(DEMO_HASH_TABLE_C_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) $(COVERAGE) -o $(INLUPP2_DIR)/ht_gcov.out $(LDFLAGS)

demo_ht_gcov: compile_ht_coverage
	./$(INLUPP2_DIR)/ht_gcov.out
	gcovr -r . --html-details coverage.html

compile_ll_coverage: $(CODE_C_REQ) $(DEMO_LINKED_LIST_C_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) $(COVERAGE) -o $(ITER_LIST_DIR)/ll_gcov.out $(LDFLAGS)

demo_ll_gcov: compile_ll_coverage
	./$(ITER_LIST_DIR)/ll_gcov.out
	gcovr -r . --html-details coverage.html

compile_iter_coverage: $(CODE_C_REQ) $(DEMO_ITERATOR_C_REQ)
	$(CC) $(CFLAGS) $^ $(CUNIT_LINK) $(COVERAGE) -o $(ITER_LIST_DIR)/iter_gcov.out $(LDFLAGS)

demo_iter_gcov: compile_iter_coverage
	./$(ITER_LIST_DIR)/iter_gcov.out
	gcovr -r . --html-details coverage.html

####################


####	ALL		####

all_test: demo_test test

all_memtest: demo_memtest memtest

all_gcov: test_gcov demo_wh_gcov demo_ht_gcov demo_ll_gcov demo_iter_gcov

####################


####	CLEAN	####

clean:
	rm $(RM_FLAGS) $(RM_TYPES)
	$(MAKE) -C $(CODE_DIR) clean
	$(MAKE) -C $(TEST_DIR) clean
	$(MAKE) -C $(DEMO_DIR) clean