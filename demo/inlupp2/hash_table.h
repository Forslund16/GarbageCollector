#pragma once

#include "linked_list.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../src/gc.h"

/**
 * @file hash_table.h
 * @author Johan Yrefors and William Forslund
 * @date 3 Okt 2022
 * @brief Simple hash table that maps keys to values.
 *
 * Keys and values are of 'elem_t' union type. the current implementation
 * allows these to store values of type int and char *
 *
 * The hash table only has ownership of the memory it allocates itself,
 * and destroying the hash table only de-allocates the memory created by
 * the hash table itself.
 *
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
 */

typedef struct hash_table ioopm_hash_table_t;
/*
    Vi har definierat om typen ioopm_predicate för att kunna skicka med en pekare till hash table-strukten.
    Detta för att komma åt rätt eq-, eller geq-funktion för den aktuella datatypen
 */
typedef bool (*ioopm_predicate)(const ioopm_hash_table_t *ht, elem_t key, elem_t value, void *extra, heap_t *heap);
typedef void (*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);
typedef int (*ioopm_hash_function)(elem_t key);



/// @brief Create a new hash table
/// @param hash_fn a hash function with the type of key in desired hash table
/// @param eq_fn_keys an equals function for the type of key in desired hash table
/// @param geq_fn_keys a function checking >= for the type of key in desired hash table
/// @param eq_fn_values an equals function for the type of value in desired hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_fn, ioopm_eq_function eq_fn_keys, ioopm_eq_function geq_fn_keys, ioopm_eq_function eq_fn_values, heap_t *heap);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value, heap_t *heap);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return a pointer to the value mapped to by key. NULL if key is not in hash table
elem_t *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return a pointer to the value of ht[key] - the value pointed to will be overwritten
/// by the next successful remove operation, so will have to be copied if it is to be used later.
/// Returns NULL if key is not in hash table
elem_t *ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table in O(n) time (n = number of entries)
/// @param h hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(const ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty in O(n) time (n = number of buckets in the hash table)
/// @param h hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty(const ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
void ioopm_hash_table_clear(const ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return an array of keys for hash table h
/// @attention the returned array is allocated on the heap and it is up the user to free the memory.
ioopm_list_t *ioopm_hash_table_keys(const ioopm_hash_table_t *ht, heap_t *heap);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param h hash table operated upon
/// @return an array of values for hash table h
/// @attention the returned array is allocated on the heap and it is up the user to free the memory
ioopm_list_t *ioopm_hash_table_values(const ioopm_hash_table_t *ht, heap_t *heap);

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
/// @return true if the key is in the hash table - false otherwise
bool ioopm_hash_table_has_key(const ioopm_hash_table_t *ht, elem_t key, heap_t *heap);

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
/// @return true if value is in ht, else false
bool ioopm_hash_table_has_value(const ioopm_hash_table_t *ht, elem_t value, heap_t *heap);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if pred holds for every element in ht, else false
bool ioopm_hash_table_all(const ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg, heap_t *heap);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @return true if pred holds for any element in ht, else false
bool ioopm_hash_table_any(const ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg, heap_t *heap);

/// @brief apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(const ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg);

// ADDED PUBLIC FUNCTIONS //
/// @brief check if all keys in the hash table are less than a given value
/// @param x the value to compare the keys to
/// @return true if (all keys in ht) >= x, else false
bool ioopm_hash_table_keys_geq(const ioopm_hash_table_t *ht, elem_t x, heap_t *heap);

/// @brief replace all values in the hash table with a given value
/// @param ht hash table operated upon
/// @param str the to insert
void ioopm_hash_table_replace_all_values(const ioopm_hash_table_t *ht, elem_t value);

/// @brief hash function for ints
/// @param key is interpreted as an int which gets hashed
/// @return key % (the number of buckets in desired hash table)
int ioopm_hash_int(elem_t key);

/// @brief hash functions for strings
/// @param key is interpreted as a string which gets hashed
/// @return the hashed value of a string
int ioopm_hash_str(elem_t key);

/// @brief checks if a >= b
/// @param a is interpreted as an int and is used to compare
/// @param b is interpreted as an int and is used to compare
/// @return true if a >= b, else false
bool ioopm_int_geq(elem_t a, elem_t b);

/// @brief checks if a >= b
/// @param a is interpreted as a string and is used to compare
/// @param b is interpreted as a string and is used to compare
/// @return true if a >= b, else false
bool ioopm_str_geq(elem_t a, elem_t b);

/// @brief checks if a == b
/// @param a is interpreted as an int and is used to compare
/// @param b is interpreted as an int and is used to compare
/// @return true if a == b, else false
bool ioopm_str_eq(elem_t a, elem_t b);

/// @brief checks if a == b
bool ioopm_int_eq(elem_t a, elem_t b);