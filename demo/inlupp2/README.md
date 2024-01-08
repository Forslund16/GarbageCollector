### Integrating Code from Different Sources

Hash table - William Forslund, Johan Yrefors
Linked list - Gabriel Hemström, Per Jutterström
Iterator - Gabriel Hemström, Per Jutterström
Utils - William Forslund, Marcus Nyfjäll


### Building and running

Unit tests
The Makefile contains rules for building and running

`$ make backend_test`

Memory tests
To build and run valgrind memory tests:

`$ make backend_memtest`
`$ make ui_memtest`
`$ make ui_file_memtest`

Coverage tests
To build and run gcov coverage tests:

`$ make ui_coverage`
`$ make test_coverage`


### Line Coverage

When lcov is used on a txt file simulating user interaction:

ui_utils.c        100.0 %
user_interface.c: 96.6 %
warehouse.c:      91.7 %
cart.c:           82.5 %
utils.c:          76.9 %
hash_table.c:     74.5 %
linked_list.c:    47.4 %

When lcov is used on tests:

cart.c:           100.0 %
warehouse.c:      100.0 %
unit_tests_wh.c:  97.3 %
linked_list.c:    56.5 %
hash_table.c:     46.8 %


### Design decisions

* All functions in cart.c and warehouse.c requires valid inputs therefore all faulty inputs are handled in the user interface. This results in a simpler implementation of the backend functions, since they can always assume that the inputs are correct.
* A smaller design decision would be how we divided the functions into resonable moduls such as user interface, backened, cart and ui_utils. 