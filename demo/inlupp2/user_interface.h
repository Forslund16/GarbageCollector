#pragma once
#include "hash_table.h"
#include "warehouse.h"
#include "../../src/gc.h"


/**
 * @file user_interface.h
 * @author William Forslund, Gabriel Hemström
 * @date 10 Okt 2022
 * @brief A user interface for the warehouse and shop
 */

//-------------------------WAREHOUSE----------------------------------

/// @brief Lets user add merch with name, description and price to warehouse
/// @param ht_items A hash table mapping names of merch to its price and description
void ioopm_add_merch_to_wh(ioopm_hash_table_t *ht_items, heap_t *heap);

/// @brief lists merch names in alphabetic order
/// @param ht_items A hash table mapping names of merch to its price and description
/// @return An array of merch names 
elem_t *ioopm_list_merch(ioopm_hash_table_t *ht_items, heap_t *heap);

/// @brief Lets user edit name, description and price of merch
/// @param wh Warehouse storing merch
/// @param shop A shop holding carts
void ioopm_edit_merch(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap);

/// @brief Lets user add stock to merch
/// @param wh Warehouse storing merch
void ioopm_replenish_merch(ioopm_warehouse_t *wh, heap_t *heap);

/// @brief Lets user show stock for specified merch
/// @param wh Warehouse storing merch
void ioopm_show_stock(ioopm_warehouse_t *wh, heap_t *heap);

/// @brief Lets user remove merch from warehouse
/// @param wh Warehouse storing merch
/// @param shop A shop holding carts
void ioopm_remove_merch(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap);

//----------------------CART--------------------------------

/// @brief Creates a cart and adds it to shop
/// @param shop A shop holding carts
/// @param index An integer refering to which cart it is
/// @return index + 1
int ioopm_cart_create(ioopm_shop_t *shop, int index, heap_t *heap);

/// @brief Lists all carts in shop
/// @param shop A shop holding carts
/// @return true if (amount of carts) > 0, else false
bool ioopm_list_carts(ioopm_shop_t *shop, heap_t *heap);

/// @brief Lets user remove a cart
/// @param shop A shop holding carts
/// @param wh Warehouse storing merch
void ioopm_cart_remove(ioopm_shop_t *shop, ioopm_warehouse_t *wh, heap_t *heap);

/// @brief Lets user add merch to cart
/// @param wh Warehouse storing merch
/// @param shop A shop holding carts
void ioopm_add_merch_to_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap);

/// @brief Lets user remove certain merch from a cart
/// @param wh Warehouse storing merch
/// @param shop A shop holding carts
void ioopm_remove_from_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap);

/// @brief Lets user check out a cart
/// @param wh Warehouse storing merch
/// @param shop A shop holding carts
void ioopm_checkout_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap);

//Warehouse containing one hash table for merch names and one hash table for shelfs

//A shop containing a hash table mapping a carts number to its contents