#include <CUnit/Basic.h>
#include "linked_list.h"
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

void test_linked_list_create()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    CU_ASSERT_PTR_NOT_NULL(list);

    h_delete(heap);
}

void test_append()
{
    heap_t *heap = h_init(10240, false, 0.3);

    ioopm_list_t *list = ioopm_linked_list_create(ioopm_str_eq, heap);
    ioopm_linked_list_append(list, (elem_t) "hej", heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->string_val, "hej");
    ioopm_linked_list_append(list, (elem_t) "hejsan", heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->string_val, "hej");
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1)->string_val, "hejsan");

    h_delete(heap);
}


void test_prepend()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_linked_list_prepend(list, (elem_t) 3, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 3);
    ioopm_linked_list_prepend(list, (elem_t) 7, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 7);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1)->int_val, 3);

    h_delete(heap);
}


void test_append_prepend()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);

    ioopm_linked_list_prepend(list, (elem_t) 2, heap);
    ioopm_linked_list_prepend(list, (elem_t) 1, heap);
    ioopm_linked_list_append(list, (elem_t) 3, heap);
    ioopm_linked_list_append(list, (elem_t) 4, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 1);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1)->int_val, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 2)->int_val, 3);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 3)->int_val, 4);

    h_delete(heap);
}


void test_insert()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);

    ioopm_linked_list_insert(list, 0, (elem_t) 1, heap);
    ioopm_linked_list_insert(list, 1, (elem_t) 3, heap);
    ioopm_linked_list_insert(list, 1, (elem_t) 2, heap);

    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 1);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1)->int_val, 2);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 2)->int_val, 3);

    ioopm_list_t *list2 = ioopm_linked_list_create(ioopm_str_eq, heap);

    ioopm_linked_list_insert(list2, 0, (elem_t) NULL, heap);

    CU_ASSERT_EQUAL(ioopm_linked_list_get(list2, 0)->int_val, NULL);

    h_delete(heap);
}


void test_remove()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    bool valid = false;

    ioopm_linked_list_append(list, (elem_t) 1, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 0, &valid).int_val, 1);
    CU_ASSERT_EQUAL(valid, true);

    ioopm_linked_list_append(list, (elem_t) 2, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 0, &valid).int_val, 2);
    CU_ASSERT_EQUAL(valid, true);

    ioopm_linked_list_append(list, (elem_t) 3, heap);
    ioopm_linked_list_append(list, (elem_t) 4, heap);
    ioopm_linked_list_append(list, (elem_t) 5, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 1, &valid).int_val, 4);
    CU_ASSERT_EQUAL(valid, true);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 1, &valid).int_val, 5);
    CU_ASSERT_EQUAL(valid, true);
    CU_ASSERT_EQUAL(ioopm_linked_list_remove(list, 4, &valid).int_val, 0);
    CU_ASSERT_EQUAL(valid, false);

    h_delete(heap);
}

void test_get()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    CU_ASSERT_PTR_NULL(ioopm_linked_list_get(list, 0));
    ioopm_linked_list_insert(list, 0, (elem_t) 3, heap);
    CU_ASSERT_PTR_NULL(ioopm_linked_list_get(list, 4));
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 3);

    h_delete(heap);
}

void test_contains()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_bool_eq, heap);

    ioopm_linked_list_prepend(list, (elem_t) true, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_contains(list, (elem_t) true), true);
    CU_ASSERT_EQUAL(ioopm_linked_list_contains(list, (elem_t) false), false);

    ioopm_list_t *list2 = ioopm_linked_list_create(ioopm_str_eq, heap);
    ioopm_linked_list_prepend(list2, (elem_t) "hej", heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_contains(list2, (elem_t) "hej"), true);
    CU_ASSERT_EQUAL(ioopm_linked_list_contains(list2, (elem_t) "vad som helst annars"), false);

    h_delete(heap);
}

void test_size()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_linked_list_prepend(list, (elem_t) 1, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);
    bool valid = false;
    ioopm_linked_list_remove(list,0, &valid);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);

    h_delete(heap);
}

void test_is_empty()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_bool_eq, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_is_empty(list), true);

    h_delete(heap);
}

void test_all_values_eq()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_str_eq, heap);

    for(int i = 0; i < 5; i++)
    {
    ioopm_linked_list_prepend(list, (elem_t) "hej", heap);
    }
    CU_ASSERT_EQUAL(ioopm_linked_list_all_elements_eq(list, (elem_t) "hej"), true);
    CU_ASSERT_EQUAL(ioopm_linked_list_all_elements_eq(list, (elem_t) "hejsan"), false);
    ioopm_linked_list_prepend(list, (elem_t) "hejsan", heap);

    CU_ASSERT_EQUAL(ioopm_linked_list_all_elements_eq(list, (elem_t) "hej"), false);

    h_delete(heap);
}

