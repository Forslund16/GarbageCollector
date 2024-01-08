#pragma once
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"

/**
 * @file cart.h
 * @author William Forslund, Gabriel Hemström
 * @date 10 Okt 2022
 * @brief A cart where we place merch
 *   All functions in this file requires inputs to be valid
 */

typedef struct warehouse ioopm_warehouse_t;

/// @brief creates a shop
/// @return a shop containing a hash table mapping cart numbers to carts
ioopm_shop_t *ioopm_shop_create(heap_t *heap);

/// @brief creates a cart
/// @param id username of cart user
/// @return a cart containing a hash table mapping merch names to amount in that cart
ioopm_cart_t *ioopm_backend_cart_create(ioopm_shop_t *shop, char *id, char *index, heap_t *heap);

/// @brief frees memory allocated in cart
/// @param cart getting destroyed
void ioopm_backend_cart_destroy(ioopm_cart_t *cart);

/// @brief removes a cart from the shop
/// @param shop containing carts
/// @param index of cart to remove
void ioopm_backend_cart_remove(ioopm_shop_t *shop, char *id);

/// @brief free memory allocated in shop
/// @param shop getting destroyed
void ioopm_shop_destroy(ioopm_shop_t *shop);

/// @brief adds a certain amount of merch to cart
/// @param wh warehouse where merch is stored
/// @param cart where merch should be put
/// @param merch_name name of merch being added
/// @param merch_amount amount of merch being added
void ioopm_backend_cart_add(ioopm_warehouse_t *wh, ioopm_cart_t *cart, char *merch_name, int *merch_amount_ptr, heap_t *heap);

/// @brief removes a certain amount of merch from cart
/// @param wh warehouse where merch is stored
/// @param cart to remove from
/// @param merch_name name of merch being removed
/// @param remove_amount amount of merch being removed
void ioopm_backend_remove_from_cart(ioopm_warehouse_t *wh, ioopm_cart_t *cart, char *merch_name, int remove_amount, heap_t *heap);

/// @brief checks out a cart
/// @param wh warehouse where merch is stored
/// @param shop including all carts
/// @param index of cart to check out
void ioopm_backend_cart_checkout(ioopm_warehouse_t *wh, ioopm_shop_t *shop, char *id);