/*
    Private header file for sharing common
    definitions between multiple .c files
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdbool.h>
#include "../../src/gc.h"

typedef struct link link_t;
typedef union elem elem_t;

union elem
{
    int int_val;
    unsigned int uint_val;
    bool bool_val;
    float float_val;
    char *string_val;
    void *ptr;
};

/// Compares two elements and returns true if they are equal
typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);
typedef int(*ioopm_hash_function)(elem_t key);

struct link
{
    elem_t element;
    link_t *next;
};

struct list
{
    size_t size;
    link_t *first;
    link_t *last;
    ioopm_eq_function eq_fn;
};

#endif