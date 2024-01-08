#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "common.h"

/**
 * @file iterator.h
 * @author Johan Yrefors and William Forslund
 * @date 3 Okt 2022
 * @brief an interator to be used together with an underlying
 * list of type ioopm_linked_list
 */

typedef struct list ioopm_list_t;
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
/// @return a pointer to the next element. NULL if there is no next element
elem_t *ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// NOTE: REMOVE IS OPTIONAL TO IMPLEMENT
/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return a pointer to the removed element - the value pointed to will be overwritten
/// by the next successful remove operation, so will have to be copied if it is to be used later. 
/// Returns NULL if key is not in the underlying list
elem_t *ioopm_iterator_remove(ioopm_list_iterator_t *iter);

/// NOTE: INSERT IS OPTIONAL TO IMPLEMENT
/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element, heap_t *heap);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return a pointer to the current element. NULL if there is no such element
elem_t *ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);