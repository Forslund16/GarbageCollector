#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "warehouse.h"
#include "cart.h"
#include "hash_table.h"
#include "common.h"
#include "linked_list.h"
#include "iterator.h"

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

void test_add_merch(void)
{
    heap_t *heap = h_init(10240, false, 0.7);

    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);

    char *name2 = h_str_dup(heap, "pant");
    char *desc2 = h_str_dup(heap, "eat pant");
    ioopm_merch_t *test_merch2 = ioopm_merch_create(name2, desc2, 17110, NULL, heap);

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);
    ioopm_backend_add(wh->ht_items, name2, test_merch2, heap);
    h_gc(heap);

    CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt"));
    CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "pant"));

    CU_ASSERT_EQUAL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt")->merch, test_merch1);
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "pant")->merch, test_merch2);
    CU_ASSERT_EQUAL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "pant")->merch->price, 17110);

    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}

void test_edit_merch(void)
{
    heap_t *heap = h_init(10240, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt"));

    CU_ASSERT_EQUAL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt")->merch, test_merch1);

    char *new_name = h_str_dup(heap, "new_shirt");
    char *new_desc = h_str_dup(heap, "a new shirt");
    int new_price = 10000;

    char *shelf_name = h_str_dup(heap, "A11");
    ioopm_backend_replenish(wh, "shirt", shelf_name, 15, heap);
    char *shelf_name1 = h_str_dup(heap, "A12");
    ioopm_backend_replenish(wh, "shirt", shelf_name1, 15, heap);
    ioopm_backend_edit(wh, "shirt", new_name, new_desc, new_price, heap);

    CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "new_shirt"));
    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt"));
    CU_ASSERT_TRUE(strcmp(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "new_shirt")->merch->name, new_name) == 0);

    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}

void test_replenish(void)
{
    heap_t *heap = h_init(10240, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    ioopm_backend_show_stock(wh->ht_items, "shirt", heap);

    char *shelf1 = h_str_dup(heap, "A32");
    ioopm_backend_replenish(wh, "shirt", shelf1, 69, heap);
    char *shelf2 = h_str_dup(heap, "A32");
    ioopm_backend_replenish(wh, "shirt", shelf2, 69, heap);

    char *shelf3 = h_str_dup(heap, "B32");
    ioopm_backend_replenish(wh, "shirt", shelf3, 69, heap);

    char *shelf4 = h_str_dup(heap, "B32");
    ioopm_backend_replenish(wh, "shirt", shelf4, 69, heap);

    CU_ASSERT_TRUE(strcmp(ioopm_hash_table_lookup(wh->ht_shelfs, (elem_t) "A32")->string_val, name1) == 0);

    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}

void test_remove_merch(void)
{
    heap_t *heap = h_init(10240, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);
    
    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    char *shelf1 = h_str_dup(heap, "A32");
    ioopm_backend_replenish(wh, "shirt", shelf1, 69, heap);
    char *shelf2 = h_str_dup(heap, "A33");
    ioopm_backend_replenish(wh, "shirt", shelf2, 69, heap);
    ioopm_backend_show_stock(wh->ht_items, "shirt", heap);

    ioopm_backend_remove(wh, "shirt", heap);

    h_gc(heap);

    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt"));
    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(wh->ht_shelfs, (elem_t) "A32"));

    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}

//--------------------------CART------------------------------

