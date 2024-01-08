#pragma once
#include "linked_list.h"
#include "common.h"
#include <stdbool.h>
#include "../../src/gc.h"

/** 
 * @file iterator.h
 * @author Per Jutterström, Gabriel Hemström
 * @date 1 Sep 2022
 * @brief Iterator for a linked list
 *
 * @see https://uppsala.instructure.com/courses/68435/assignments/130155
 */

typedef struct iter ioopm_list_iterator_t;

/// @brief Create an iterator for a given list
/// @param list the list to be iterated over
/// @return an iteration positioned at the start of list
ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list, heap_t *heap);

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is at least one more element 
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step
/// @param iter the iterator
/// @return the value of the next element
elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the value of the current element
elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);