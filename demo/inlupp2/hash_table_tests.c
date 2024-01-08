#include <CUnit/Basic.h>
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "common.h"
#include "../../src/gc.h"
/*
    Macros for creating hash tables with specified types for keys and values
    ht_create_[key type]_[value type]
*/
#define ht_create_str_int ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_int_eq, heap)
#define ht_create_int_int ioopm_hash_table_create(ioopm_hash_int, ioopm_int_eq, ioopm_int_geq, ioopm_int_eq, heap)
#define ht_create_int_str ioopm_hash_table_create(ioopm_hash_int, ioopm_int_eq, ioopm_int_geq, ioopm_str_eq, heap)
#define ht_create_str_str ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_str_eq, heap)
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

void test_create_destroy()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  h_gc(heap);
  CU_ASSERT_PTR_NOT_NULL(ht);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_insert()
{
  heap_t *heap = h_init(10240, false, 0.3);

  ioopm_hash_table_t *ht = ht_create_str_str;
  
  char *key1 = h_str_dup(heap, "1");
  char *key2 = h_str_dup(heap, "2");
  char *key3 = h_str_dup(heap, "3");
  char *value1 = h_str_dup(heap, "4");
  char *value2 = h_str_dup(heap, "5");
  char *value3 = h_str_dup(heap, "6");

  ioopm_hash_table_insert(ht, (elem_t) key1, (elem_t) value1, heap);
  ioopm_hash_table_insert(ht, (elem_t) key2, (elem_t) value2, heap); // 18%17 == 1 - same bucket
  ioopm_hash_table_insert(ht, (elem_t) key3, (elem_t) value3, heap); // -16%17 == 1 - same bucket

  h_gc(heap);

  CU_ASSERT_PTR_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) "1")->string_val, value1);
  CU_ASSERT_PTR_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) "2")->string_val, value2);
  CU_ASSERT_PTR_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) "3")->string_val, value3);

  char *k = h_str_dup(heap, "hej");
  ioopm_hash_table_insert(ht, (elem_t) k, (elem_t) NULL, heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) "hej")->string_val, NULL);
  CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(ht, (elem_t) "hej"));

  ioopm_hash_table_destroy(ht);

  h_gc(heap);

  ioopm_hash_table_t *ht2 = ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_int_eq, heap);
  
  char *key4 = h_str_dup(heap, "7");
  char *key5 = h_str_dup(heap, "8");
  char *key6 = h_str_dup(heap, "9");
  char *value4 = h_str_dup(heap, "10");
  char *value5 = h_str_dup(heap, "11");
  char *value6 = h_str_dup(heap, "12");

  ioopm_hash_table_insert(ht2, (elem_t) key4, (elem_t) value4, heap);
  ioopm_hash_table_insert(ht2, (elem_t) key5, (elem_t) value5, heap);
  ioopm_hash_table_insert(ht2, (elem_t) key6, (elem_t) value6, heap);

  h_gc(heap);
  
  CU_ASSERT_PTR_EQUAL(ioopm_hash_table_lookup(ht2, (elem_t) "7")->string_val, value4);
  CU_ASSERT_PTR_EQUAL(ioopm_hash_table_lookup(ht2, (elem_t) "8")->string_val, value5);

  ioopm_hash_table_destroy(ht2);
  h_delete(heap);
}

void test_insert_existing_key()
{
  heap_t *heap = h_init(10240, false, 0.3);

  ioopm_hash_table_t *ht = ht_create_int_str;
  int key = 1;
  char *value1 = "foo";
  char *value2 = "bar";
  ioopm_hash_table_insert(ht, (elem_t) key, (elem_t) value1, heap);
  ioopm_hash_table_insert(ht, (elem_t) key, (elem_t) value2, heap);

  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) key)->string_val, "bar");
  CU_ASSERT_NOT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) key)->string_val, "foo");
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);

  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_insert_str()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_str_int;

  ioopm_hash_table_insert(ht, (elem_t) "foo", (elem_t) 1, heap);
  ioopm_hash_table_insert(ht, (elem_t) "bar", (elem_t) 1, heap);
  ioopm_hash_table_insert(ht, (elem_t) "foo", (elem_t) 2, heap);

  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, (elem_t) "fo"));
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) "foo")->int_val, 2);
  CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(ht, (elem_t) "foo"));

  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}


void test_lookup_empty()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  for (int i = 0; i < 18; ++i)
  {
    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, (elem_t) i));
  }
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, (elem_t) -1));
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_lookup_one()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  ioopm_hash_table_insert(ht, (elem_t) 3, (elem_t) "foo", heap);
  elem_t *str = ioopm_hash_table_lookup(ht, (elem_t) 3);
  CU_ASSERT_EQUAL(str->string_val, "foo");
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}


