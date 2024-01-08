#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define No_Buckets 17

typedef union elem elem_t;
typedef struct entry entry_t;

static int modulo(int a, int b);
static entry_t *entry_create(elem_t key, elem_t value, entry_t *next, heap_t *heap);
static entry_t *find_previous_entry_for_key(const ioopm_hash_table_t *ht, entry_t *entry, elem_t key);
static bool key_equiv(const ioopm_hash_table_t *ht, elem_t key, elem_t value_ignored, void *x);
static bool key_geq(const ioopm_hash_table_t *ht, elem_t key, elem_t value_ignored, void *x);
static bool value_equiv(const ioopm_hash_table_t *ht, elem_t key_ignored, elem_t value, void *x);
static void replace_value(elem_t key_ignored, elem_t *value, void *arg);

struct entry
{
    elem_t key;
    elem_t value;
    entry_t *next;
};

struct hash_table
{
    entry_t *buckets[No_Buckets];
    ioopm_hash_function hash_fn;
    ioopm_eq_function eq_fn_keys;
    ioopm_eq_function geq_fn_keys;
    ioopm_eq_function eq_fn_values;
};

static int modulo(int a, int b)
{
    int c = a % b;
    while (c < 0)
    {
        c = c + b;
    }
    return c;
}

static entry_t *entry_create(elem_t key, elem_t value, entry_t *next, heap_t *heap)
{
    entry_t *new_entry = h_alloc_struct(heap, "***");
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = next;
    return new_entry;
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_fn, ioopm_eq_function eq_fn_keys, ioopm_eq_function geq_fn_keys, ioopm_eq_function eq_fn_values, heap_t *heap)
{
    ioopm_hash_table_t *ht = h_alloc_struct(heap, "17*4l");
    ht->hash_fn = hash_fn;
    ht->eq_fn_keys = eq_fn_keys;
    ht->geq_fn_keys = geq_fn_keys;
    ht->eq_fn_values = eq_fn_values;
    for (int i = 0; i < No_Buckets; i++)
    {
        ht->buckets[i] = entry_create((elem_t) { .ptr = NULL}, (elem_t) { .ptr = NULL}, NULL, heap);
    }
    return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
    // for(int i = 0; i < No_Buckets; ++i)
    // {
    //     free_entry(ht->buckets[i]);
    // }
    ht = NULL;
}

static entry_t *find_previous_entry_for_key(const ioopm_hash_table_t *ht, entry_t *entry, elem_t key)
{
    while(entry->next != NULL)
    {
        if(ht->geq_fn_keys(entry->next->key, key))
        {
            return entry;
        }
        else
        {
            entry = entry->next;
        }
    }
    return entry;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value, heap_t *heap)
{
  int bucket = ht->hash_fn(key);
  entry_t *entry = find_previous_entry_for_key(ht, ht->buckets[bucket], key);
  entry_t *next = entry->next;

  /// Check if the next entry should be updated or not
  if(next && ht->eq_fn_keys(next->key, key))
    {
      next->value = value;
    }
  else
    {
      entry->next = entry_create(key, value, next, heap);
    }
}

/* 
    We chose to return a pointer to the value because it allows
    a value in an entry to be 'NULL', and if there is no entry
    for 'key' we can return NULL
*/
elem_t *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, const elem_t key)
{
    int bucket = ht->hash_fn(key);
    entry_t *previous = find_previous_entry_for_key(ht, ht->buckets[bucket], key);
    entry_t *next = previous->next;

    if(next && ht->eq_fn_keys(next->key, key))
    {
        return &next->value;
    }
    return NULL;
}


/*
    We chose to return a pointer to the object removed, so that we can
    return a NULL pointer in case the key isn't in the hash table.
    Before removing the element, its value is stored in the bucket's sentinel link,
    and will be overwritten by the next successful remove operation.
*/
elem_t *ioopm_hash_table_remove(ioopm_hash_table_t *ht, const elem_t key)
{
    int bucket = ht->hash_fn(key);
    entry_t *previous = find_previous_entry_for_key(ht, ht->buckets[bucket], key);

    if(previous->next == NULL)
    {
        return NULL;
    }

    entry_t *to_remove = previous->next;

    if(ht->eq_fn_keys(to_remove->key, key))
    {
        previous->next = previous->next->next ? previous->next->next : NULL;

        //ht->buckets[bucket]->value = to_remove->value;
        to_remove = NULL;

        return NULL; //&ht->buckets[bucket]->value; //NOTE: vi har ändrat här för att inte spara gammla merch i inlupp2
    }
    return NULL;
}

size_t ioopm_hash_table_size(const ioopm_hash_table_t *ht)
{
    size_t size = 0;

    for(int i = 0; i < No_Buckets; ++i)
    {
        entry_t *entry = ht->buckets[i]->next;
        while(entry)
        {
            entry = entry->next;
            ++size;
        }
    }
    return size;
}

bool ioopm_hash_table_is_empty(const ioopm_hash_table_t *ht)
{
    for(int i = 0; i < No_Buckets; ++i)
    {
        entry_t *entry = ht->buckets[i]->next;

        if(entry) { return false; }
    }
    return true;
}

void ioopm_hash_table_clear(const ioopm_hash_table_t *ht)
{
    for(int i = 0; i < No_Buckets; ++i)
    {
        entry_t *entry = ht->buckets[i];

        if(entry->next != NULL)
        {
            //free_entry(entry->next);
            entry->next = NULL;
        }
    }
}

