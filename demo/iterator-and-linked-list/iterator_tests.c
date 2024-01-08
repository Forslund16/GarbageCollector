#include <CUnit/Basic.h>
#include "linked_list.h"
#include "iterator.h"
#include "common.h"

int init_suite(void) {
    // Change this function if you want to do something *before* you
    // run a test suite
    return 0;
}

int clean_suite(void) {
    // Change this function if you want to do something *after* you
    // run a test suite
    return 0;
}

void test_iterator_create()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);
    CU_ASSERT_PTR_NOT_NULL(iter);

    h_delete(heap);
}

void test_iterator_has_next()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);
    CU_ASSERT_EQUAL(ioopm_iterator_has_next(iter), false);
    ioopm_linked_list_append(list, (elem_t) 1, heap);
    CU_ASSERT_EQUAL(ioopm_iterator_has_next(iter), true);

    h_delete(heap);
}

void test_iterator_next()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);
    CU_ASSERT_PTR_NULL(ioopm_iterator_next(iter));
    ioopm_linked_list_append(list, (elem_t) 1, heap);
    CU_ASSERT_EQUAL(ioopm_iterator_next(iter)->int_val, 1);
   
    h_delete(heap);
}

void test_iterator_remove()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);
    CU_ASSERT_PTR_NULL(ioopm_iterator_remove(iter));
    ioopm_linked_list_append(list, (elem_t) 1, heap);
    CU_ASSERT_EQUAL(ioopm_iterator_remove(iter)->int_val, 1);
    //(list);
    h_delete(heap);
}

void test_iterator_insert()
{
    heap_t *heap = h_init(4096, false, 0.1);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 1);
    
    ioopm_list_t *list2 = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter2 = ioopm_list_iterator(list2, heap);
    ioopm_iterator_insert(iter2, (elem_t) 1, heap);
    ioopm_iterator_insert(iter2, (elem_t) 1, heap);
    ioopm_iterator_insert(iter2, (elem_t) 1, heap);

    list2 = NULL;
    iter2 = NULL;
    h_gc(heap);
    ioopm_iterator_insert(iter, (elem_t) 1, heap);
    
    h_delete(heap);
}

void test_iterator_reset()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);

    ioopm_linked_list_append(list, (elem_t) 1, heap);
    ioopm_linked_list_append(list, (elem_t) 2, heap);
    ioopm_linked_list_append(list, (elem_t) 3, heap);

    ioopm_iterator_next(iter);
    ioopm_iterator_next(iter);
    ioopm_iterator_reset(iter);

    CU_ASSERT_EQUAL(ioopm_iterator_next(iter)->int_val, 1);

    
    h_delete(heap);
}

void test_iterator_current()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_list_iterator_t *iter = ioopm_list_iterator(list, heap);

    CU_ASSERT_PTR_NULL(ioopm_iterator_current(iter));

    ioopm_linked_list_append(list, (elem_t) 1, heap);

    CU_ASSERT_EQUAL(ioopm_iterator_current(iter)->int_val, 1)


    h_delete(heap);
}



int main() {
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("IOOPM Assignment 1 iterator_tests", init_suite, clean_suite);
    if (my_test_suite == NULL) {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    printf("Iterator tests\n");

    // This is where we add the test functions to our test suite.
    // For each call to CU_add_test we specify the test suite, the
    // name or description of the test, and the function that runs
    // the test in question. If you want to add another test, just
    // copy a line below and change the information
    if (

            (CU_add_test(my_test_suite, "Iterator create", test_iterator_create) == NULL) ||
            (CU_add_test(my_test_suite, "Iterator has next", test_iterator_has_next) == NULL) ||
            (CU_add_test(my_test_suite, "Iterator next", test_iterator_next) == NULL) ||
            (CU_add_test(my_test_suite, "Iterator remove", test_iterator_remove) == NULL) ||
            (CU_add_test(my_test_suite, "Iterator insert", test_iterator_insert) == NULL) ||
            (CU_add_test(my_test_suite, "Iterator reset", test_iterator_reset) == NULL) ||
            (CU_add_test(my_test_suite, "Iterator current", test_iterator_current) == NULL) ||
            0
            )
    {
        // If adding any of the tests fails, we tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_NORMAL);

    // This is where the tests are actually run!
    CU_basic_run_tests();

    // Tear down CUnit before exiting
    CU_cleanup_registry();
    return CU_get_error();
}
