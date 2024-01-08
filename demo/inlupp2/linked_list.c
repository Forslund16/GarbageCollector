#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "linked_list.h"
#include "iterator.h"


struct iter
{
  link_t *current;
  ioopm_list_t *list;
};

// check if a value is even
bool ioopm_is_even_value(elem_t value, void *extra)
{ // ? into common.h?
  return value.int_val % 2 == 0;
}

// increment a value
void ioopm_increment_value(elem_t *value, void *extra)
{ // ? into common.h?
  value->int_val += 1;
}

// compare keys
static bool compare_keys(elem_t key, void *extra)
{ // ? into common.h?
  return key.int_val == *(int *)extra;
}

// create a new list
ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function compare, heap_t *heap)
{
  ioopm_list_t *list = h_alloc_struct(heap, "**ll");
  list -> head = NULL;
  list -> last = NULL;
  list -> eq = compare;
  list -> size = 0;
  return list;
}

// destroy a list
void ioopm_linked_list_destroy(ioopm_list_t *list)
{
  ioopm_linked_list_clear(list);
  // free(list);
  list = NULL;
}

// create a new link with given value
static link_t *create_link(elem_t value, heap_t *heap)
{
  link_t *link = h_alloc_struct(heap, "**");
  link->value = value;
  link->next = NULL; // make link as own function
  return link;
}

// insert a new element at the end of the list
void ioopm_linked_list_append(ioopm_list_t *list, elem_t value, heap_t *heap)
{
  link_t *index = list->head;
  link_t *new_link = create_link(value, heap); // create new link
  if (index == NULL)                     // if empty list
  {
    list->head = new_link; // update head ptr
    list->last = new_link; // update last ptr
    list->size++;          // increment size
  }
  else
  {
    list->last->next = new_link; // append new link
    list->last = new_link;       // update last ptr
    list->size++;                // increment size
  }
}

// insert a new element at the beginning of the list
void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value, heap_t *heap)
{
  link_t *new_link = create_link(value, heap); // create new link
  if (list->head == NULL)                // if empty list
  {
    list->head = new_link; // update head ptr
    list->last = new_link; // update last ptr
    list->size++;          // increment size
  }
  else
  {
    link_t *first = list->head; // tmp ptr to first link
    list->head = new_link;      // update head ptr
    new_link->next = first;     // new link points to old first link
    list->size++;               // increment size
  }
}

// insert an element at the given index
void ioopm_linked_list_insert(ioopm_list_t *list, elem_t index, elem_t value, heap_t *heap)
{
  assert(list->size >= index.int_val); // check valid index
  if (index.int_val == 0)
  {
    ioopm_linked_list_prepend(list, value, heap);
    return;
  }
  else if (index.int_val== list->size)
  {
    ioopm_linked_list_append(list, value, heap);
    return;
  }
  else
  {
    link_t *current = list->head;
    link_t *new_link = create_link(value, heap);
    for (size_t i = 1; i < index.int_val; i++)
    {
      current = current->next; // find link index
    }
    link_t *tmp = current->next; // tmp ptr to next link
    current->next = new_link;    // update next ptr
    new_link->next = tmp;        // new link points to next link
    list->size++;                // increment size
  }
}

// remove an element from the list
elem_t ioopm_linked_list_remove(ioopm_list_t *list, elem_t index)
{
  assert(list->size > index.int_val); // check valid index
  if (index.int_val == 0)
  {
    link_t *tmp = list->head;  // tmp ptr to first link
    list->head = tmp->next;    // update head ptr
    elem_t value = tmp->value; // save value of removed element
    tmp = NULL;
    //free(tmp);                 // free link
    list->size--;
    return value;
  }
  link_t *prev = list->head;
  link_t *current = prev->next;
  for (size_t i = 1; i < index.int_val; i++)
  {
    prev = current;
    current = current->next; // find link index
  }
  if (current->next == NULL)
  {
    list->last = prev; // update last pointer
  }
  prev->next = current->next;    // update next ptr
  elem_t value = current->value; // save value of removed element
  //free(current); 
  current = NULL;                // free link
  list->size--;                  // decrement size
  return value;
}

// get the value at the given index
elem_t ioopm_linked_list_get(ioopm_list_t *list, elem_t index)
{
  assert(list->size > index.int_val); // check valid index
  link_t *current = list->head;
  for (size_t i = 0; i < index.int_val; i++)
  {
    current = current->next; // find link index
  }
  return current->value;
}

// check if the list contains the given element
bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
{
  return ioopm_linked_list_any(list, compare_keys, &element);
}

// get size of list
size_t ioopm_linked_list_size(ioopm_list_t *list)
{
  return list->size;
}

// check if list is empty
bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
  return list->size == 0;
}

// clear the list
void ioopm_linked_list_clear(ioopm_list_t *list)
{
  link_t *current = list->head;
  while (current != NULL)
  {
    link_t *tmp = current;
    current = current->next;
    tmp = NULL;
    //free(tmp);
  }
  list->head = NULL;
  list->last = NULL;
  list->size = 0;
}

// check if all elements in list hold a predicate
bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_int_predicate pred, void *x)
{
  link_t *current = list->head;
  while (current != NULL)
  {
    if (!pred(current->value, x))
    {
      return false;
    }
    current = current->next;
  }
  return true;
}

// check if any element in list holds a predicate
bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_int_predicate pred, void *x)
{
  link_t *current = list->head;
  while (current != NULL)
  {
    if (pred(current->value, x))
    {
      return true;
    }
    current = current->next;
  }
  return false;
}

// apply a function to all elements in list
void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_int_function fun, void *x)
{
  link_t *current = list->head;
  while (current != NULL)
  {
    fun(&current->value, x);
    current = current->next;
  }
}

elem_t *ioopm_linked_list_to_array(ioopm_list_t *list, heap_t *heap)
{
    elem_t *array = h_alloc_data(heap, ioopm_linked_list_size(list) * sizeof(elem_t));    //calloc(ioopm_linked_list_size(list), sizeof(elem_t));
    link_t *current = list->head;
    int i = 0;
    while(current)
    {
        array[i] = current->value;
        current = current->next;
        i++;
    }
    return array;
}

//ITERATOR

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list, heap_t *heap)
{
  ioopm_list_iterator_t *iter = h_alloc_struct(heap, "**");
  iter->current = list->head;
  iter->list = list;
  return iter;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
  return iter->current->next != NULL;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
  //assert(ioopm_iterator_has_next(iter)); // check iterator has next
  if(ioopm_iterator_has_next(iter)){
    iter->current = iter->current->next;
    elem_t value = iter->current->value;
    return value;
  }
  return (elem_t) NULL;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
  iter->current = iter->list->head;
}

elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
  return iter->current->value;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
  iter = NULL;
}