void test_lookup_two()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  ioopm_hash_table_insert(ht, (elem_t) 3, (elem_t) "foo", heap);
  ioopm_hash_table_insert(ht, (elem_t) 20, (elem_t) "bar", heap);
  char *str1 = ioopm_hash_table_lookup(ht, (elem_t) 3)->string_val;
  char *str2 = ioopm_hash_table_lookup(ht, (elem_t) 20)->string_val;
  CU_ASSERT_EQUAL(str1, "foo");
  CU_ASSERT_EQUAL(str2, "bar");
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_remove()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_str_str;
  char *key = h_str_dup(heap, "key");
  char *value = h_str_dup(heap, "test");
  ioopm_hash_table_insert(ht, (elem_t) key, (elem_t) value, heap);
  CU_ASSERT_EQUAL(h_gc(heap), 0);

  ioopm_hash_table_remove(ht, (elem_t) key);
  CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, (elem_t) "key"));
  CU_ASSERT_NOT_EQUAL(h_gc(heap), 0);

  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_remove_non_existing()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;

  CU_ASSERT_PTR_NULL(ioopm_hash_table_remove(ht, (elem_t) 0));

  ioopm_hash_table_insert(ht, (elem_t) 0, (elem_t) "foo", heap);
  ioopm_hash_table_insert(ht, (elem_t) 34, (elem_t) "bar", heap); //0, 17 and 34 are in the same bucket
  CU_ASSERT_PTR_NULL(ioopm_hash_table_remove(ht, (elem_t) 17));

  ioopm_hash_table_destroy(ht);
  h_delete(heap);

}

void test_hash_table_size()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 0);
  //h_gc(heap);
  ioopm_hash_table_insert(ht, (elem_t) 2, (elem_t) "foo", heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 1);
  ioopm_hash_table_insert(ht, (elem_t) 3, (elem_t) "bar", heap);
  ioopm_hash_table_insert(ht, (elem_t) 20, (elem_t) "baz", heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_size(ht), 3);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_hash_table_is_empty()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  //h_gc(heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_is_empty(ht), true);
  ioopm_hash_table_insert(ht, (elem_t) 1, (elem_t) "foo", heap);
  //h_gc(heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_is_empty(ht), false);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_hash_table_clear()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  ioopm_hash_table_clear(ht);
  ioopm_hash_table_insert(ht, (elem_t) -2, (elem_t) "foo", heap);
  ioopm_hash_table_insert(ht, (elem_t) 3, (elem_t) "bar", heap);
  ioopm_hash_table_clear(ht);
  //h_gc(heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_is_empty(ht), true);
  ioopm_hash_table_insert(ht, (elem_t) 1, (elem_t) "foobar", heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) 1)->string_val, "foobar");
  ioopm_hash_table_destroy(ht);
  h_delete(heap);

}

  void test_hash_table_keys()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  ioopm_hash_table_insert(ht, (elem_t) 0, (elem_t) "a", heap);
  ioopm_hash_table_insert(ht, (elem_t) 1, (elem_t) "b", heap);
  ioopm_hash_table_insert(ht, (elem_t) 2, (elem_t) "c", heap);
  ioopm_hash_table_insert(ht, (elem_t) 3, (elem_t) "d", heap);
  //h_gc(heap);
  ioopm_list_t *keys = ioopm_hash_table_keys(ht, heap);
  //h_gc(heap);
  CU_ASSERT_EQUAL(ioopm_linked_list_contains(keys, (elem_t) 0), true);
  CU_ASSERT_EQUAL(ioopm_linked_list_contains(keys, (elem_t) 1), true);
  CU_ASSERT_EQUAL(ioopm_linked_list_contains(keys, (elem_t) 2), true);
  CU_ASSERT_EQUAL(ioopm_linked_list_contains(keys, (elem_t) 3), true);
  CU_ASSERT_EQUAL(ioopm_linked_list_contains(keys, (elem_t) 4), false);

  ioopm_linked_list_destroy(keys);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_hash_table_values()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  int no_keys = 3;
  char *values[] = { "foo", "bar", "baz" };
  for(int i = 0; i < no_keys; ++i)
  {
    ioopm_hash_table_insert(ht, (elem_t) i, (elem_t) values[i], heap);
  }
  //h_gc(heap);
  ioopm_list_t *result = ioopm_hash_table_values(ht, heap);
  bool found[3] = {false};
  ioopm_list_iterator_t *iter = ioopm_list_iterator(result, heap);

  for(int i = 0; i < no_keys; ++i)
  {
    elem_t value = ioopm_iterator_current(iter);
    for(int j = 0; j < no_keys; ++j)
    {
      if(ioopm_str_eq((elem_t) value.string_val, (elem_t) values[j]))
      {
        found[j] = true;
      }
    }
    value = ioopm_iterator_next(iter);
  }
  CU_ASSERT_EQUAL(ioopm_iterator_has_next(iter), false);

  for(int i = 0; i < no_keys; ++i)
  {
    CU_ASSERT_EQUAL(found[i], true);
  }
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(result);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_hash_table_has_key()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  elem_t elem1 = { .int_val = 0 };
  elem_t elem2 = { .string_val = "foo" };
  CU_ASSERT_EQUAL(ioopm_hash_table_has_key(ht, elem1, heap), false);
  //h_gc(heap);
  ioopm_hash_table_insert(ht, elem1, elem2, heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_has_key(ht, elem1, heap), true);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}


