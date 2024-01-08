#pragma once
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "../../src/gc.h"

/**
 * @file warehouse.h
 * @author William Forslund, Gabriel Hemström
 * @date 10 Okt 2022
 * @brief functions for the warehouse
 */
typedef struct warehouse ioopm_warehouse_t;

struct warehouse
{
    ioopm_hash_table_t *ht_items;
    ioopm_hash_table_t *ht_shelfs;
};

/*
    All functions in this file requires inputs to be valid
*/

/// @brief Frees all memory allocated in wh
/// @param wh a warehouse containing two hash tables
void ioopm_warehouse_destroy(ioopm_warehouse_t *wh);

/// @brief creates a warehouse
/// @return a pointer to a warehouse struct
ioopm_warehouse_t *ioopm_warehouse_create(heap_t *heap);

/// @brief creates a merch
/// @param name of merch
/// @param desc(ription) of merch
/// @param price of merch
/// @param loc(ation) of the list of shelfs
/// @return a pointer to the merch containing name, desc, price and loc
ioopm_merch_t *ioopm_merch_create(char *name, char *desc, int price, ioopm_list_t *loc, heap_t *heap);

/// @brief adds merch to database
/// @param ht_items hash table mapping names of merch to its contents
/// @param name of merch
/// @param new_merch merch intended to be added
void ioopm_backend_add(ioopm_hash_table_t *ht_items, char *name, ioopm_merch_t *new_merch, heap_t *heap);

/// @brief changes name, description and price of merch
/// @param wh warehouse
/// @param name of merch to edit
/// @param new_name new name of merch
/// @param new_desc new description of merch
/// @param new_price new price of merch
void ioopm_backend_edit(ioopm_warehouse_t *wh, char *name, char *new_name, char *new_desc, int new_price, heap_t *heap);

/// @brief adds stock to merch
/// @param wh warehouse
/// @param name of merch to add stock to
/// @param shelf name of shelf
/// @param stock amount of stock to be added
void ioopm_backend_replenish(ioopm_warehouse_t *wh, char *name, char *shelf, int stock, heap_t *heap);

/// @brief prints stock of merch
/// @param ht_items hash table mapping names of merch to its contents
/// @param merch_name name of merch whos stock is to be shown
void ioopm_backend_show_stock(ioopm_hash_table_t *ht_items, char *merch_name, heap_t *heap);

/// @brief removes merch from warehouse
/// @param wh warehouse
/// @param name of merch to remove
void ioopm_backend_remove(ioopm_warehouse_t *wh, char *name, heap_t *heap);






