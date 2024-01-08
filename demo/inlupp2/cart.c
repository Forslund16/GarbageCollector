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
#include "ui_utils.h"


ioopm_shop_t *ioopm_shop_create(heap_t *heap)
{
    ioopm_shop_t *shop = h_alloc_struct(heap, "*i");
    ioopm_hash_table_t *shop_ht = ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_str_eq, heap);//keys: cart index, value: cart ht
    shop->ht_carts = shop_ht;
    shop->num_of_carts = 0;
    return shop;
}

ioopm_cart_t *ioopm_backend_cart_create(ioopm_shop_t *shop, char *id, char *index, heap_t *heap)
{
    ioopm_cart_t *cart = h_alloc_struct(heap, "**i");
    cart->ht = ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_int_eq, heap);
    cart->id = id;
    cart->total_price = 0;
    ioopm_hash_table_insert(shop->ht_carts, (elem_t) index, (elem_t) cart, heap);
    return cart;
}

void free_cart(elem_t key_ignored, elem_t *value, void *extra_ignored)
{
    ioopm_cart_t *cart = value->cart;
    ioopm_backend_cart_destroy(cart);
}

void ioopm_shop_destroy(ioopm_shop_t *shop)
{
    ioopm_hash_table_apply_to_all(shop->ht_carts, free_cart, NULL);
    ioopm_hash_table_destroy(shop->ht_carts);
    shop = NULL;
    //free(shop);
}

void ioopm_backend_cart_remove(ioopm_shop_t *shop, char *id)//antag att index finns i ht
{
    ioopm_cart_t *cart = ioopm_hash_table_lookup(shop->ht_carts, (elem_t) id)->cart; //måste ta bort det reserverade
    ioopm_backend_cart_destroy(cart);
    ioopm_hash_table_remove(shop->ht_carts, (elem_t) id);
}


void ioopm_backend_cart_destroy(ioopm_cart_t *cart)
{
    ioopm_hash_table_destroy(cart->ht);
    cart->id = NULL;
    cart = NULL;
    //free(cart->id);
    //free(cart);
}

void ioopm_backend_cart_add(ioopm_warehouse_t *wh, ioopm_cart_t *cart, char *merch_name, int *merch_amount_ptr, heap_t *heap)
{
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t) merch_name)->merch;
    int merch_amount = *merch_amount_ptr;
    cart->total_price += (merch->price * merch_amount);

    elem_t *current_amount_ptr = ioopm_hash_table_lookup(cart->ht, (elem_t) merch_name);
    if(current_amount_ptr != NULL)
    {   
        int current_amount = *current_amount_ptr->int_ptr;
        merch_amount += current_amount;
    }
    *merch_amount_ptr = merch_amount;
    ioopm_hash_table_insert(cart->ht, (elem_t) merch_name, (elem_t) merch_amount_ptr, heap);
}

void ioopm_backend_remove_from_cart(ioopm_warehouse_t *wh, ioopm_cart_t *cart, char *merch_name, int remove_amount, heap_t *heap)
{
    int *current_amount = ioopm_hash_table_lookup(cart->ht, (elem_t) merch_name)->int_ptr;

    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t) merch_name)->merch;
    merch->reserved_stock -= remove_amount;
    cart->total_price -= (merch->price * remove_amount);

    if(*current_amount == remove_amount)
    {
        ioopm_hash_table_remove(cart->ht, (elem_t) merch_name);
    }
    *current_amount -= remove_amount;
}

void apply_checkout(elem_t key, elem_t *value, void *x)
{
    char *merch_name = key.string_val;
    int merch_amount = *(value->int_ptr);
    ioopm_warehouse_t *wh = (ioopm_warehouse_t *)x;
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t) merch_name)->merch;

    merch->reserved_stock -= merch_amount;
    merch->total_stock -= merch_amount;

    ioopm_list_t *merch_list = merch->location;


    link_t *curr_link = merch_list->head;
    ioopm_shelf_t *current = curr_link->value.shelf;

    while(true)
    {
        if(current->quantity > merch_amount)
        {
            current->quantity -= merch_amount;
            break;
            
        }
        else if(current->quantity == merch_amount)
        {
            ioopm_hash_table_remove(wh->ht_shelfs, (elem_t) current->shelf);
            ioopm_linked_list_remove(merch_list, (elem_t) 0);
            break;
        }
        else
        {
            ioopm_hash_table_remove(wh->ht_shelfs, (elem_t) current->shelf);

            merch_amount -= current->quantity;

            curr_link = curr_link->next;
            current = curr_link->value.shelf;
            ioopm_linked_list_remove(merch_list, (elem_t) 0);
            
        }
    }
}

void ioopm_backend_cart_checkout(ioopm_warehouse_t *wh, ioopm_shop_t *shop, char *id)
{
    ioopm_cart_t *cart = ioopm_hash_table_lookup(shop->ht_carts, (elem_t) id)->cart;
    ioopm_hash_table_apply_to_all(cart->ht, apply_checkout, wh);
    ioopm_backend_cart_remove(shop, id);
}