void test_hash_table_has_value()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  CU_ASSERT_EQUAL(ioopm_hash_table_has_value(ht, (elem_t) "foo", heap), false);
  ioopm_hash_table_insert(ht, (elem_t) 0, (elem_t) "foo", heap);
  //h_gc(heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_has_value(ht, (elem_t) "foo", heap), true);
  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}


void test_hash_table_all()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  //h_gc(heap);
  int keys[] = {1,2,3};
  for(int i = 0; i < 3; ++i)
  {
    ioopm_hash_table_insert(ht, (elem_t) keys[i], (elem_t) "foo", heap);
  }
  CU_ASSERT_EQUAL(ioopm_hash_table_keys_geq(ht, (elem_t) 1, heap), true);

  ioopm_hash_table_insert(ht, (elem_t) 4, (elem_t) "foo", heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_keys_geq(ht, (elem_t) 4, heap), false);

  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

void test_hash_table_apply_to_all()
{
  heap_t *heap = h_init(10240, false, 0.3);
  ioopm_hash_table_t *ht = ht_create_int_str;
  ioopm_hash_table_insert(ht, (elem_t) 0, (elem_t) "foo", heap);
  ioopm_hash_table_insert(ht, (elem_t) 1, (elem_t) "bar", heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_has_value(ht, (elem_t) "baz", heap), false);
  //h_gc(heap);
  ioopm_hash_table_replace_all_values(ht, (elem_t) "baz");
  CU_ASSERT_EQUAL(ioopm_hash_table_has_value(ht, (elem_t) "foo", heap), false);
  CU_ASSERT_EQUAL(ioopm_hash_table_has_value(ht, (elem_t) "bar", heap), false);
  //h_gc(heap);
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) 0)->string_val, "baz");
  CU_ASSERT_EQUAL(ioopm_hash_table_lookup(ht, (elem_t) 1)->string_val, "baz");

  ioopm_hash_table_destroy(ht);
  h_delete(heap);
}

int main() {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite("IOOPM Assignment 1 hash_table_tests", init_suite, clean_suite);
  if (my_test_suite == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  printf("Hash table tests\n");

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if (
    (CU_add_test(my_test_suite, "Create and Destroy", test_create_destroy) == NULL) ||
    (CU_add_test(my_test_suite, "Insert", test_insert) == NULL) ||
    (CU_add_test(my_test_suite, "Insert existing key", test_insert_existing_key) == NULL) ||
    (CU_add_test(my_test_suite, "Lookup empty", test_lookup_empty) == NULL) ||
    (CU_add_test(my_test_suite, "Lookup one", test_lookup_one) == NULL) ||
    (CU_add_test(my_test_suite, "Lookup two", test_lookup_two) == NULL) ||
    (CU_add_test(my_test_suite, "Remove", test_remove) == NULL) ||
    (CU_add_test(my_test_suite, "Remove non-existing", test_remove_non_existing) == NULL) ||
    (CU_add_test(my_test_suite, "Size", test_hash_table_size) == NULL) ||
    (CU_add_test(my_test_suite, "Is empty", test_hash_table_is_empty) == NULL) ||
    (CU_add_test(my_test_suite, "Clear", test_hash_table_clear) == NULL) ||
    (CU_add_test(my_test_suite, "Hash table keys", test_hash_table_keys) == NULL) ||
    (CU_add_test(my_test_suite, "Hash table values", test_hash_table_values) == NULL) ||
    (CU_add_test(my_test_suite, "Hash table has key", test_hash_table_has_key) == NULL) ||
    (CU_add_test(my_test_suite, "Hash table has value", test_hash_table_has_value) == NULL) ||
    (CU_add_test(my_test_suite, "Hash table all", test_hash_table_all) == NULL) ||
    (CU_add_test(my_test_suite, "Hash table apply to all", test_hash_table_apply_to_all) == NULL) ||
    (CU_add_test(my_test_suite, "insert str", test_insert_str) == NULL) ||
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
