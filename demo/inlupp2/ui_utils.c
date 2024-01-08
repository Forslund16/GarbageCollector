#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "iterator.h"
#include "hash_table.h"
#include "common.h"
#include "cart.h"
#include "warehouse.h"

static bool username_pred(const ioopm_hash_table_t *ht, elem_t key, elem_t value, void *extra, heap_t *heap)
{   
    ioopm_cart_t *cart = value.cart;
    char *name = (char *)extra;
    return (strcmp(cart->id, name) == 0);
}

bool ioopm_has_username(ioopm_shop_t *shop, char *name, heap_t *heap)
{
    return ioopm_hash_table_any(shop->ht_carts, username_pred, name, heap);
}

bool ioopm_check_stock(ioopm_warehouse_t *wh, char *name, int merch_amount)
{
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t) name)->merch;
    if (merch->total_stock >= (merch->reserved_stock + merch_amount))
    {
        merch->reserved_stock += merch_amount;
        return true;
    }
    printf("Not enough stock. Available stock: %d\n", (merch->total_stock - merch->reserved_stock));
    return false;
}

bool ioopm_has_stock(ioopm_warehouse_t *wh, char *merch_name)
{
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t) merch_name)->merch;
    if(merch->total_stock == 0)
    {
        return false;
    }
    return true;
}

void apply_remove_reserved(elem_t key, elem_t *value, void *x)
{
    char *merch_name = key.string_val;
    int removed_amount = value->int_val;

    ioopm_warehouse_t *wh = (ioopm_warehouse_t *)x;

    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t) merch_name)->merch;
    merch->reserved_stock -= removed_amount;
}

void ioopm_remove_reserved_in_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, char *index)
{
    ioopm_cart_t *cart = ioopm_hash_table_lookup(shop->ht_carts, (elem_t) index)->cart;
    ioopm_hash_table_apply_to_all(cart->ht, apply_remove_reserved, wh);
}

static int cmpstringp(const void *p1, const void *p2)//taken from inlupp1
{
  return strcmp(*(char *const *)p1, *(char *const *)p2);
}

elem_t *ioopm_keys_sort(ioopm_hash_table_t *ht_items, size_t size, heap_t *heap)
{
    ioopm_list_t *merch_list = ioopm_hash_table_keys(ht_items ,heap);
    elem_t *keys_array = ioopm_linked_list_to_array(merch_list, heap);
    
    qsort(keys_array, size, sizeof(char *), cmpstringp);
    ioopm_linked_list_destroy(merch_list);
    merch_list = NULL;
    return keys_array;
}

bool pred_in_cart(const ioopm_hash_table_t *ht, elem_t key, elem_t value, void *x, heap_t *heap)
{
    ioopm_cart_t *cart = value.cart;
    char *name = (char *)x;
    return ioopm_hash_table_has_key(cart->ht, (elem_t) name, heap);
}

bool ioopm_in_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, char *name, heap_t *heap)
{
    return ioopm_hash_table_any(shop->ht_carts, pred_in_cart, name, heap);
}