void test_some_value_eq()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);

    for(int i = 1; i < 6; i++)
    {
        ioopm_linked_list_append(list, (elem_t) i, heap);
    }
    CU_ASSERT_EQUAL(ioopm_linked_list_some_element_eq(list, 1), true);
    CU_ASSERT_EQUAL(ioopm_linked_list_some_element_eq(list, 4), true);
    CU_ASSERT_EQUAL(ioopm_linked_list_some_element_eq(list, 6), false);
    bool valid = false;
    ioopm_linked_list_remove(list, 0, &valid);
    CU_ASSERT_EQUAL(ioopm_linked_list_some_element_eq(list, 1), false);

    h_delete(heap);
}

void test_apply_all()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_linked_list_append(list, (elem_t) 1, heap);
    ioopm_linked_list_append(list, (elem_t) 2, heap);
    int value = 2;
    ioopm_linked_list_add_to_all(list, &value);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 0)->int_val, 3);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, 1)->int_val, 4);

    h_delete(heap);
}

void test_clear()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_linked_list_append(list, (elem_t) 1, heap);
    ioopm_linked_list_append(list, (elem_t) 2, heap);
    ioopm_linked_list_append(list, (elem_t) 3, heap);
    ioopm_linked_list_clear(list);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 0);
    ioopm_linked_list_append(list, (elem_t) 2, heap);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(list), 1);

    h_delete(heap);
}

void test_array()
{
    heap_t *heap = h_init(10240, false, 0.3);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_str_eq, heap);

    ioopm_linked_list_append(list, (elem_t) "foo", heap);
    ioopm_linked_list_append(list, (elem_t) "bar", heap);
    ioopm_linked_list_append(list, (elem_t) "baz", heap);

    elem_t *array = ioopm_linked_list_to_array(list, heap);

    CU_ASSERT_TRUE(strcmp(array[0].string_val, "foo") == 0);
    CU_ASSERT_TRUE(strcmp(array[1].string_val, "bar") == 0);
    CU_ASSERT_TRUE(strcmp(array[2].string_val, "baz") == 0);
    
    h_delete(heap);
}

void test_eq_functions()
{
    heap_t *heap = h_init(4096, false, 0.1);
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_float_eq, heap);

    float num = 1.45;
    ioopm_linked_list_append(list, (elem_t) num, heap);

    CU_ASSERT_TRUE(ioopm_linked_list_contains(list, (elem_t) num));

    ioopm_list_t *list2 = ioopm_linked_list_create(ioopm_uint_eq, heap);

    unsigned i = 54;
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);
    ioopm_linked_list_append(list2, (elem_t) i, heap);

    CU_ASSERT_TRUE(ioopm_linked_list_contains(list2, (elem_t) i));

    h_delete(heap);
}

int main() {
    // First we try to set up CUnit, and exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // We then create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("IOOPM Assignment 1 linked_list_tests", init_suite, clean_suite);
    if (my_test_suite == NULL) {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    printf("Linked list tests\n");

    // This is where we add the test functions to our test suite.
    // For each call to CU_add_test we specify the test suite, the
    // name or description of the test, and the function that runs
    // the test in question. If you want to add another test, just
    // copy a line below and change the information
    if (

            (CU_add_test(my_test_suite, "Create", test_linked_list_create) == NULL) ||
            (CU_add_test(my_test_suite, "Append", test_append) == NULL) ||
            (CU_add_test(my_test_suite, "Prepend", test_prepend) == NULL) ||
            (CU_add_test(my_test_suite, "Append and Prepend", test_append_prepend) == NULL) ||
            (CU_add_test(my_test_suite, "Insert", test_insert) == NULL) ||
            (CU_add_test(my_test_suite, "Get", test_get) == NULL) ||
            (CU_add_test(my_test_suite, "Remove", test_remove) == NULL) ||
            (CU_add_test(my_test_suite, "Contains", test_contains) == NULL) ||
            (CU_add_test(my_test_suite, "Size", test_size) == NULL) ||
            (CU_add_test(my_test_suite, "Is empty", test_is_empty) == NULL) ||
            (CU_add_test(my_test_suite, "All values eq", test_all_values_eq) == NULL) ||
            (CU_add_test(my_test_suite, "Some value eq", test_some_value_eq) == NULL) ||
            (CU_add_test(my_test_suite, "Apply to all", test_apply_all) == NULL) ||
            (CU_add_test(my_test_suite, "Clear", test_clear) == NULL) ||
            (CU_add_test(my_test_suite, "Array", test_array) == NULL) ||
            (CU_add_test(my_test_suite, "Eq_geq", test_eq_functions) == NULL) ||
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
