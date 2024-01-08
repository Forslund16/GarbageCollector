#pragma once
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "../../src/gc.h"
/**
 * @file ui_utils.h
 * @author William Forslund, Gabriel Hemström
 * @date 15 Okt 2022
 * @brief Utility functions for the UI
 */

typedef struct warehouse ioopm_warehouse_t;

/// @brief checks if username has already been used
/// @param shop the carts that are being checked
/// @param name of the user
/// @return true if name already exists in shop, else false
bool ioopm_has_username(ioopm_shop_t *shop, char *name, heap_t *heap);

/// @brief checks if theres is enough stock of a certain merch
/// @param wh warehouse where merch is stored
/// @param name of the merch that is being checked
/// @param merch_amount the amount of merch the user wants to add to cart
/// @return true if (stock of merch) > merch_amount, else false
bool ioopm_check_stock(ioopm_warehouse_t *wh, char *name, int merch_amount);

/// @brief checks if merch is in stock
/// @param wh warehouse where merch is stored
/// @param merch_name name of merch being checked
/// @return true if merch is in stock, else false
bool ioopm_has_stock(ioopm_warehouse_t *wh, char *merch_name);

/// @brief removes all reserved stock when cart is removed
/// @param wh warehouse where merch is stored
/// @param shop including all carts
/// @param index of cart to remove reserved from
void ioopm_remove_reserved_in_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, char *index);

/// @brief converts keys and adds them to an array
/// @param ht_items hash table mapping names of merch to its contents
/// @param size the amount of elements in ht_items
/// @return an array of merch names in alphabetic order
elem_t *ioopm_keys_sort(ioopm_hash_table_t *ht_items, size_t size, heap_t *heap);

/// @brief checks if merch is currently in any cart
/// @param wh warehouse where merch is stored
/// @param shop hash table of carts
/// @param name of merch
/// @return true if name is in any cart in shop
bool ioopm_in_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, char *name, heap_t *heap);