ioopm_list_t *ioopm_hash_table_keys(const ioopm_hash_table_t *ht, heap_t *heap)
{
    ioopm_list_t *keys = ioopm_linked_list_create(ht->eq_fn_keys, heap);
    for(int i = 0; i < No_Buckets; ++i)
    {
        entry_t *entry = ht->buckets[i];
        while(entry->next)
        {
            ioopm_linked_list_append(keys, entry->next->key, heap);
            entry = entry->next;
        }
    }
    return keys;
}

ioopm_list_t *ioopm_hash_table_values(const ioopm_hash_table_t *ht, heap_t *heap)
{
    ioopm_list_t *values = ioopm_linked_list_create(ht->eq_fn_values, heap);

    for(int i = 0; i < No_Buckets; ++i)
    {
        entry_t *entry = ht->buckets[i];
        while(entry->next)
        {
            ioopm_linked_list_append(values, entry->next->value, heap);
            entry = entry->next;
        }
    }
    return values;
}

bool ioopm_hash_table_all(const ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg, heap_t *heap)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht, heap);
    ioopm_list_t *values = ioopm_hash_table_values(ht, heap);
    bool result = true;

    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator(keys, heap);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator(values, heap);

    for(size_t i = 0; i < size; ++i)
    {
        if(!pred(ht, ioopm_iterator_current(iter_keys), ioopm_iterator_current(iter_values), arg, heap))
        {
            result = false;
            break;
        }
        ioopm_iterator_next(iter_values);
        ioopm_iterator_next(iter_keys);
    }
    ioopm_linked_list_destroy(keys);
    keys = NULL;
    ioopm_linked_list_destroy(values);
    values = NULL;
    ioopm_iterator_destroy(iter_keys);
    iter_keys = NULL;
    ioopm_iterator_destroy(iter_values);
    iter_values = NULL;
    return result;  
}

bool ioopm_hash_table_any(const ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg, heap_t *heap)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht, heap);
    ioopm_list_t *values = ioopm_hash_table_values(ht, heap);
    bool result = false;

    ioopm_list_iterator_t *iter_keys = ioopm_list_iterator(keys, heap);
    ioopm_list_iterator_t *iter_values = ioopm_list_iterator(values, heap);

    for(size_t i = 0; i < size; ++i)
    {
        if(pred(ht, ioopm_iterator_current(iter_keys), ioopm_iterator_current(iter_values), arg, heap))
        {
            result = true; 
            break;
        }
        ioopm_iterator_next(iter_keys);
        ioopm_iterator_next(iter_values);
    }
    ioopm_linked_list_destroy(keys);
    keys = NULL;
    ioopm_linked_list_destroy(values);
    values = NULL;
    ioopm_iterator_destroy(iter_keys);
    iter_keys = NULL;
    ioopm_iterator_destroy(iter_values);
    iter_values = NULL;
    return result;  
}

void ioopm_hash_table_apply_to_all(const ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
    for(int i = 0; i < No_Buckets; ++i)
    {
        entry_t *entry = ht->buckets[i];
        while(entry->next)
        {
            entry = entry->next;
            apply_fun(entry->key, &entry->value, arg);
        }
    }
}



/// ==== HASH FUNCTIONS ==== ///

int ioopm_hash_int(elem_t key)
{
    return modulo(key.int_val, No_Buckets);
}

int ioopm_hash_str(elem_t key)
{
    char *str = key.string_val;
    int sum = 0;
    do
    {
        sum = sum * 31 + *str;
        str++;
    } while (*str != '\0');
    return modulo(sum, No_Buckets);
}



// ==== Public functions made for testing with CUnit ==== //

bool ioopm_hash_table_has_key(const ioopm_hash_table_t *ht, elem_t key, heap_t *heap)
{
    return ioopm_hash_table_any(ht, key_equiv, &key, heap);
}

bool ioopm_hash_table_has_value(const ioopm_hash_table_t *ht, elem_t value, heap_t *heap)
{
    return ioopm_hash_table_any(ht, value_equiv, &value, heap);
}

void ioopm_hash_table_replace_all_values(const ioopm_hash_table_t *ht, elem_t elem)
{
    ioopm_hash_table_apply_to_all(ht, replace_value, &elem);
}

bool ioopm_hash_table_keys_geq(const ioopm_hash_table_t *ht, elem_t x, heap_t *heap)
{
    return ioopm_hash_table_all(ht, key_geq, &x, heap);
}



/// ==== EQ and GEQ functions for keys and values ==== ///

bool ioopm_int_geq(elem_t a, elem_t b)
{
    return a.int_val >= b.int_val;
}

bool ioopm_str_geq(elem_t a, elem_t b)
{
    if(strcmp(a.string_val, b.string_val) >= 0)
    {
        return true;
    }
    return false;
}

bool ioopm_int_eq(elem_t a, elem_t b)
{
    return a.int_val == b.int_val;
}

bool ioopm_str_eq(elem_t a, elem_t b)
{
    return strcmp(a.string_val, b.string_val) == 0 ? true : false;
}


static bool key_equiv(const ioopm_hash_table_t *ht, elem_t key, elem_t value_ignored, void *x)
{
    return ht->eq_fn_keys(key, *(elem_t *)x);
}

static bool key_geq(const ioopm_hash_table_t *ht, elem_t key, elem_t value_ignored, void *x)
{
    return ht->geq_fn_keys(key, *(elem_t *)x);
}

static bool value_equiv(const ioopm_hash_table_t *ht, elem_t key_ignored, elem_t value, void *x)
{
    return ht->eq_fn_values(value, *(elem_t *)x);
}



/// ==== Function added for testing ioopm_hash_table_apply_to_all ==== ///

static void replace_value(elem_t key_ignored, elem_t *value, void *arg)
{
    *value = *(elem_t *)arg;
}