void test_add_to_cart()
{
    heap_t *heap = h_init(20480, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);
    ioopm_shop_t *shop = ioopm_shop_create(heap);
    char *cart_name1 = h_str_dup(heap, "Willy");
    char *cart_name2 = h_str_dup(heap, "Erik");
    char *cart_name3 = h_str_dup(heap, "Anton");
    char *cart_name4 = h_str_dup(heap, "Lol");
    ioopm_cart_t *cart = ioopm_backend_cart_create(shop, cart_name1,  h_str_dup(heap, "1"), heap);
    ioopm_cart_t *cart2 = ioopm_backend_cart_create(shop, cart_name2, h_str_dup(heap, "2"), heap);
    ioopm_cart_t *cart3 = ioopm_backend_cart_create(shop, cart_name3, h_str_dup(heap, "3"), heap);
    ioopm_cart_t *cart4 = ioopm_backend_cart_create(shop, cart_name4, h_str_dup(heap, "4"), heap);

    ioopm_backend_cart_remove(shop, "4");
    ioopm_backend_cart_remove(shop, "3");
    ioopm_backend_cart_remove(shop, "2");
    cart4 = NULL;
    cart3 = NULL;
    cart2 = NULL;
    size_t cleaned = h_gc(heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    char *shelf1 = h_str_dup(heap, "A32");
    ioopm_backend_replenish(wh, "shirt", shelf1, 69, heap);
    char *shelf2 = h_str_dup(heap, "A33");
    ioopm_backend_replenish(wh, "shirt", shelf2, 69, heap);

    int *merch_amount_ptr = h_alloc_data(heap, sizeof(int));
    *merch_amount_ptr = 1;
    ioopm_backend_cart_add(wh, cart, name1, merch_amount_ptr, heap);

    CU_ASSERT_EQUAL(*ioopm_hash_table_lookup(cart->ht, (elem_t) "shirt")->int_ptr, 1);

    int *merch_amount_ptr1 = h_alloc_data(heap, sizeof(int));
    *merch_amount_ptr1 = 5;
    ioopm_backend_cart_add(wh, cart, name1, merch_amount_ptr1, heap);

    CU_ASSERT_EQUAL(*ioopm_hash_table_lookup(cart->ht, (elem_t) "shirt")->int_ptr, 6);
    CU_ASSERT_NOT_EQUAL(*ioopm_hash_table_lookup(cart->ht, (elem_t) "shirt")->int_ptr, 5);

    ioopm_backend_remove(wh, name1, heap);
  
    h_delete(heap);
}

void test_remove_from_cart()
{
    heap_t *heap = h_init(10240, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);
    ioopm_shop_t *shop = ioopm_shop_create(heap);
    char *cart_name = h_str_dup(heap, "Willy");
    ioopm_cart_t *cart = ioopm_backend_cart_create(shop, cart_name, "1", heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    char *shelf1 = h_str_dup(heap, "A32");
    ioopm_backend_replenish(wh, "shirt", shelf1, 69, heap);
    char *shelf2 = h_str_dup(heap, "A33");
    ioopm_backend_replenish(wh, "shirt", shelf2, 69, heap);

    int *merch_amount = h_alloc_data(heap, sizeof(int));
    *merch_amount = 45;
    ioopm_backend_cart_add(wh, cart, name1, merch_amount, heap);
    CU_ASSERT_EQUAL(*ioopm_hash_table_lookup(cart->ht, (elem_t) "shirt")->int_ptr, 45);
    ioopm_backend_remove_from_cart(wh, cart, name1, 40, heap);
    CU_ASSERT_EQUAL(*ioopm_hash_table_lookup(cart->ht, (elem_t) "shirt")->int_ptr, 5);
    ioopm_backend_remove_from_cart(wh, cart, name1, 5, heap);

    ioopm_shop_destroy(shop);
    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}

void test_checkout()
{
    heap_t *heap = h_init(10240, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);
    ioopm_shop_t *shop = ioopm_shop_create(heap);
    char *cart_name = h_str_dup(heap, "Willy");
    ioopm_cart_t *cart = ioopm_backend_cart_create(shop, cart_name, h_str_dup(heap, "1"), heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);
    char *shelf1 = h_str_dup(heap, "A32");

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    ioopm_backend_replenish(wh, "shirt", shelf1, 30, heap);

    int *merch_amount = h_alloc_data(heap, sizeof(int));
    *merch_amount = 15;
    ioopm_backend_cart_add(wh, cart, name1, merch_amount, heap);
    ioopm_backend_remove_from_cart(wh, cart, name1, 5, heap);

    ioopm_list_t *list = ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt")->merch->location;

    CU_ASSERT_PTR_NOT_NULL(list);

    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, (elem_t)0).shelf->quantity, 30);
    ioopm_backend_cart_checkout(wh, shop, "1");
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, (elem_t)0).shelf->quantity, 20);

    ioopm_shop_destroy(shop);
    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}

void test_checkout_mult_shelf()
{
    heap_t *heap = h_init(10240, false, 0.7);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);
    ioopm_shop_t *shop = ioopm_shop_create(heap);
    char *cart_name = h_str_dup(heap, "Willy");
    ioopm_cart_t *cart = ioopm_backend_cart_create(shop, cart_name, h_str_dup(heap, "1"), heap);

    char *name1 = h_str_dup(heap, "shirt");
    char *desc1 = h_str_dup(heap, "a shirt");
    ioopm_merch_t *test_merch1 = ioopm_merch_create(name1, desc1, 17990, NULL, heap);
    char *shelf1 = h_str_dup(heap, "A32");
    char *shelf2 = h_str_dup(heap, "A33");

    ioopm_backend_add(wh->ht_items, name1, test_merch1, heap);

    ioopm_backend_replenish(wh, "shirt", shelf1, 30, heap);
    ioopm_backend_replenish(wh, "shirt", shelf2, 5, heap);

    int *merch_amount = h_alloc_data(heap, sizeof(int));
    *merch_amount = 35;
    ioopm_backend_cart_add(wh, cart, name1, merch_amount, heap);

    ioopm_list_t *list = ioopm_hash_table_lookup(wh->ht_items, (elem_t) "shirt")->merch->location;

    CU_ASSERT_PTR_NOT_NULL(list);

    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, (elem_t)0).shelf->quantity, 30);
    CU_ASSERT_EQUAL(ioopm_linked_list_get(list, (elem_t)1).shelf->quantity, 5);
    ioopm_backend_cart_checkout(wh, shop, "1");

    ioopm_shop_destroy(shop);
    ioopm_warehouse_destroy(wh);
    h_delete(heap);
}
int main()
{
    CU_pSuite test_suite1 = NULL;

    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
    if (NULL == test_suite1)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        (NULL == CU_add_test(test_suite1, "Add_merch", test_add_merch)) ||
        (NULL == CU_add_test(test_suite1, "Replenish", test_replenish)) ||
        (NULL == CU_add_test(test_suite1, "Remove_merch", test_remove_merch)) ||
        (NULL == CU_add_test(test_suite1, "Edit_merch", test_edit_merch)) ||
        (NULL == CU_add_test(test_suite1, "Add_to_cart", test_add_to_cart)) ||
        (NULL == CU_add_test(test_suite1, "Remove_from_cart", test_remove_from_cart)) ||
        (NULL == CU_add_test(test_suite1, "Checkout", test_checkout)) ||
        (NULL == CU_add_test(test_suite1, "Checkout_mult_shelf", test_checkout_mult_shelf)) ||
        0)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}