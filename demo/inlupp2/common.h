#pragma once
#include <stdbool.h>
#include "utils.h"
#include "../../src/gc.h"

#define int_elem(x) (elem_t) { .integer=(x) }
#define ptr_elem(x) (elem_t) { .ptr= x }

/**
 * @file common.h
 * @author William Forslund, Gabriel Hemström
 * @date 1 Sep 2022
 * @brief file containing common functions for @file hash_table.c and @file linked_list.c
 *
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
 */

typedef union elem elem_t;
typedef struct hash_table ioopm_hash_table_t;
typedef struct link link_t;
typedef struct list ioopm_list_t;
/// Compares two elements and returns true if they are equal
typedef bool (*ioopm_eq_function)(elem_t a, elem_t b);

typedef struct shop ioopm_shop_t;
typedef struct merch ioopm_merch_t;
typedef struct shelf ioopm_shelf_t;
typedef struct cart ioopm_cart_t;

struct shop
{
  ioopm_hash_table_t *ht_carts;
  long num_of_carts;
};

struct cart
{
  char *id;
  ioopm_hash_table_t *ht;
  long total_price;
};

struct merch
{
  char *name;
  char *description;
  long price;
  ioopm_list_t *location;
  int total_stock;//for cart!
  int reserved_stock;
};

struct shelf
{
  char *shelf;
  int quantity;
};

union elem
{
  int int_val;
  int *int_ptr;
  unsigned int uint_val;
  char *string_val;
  float float_val;
  bool bool_val;
  ioopm_merch_t *merch;
  ioopm_shelf_t *shelf;
  ioopm_cart_t *cart;
  void *ptr;
  /// other choices certainly possible
};

struct link
{
  elem_t value;
  link_t *next;
};

struct list
{
  link_t *head;
  link_t *last;
  ioopm_eq_function eq; 
  size_t size;
};
