#pragma once
#include <stdbool.h>
#include "common.h"

/**
 * @file linked_list.h
 * @author Johan Yrefors and William Forslund
 * @date 3 Okt 2022
 * @brief a linked list that stores values of
 * 'elem_t' union type.
 * 
 * The current implementation allows this type to store values
 * of type int and char *
 */

typedef struct list ioopm_list_t;
typedef bool(*ioopm_int_predicate)(ioopm_list_t *list, int index, elem_t value, void *extra);
typedef void(*ioopm_apply_int_function)(int index, elem_t *value, void *extra);

/// @brief Creates a new empty list
/// @param eq_fun an equals function for the type of value in desired linked list
/// @return an empty linked list
ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_fun, heap_t *heap);

/// @brief Tear down the linked list and return all its memory (but not the memory of the elements)
/// @param list the list to be destroyed
void ioopm_linked_list_destroy(ioopm_list_t *list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_linked_list_append(ioopm_list_t *list, elem_t value, heap_t *heap);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended to
/// @param value the value to be prepended
void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value, heap_t *heap);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be inserted 
void ioopm_linked_list_insert(ioopm_list_t *list, const int index, elem_t value, heap_t *heap);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list
/// @param index the position in the list
/// @param valid pointer to a bool used to indicate if the index provided is valid or not
/// @return the value removed if the index is valid, otherwise 0
elem_t ioopm_linked_list_remove(ioopm_list_t *list, const int index, bool *valid);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return a pointer to the value at the given position
///         if index is valid, else NULL.
elem_t *ioopm_linked_list_get(const ioopm_list_t *list, const int index);

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param element the element sought
/// @return true if element is in the list, else false
bool ioopm_linked_list_contains(const ioopm_list_t *list, elem_t element);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_linked_list_size(const ioopm_list_t *list);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_linked_list_is_empty(const ioopm_list_t *list);

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ioopm_linked_list_clear(ioopm_list_t *list);

/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_int_predicate prop, void *extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_int_predicate prop, void *extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param fun the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of fun
void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_int_function fun, void *extra);


// ADDED PUBLIC FUNCTIONS FOR TESTING //

/// @brief Test if all values in list equals the value of extra
/// @param list the linked list
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if all values equal extra, else false
bool ioopm_linked_list_all_elements_eq(ioopm_list_t *list, elem_t extra);

/// @brief Test if some value in list equals the value of extra
/// @param list the linked list
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if at least one value equals extra, else false
bool ioopm_linked_list_some_element_eq(ioopm_list_t *list, int extra);

/// @brief adds a given value to each value in the list
/// @param list the linked list operated upon
/// @param the value to be added
void ioopm_linked_list_add_to_all(ioopm_list_t *list, void *extra);

// EQ_FUNCTIONS
//bool ioopm_element_eq(ioopm_list_t *list, int index_ignored, elem_t element, void *arg);

/// @brief checks if two elements are equal
/// @param a is interpreted as an int and gets compared
/// @param b is interpreted as an int and gets compared
/// @return true if a == b, else false
bool ioopm_int_eq(elem_t a, elem_t b);

/// @brief checks if two elements are equal
/// @param a is interpreted as an unsigned int and gets compared
/// @param b is interpreted as an unsigned int and gets compared
/// @return true if a == b, else false
bool ioopm_uint_eq(elem_t a, elem_t b);

/// @brief checks if two elements are equal
/// @param a is interpreted as a float number and gets compared
/// @param b is interpreted as a float number and gets compared
/// @return true if a == b, else false
bool ioopm_float_eq(elem_t a, elem_t b);

/// @brief checks if two elements are equal
/// @param a is interpreted as a string and gets compared
/// @param b is interpreted as a string and gets compared
/// @return true if a == b, else false
bool ioopm_str_eq(elem_t a, elem_t b);

/// @brief checks if two elements are equal
/// @param a is interpreted as a boolean and gets compared
/// @param b is interpreted as a boolean and gets compared
/// @return true if a == b, else false
bool ioopm_bool_eq(elem_t a, elem_t b);


//for freq_count
// elem_t *ioopm_linked_list_to_array(ioopm_list_t *list);

/// @brief converts a linked list to an array
/// @param list the linked list getting converted
/// @return an array in the same order as teh linked list
char **ioopm_linked_list_to_array(ioopm_list_t *list, heap_t *heap);