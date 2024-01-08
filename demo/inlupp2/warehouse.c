#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "iterator.h"
#include "hash_table.h"
#include "common.h"
#include "warehouse.h"
#include "ui_utils.h"

free_merch(ioopm_merch_t *merch)
{
    merch->description = NULL;
    merch->name = NULL;
    ioopm_linked_list_clear(merch->location);
    merch->location = NULL;
    merch = NULL;
}

void ioopm_warehouse_destroy(ioopm_warehouse_t *wh)
{
    ioopm_hash_table_destroy(wh->ht_items);
    ioopm_hash_table_destroy(wh->ht_shelfs);
    wh = NULL;
}

ioopm_warehouse_t *ioopm_warehouse_create(heap_t *heap)
{
    ioopm_warehouse_t *wh = h_alloc_struct(heap, "**");
    wh->ht_items = ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_int_eq, heap);
    wh->ht_shelfs = ioopm_hash_table_create(ioopm_hash_str, ioopm_str_eq, ioopm_str_geq, ioopm_str_eq, heap);
    return wh;
}

ioopm_shelf_t *create_shelf(char *shelf_id, int stock, heap_t *heap)
{
    ioopm_shelf_t *shelf = h_alloc_struct(heap, "*i");
    shelf->shelf = h_str_dup(heap, shelf_id);
    shelf->quantity = stock;
    return shelf;
}

ioopm_merch_t *ioopm_merch_create(char *name, char *desc, int price, ioopm_list_t *loc, heap_t *heap)
{
    ioopm_merch_t *merch = h_alloc_struct(heap, "**l*ii");

    merch->name = name;//h_str_dup(heap, name);
    merch->description = name;//h_str_dup(heap, desc);
    merch->price = price;
    merch->location = loc;
    merch->total_stock = 0;
    merch->reserved_stock = 0;
    return merch;
}

void ioopm_backend_add(ioopm_hash_table_t *ht_items, char *name, ioopm_merch_t *new_merch, heap_t *heap)
{
    ioopm_list_t *list = ioopm_linked_list_create(ioopm_int_eq, heap);
    ioopm_hash_table_insert(ht_items, (elem_t)new_merch->name, (elem_t)new_merch, heap);
    new_merch->location = list;
}

bool shelf_equiv(elem_t elem, void *x)
{
    return strcmp(elem.shelf->shelf, (char *)x) == 0;
}

void ioopm_backend_edit(ioopm_warehouse_t *wh, char *name, char *new_name, char *new_desc, int new_price, heap_t *heap)
{
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t)name)->merch;
    elem_t *elem = ioopm_hash_table_remove(wh->ht_items, (elem_t)name);
    elem = NULL;

    ioopm_merch_t *new_merch = ioopm_merch_create(new_name, new_desc, new_price, merch->location, heap);
    ioopm_hash_table_insert(wh->ht_items, (elem_t)new_name, (elem_t)new_merch, heap);

    new_merch->total_stock = merch->total_stock;
    new_merch->reserved_stock = merch->reserved_stock;

    ioopm_list_iterator_t *iter = ioopm_list_iterator(merch->location, heap);

    if (ioopm_linked_list_size(merch->location) != 0)
    {
        ioopm_shelf_t *current_shelf = ioopm_iterator_current(iter).shelf;
        ioopm_hash_table_insert(wh->ht_shelfs, (elem_t)current_shelf->shelf, (elem_t)new_name, heap);
        while (ioopm_iterator_has_next(iter))
        {
            ioopm_shelf_t *next_shelf = ioopm_iterator_next(iter).shelf;
            ioopm_hash_table_insert(wh->ht_shelfs, (elem_t)next_shelf->shelf, (elem_t)new_name, heap);
        }
    }
    name = NULL;
    merch->description = NULL;
    merch->name = NULL;
    free_merch(merch);
    merch = NULL;
    ioopm_iterator_destroy(iter);
}

void ioopm_backend_replenish(ioopm_warehouse_t *wh, char *name, char *shelf, int stock, heap_t *heap)
{
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t)name)->merch;
    merch->total_stock += stock;

    if (!ioopm_linked_list_any(merch->location, shelf_equiv, shelf)) // om hyllan inte redan finns
    {
        ioopm_shelf_t *new_shelf = create_shelf(shelf, stock, heap);
        ioopm_linked_list_append(merch->location, (elem_t)new_shelf, heap);
        ioopm_hash_table_insert(wh->ht_shelfs, (elem_t)shelf, (elem_t)merch->name, heap);
        return;
    }
    ioopm_list_iterator_t *iter = ioopm_list_iterator(merch->location, heap);

    if (strcmp(shelf, ioopm_iterator_current(iter).shelf->shelf) == 0)
    {
        ioopm_iterator_current(iter).shelf->quantity += stock;
    }

    while (ioopm_iterator_has_next(iter))
    {
        ioopm_shelf_t *next_shelf = ioopm_iterator_next(iter).shelf;
        if (strcmp(shelf, next_shelf->shelf) == 0)
        {
            next_shelf->quantity += stock;
        }
    }
    ioopm_iterator_destroy(iter);
}

static int shelfcmp(const void *p1, const void *p2)
{
    ioopm_shelf_t *shelf1 = *(ioopm_shelf_t *const *)p1;
    ioopm_shelf_t *shelf2 = *(ioopm_shelf_t *const *)p2;
    return strcmp(shelf1->shelf, shelf2->shelf);
}

void ioopm_backend_show_stock(ioopm_hash_table_t *ht_items, char *merch_name, heap_t *heap)
{
    ioopm_list_t *shelf_list = ioopm_hash_table_lookup(ht_items, (elem_t)merch_name)->merch->location;
    size_t size = ioopm_linked_list_size(shelf_list);

    if (size == 0)
    {
        printf("Merch not in stock\n");
        return;
    }

    elem_t *shelf_array = ioopm_linked_list_to_array(shelf_list, heap);
    qsort(shelf_array, size, sizeof(ioopm_shelf_t *), shelfcmp);
    printf("Shelf | Stock\n");
    for (size_t i = 0; i < size; i++)
    {
        printf(" %s  | %d\n", shelf_array[i].shelf->shelf, shelf_array[i].shelf->quantity);
    }

}

void ioopm_backend_remove(ioopm_warehouse_t *wh, char *name, heap_t *heap)
{
    ioopm_merch_t *merch = ioopm_hash_table_lookup(wh->ht_items, (elem_t)name)->merch;
    size_t size = ioopm_linked_list_size(merch->location);
    if (size != 0)
    {
        ioopm_list_iterator_t *iter = ioopm_list_iterator(merch->location, heap);
        ioopm_shelf_t *current_shelf = ioopm_iterator_current(iter).shelf;
        elem_t *elem = ioopm_hash_table_remove(wh->ht_shelfs, (elem_t)current_shelf->shelf);
        elem = NULL;
        while (ioopm_iterator_has_next(iter))
        {
            ioopm_shelf_t *next_shelf = ioopm_iterator_next(iter).shelf;
            elem_t *elem2 = ioopm_hash_table_remove(wh->ht_shelfs, (elem_t)next_shelf->shelf);
            elem2 = NULL;
        }
        ioopm_iterator_destroy(iter);
    }
    elem_t *elem3 = ioopm_hash_table_remove(wh->ht_items, (elem_t)name);
    elem3 = NULL;
    free_merch(merch);
    merch = NULL;
}
