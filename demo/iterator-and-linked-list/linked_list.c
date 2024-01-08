#include "linked_list.h"
#include "iterator.h"
#include "common.h"
#include <stdlib.h>
#include <string.h>

static link_t *find_previous_link_with_index(const ioopm_list_t *list, const int index);
static bool element_eq(ioopm_list_t *list, int index_ignored, elem_t element, void *arg);
static void element_add(int index_ignored, elem_t *element, void *extra);

static link_t *link_create(elem_t elem, link_t *next, heap_t *heap)
{
    link_t *link = h_alloc_struct(heap, "l*");
    link->element = elem;
    link->next = next;
    return link;
}

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_fn, heap_t *heap)
{
    ioopm_list_t *list = h_alloc_struct(heap, "l**l");

    list->first = link_create((elem_t) { .ptr = NULL }, NULL, heap);
    list->last = list->first;
    list->eq_fn = eq_fn;
    return list;
}


// static void free_links(link_t *link)
// {
//     if(link->next != NULL)
//     {
//         free_links(link->next);
//     }
//     free(link);
//     link = NULL;
// }

// void ioopm_linked_list_destroy(ioopm_list_t *list)
// {
//     free_links(list->first);
//     free(list);
//     list = NULL;
// }

void ioopm_linked_list_append(ioopm_list_t *list, elem_t element, heap_t *heap)
{
    link_t *new_link = link_create(element, NULL, heap);
    list->last->next = new_link;
    list->last = new_link;
    list->size += 1;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t element, heap_t *heap)
{
    link_t *new_link = link_create(element, list->first->next, heap);
    list->first->next = new_link;
    if (list->first == list->last)
    {
        list->last = new_link;
    }
    list->size += 1;
}

void ioopm_linked_list_insert(ioopm_list_t *list, const int index, elem_t element, heap_t *heap)
{
    link_t *prev = list->first;
    int cursor = 0;
    while(cursor < index && prev)
    {
        ++cursor;
        prev = prev->next;
    }
    prev->next = link_create(element, prev->next, heap);
    list->size += 1;
}

static link_t *find_previous_link_with_index(const ioopm_list_t *list, const int index)
{
    link_t *prev_link = list->first;
    for (int k = 0; k < index; k++)
    {
        prev_link = prev_link->next;
    }
    return prev_link;
}

elem_t ioopm_linked_list_remove(ioopm_list_t *list, const int index, bool *valid)
{
    *valid = true;
    if(index < 0 || index > (int) ioopm_linked_list_size(list)-1)
    {
        *valid = false;
        return (elem_t) { .ptr = NULL };
    }
    link_t *link = find_previous_link_with_index(list, index);
    link_t *to_remove = link->next;
    elem_t removed = to_remove->element;
    if (to_remove->next != NULL)
    {
        link->next = to_remove->next;
    }
    else 
    {
        list->last = link;
        link->next = NULL;
    }
    //free(to_remove);
    to_remove = NULL;
    list->size -= 1;
    return removed;
}

void ioopm_linked_list_clear(ioopm_list_t *list)
{
    if(!ioopm_linked_list_is_empty(list))
    {
        //free_links(list->first->next);
        list->first->next = NULL;
        list ->size = 0;
        if(ioopm_linked_list_is_empty(list))
        {
            list->last = list->first;
        }
    }
}

elem_t *ioopm_linked_list_get(const ioopm_list_t *list, const int index)
{
    if(index < 0 || index > (int) ioopm_linked_list_size(list)-1)
    {
        return NULL;
    }
    link_t *link = find_previous_link_with_index(list, index);
    return &link->next->element;
}

bool ioopm_linked_list_contains(const ioopm_list_t *list, elem_t element)
{
    link_t *current = list->first->next;
    while(current != NULL)
    {
        if (list->eq_fn(current->element, element))
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

size_t ioopm_linked_list_size(const ioopm_list_t *list)
{
    return list->size;
}

bool ioopm_linked_list_is_empty(const ioopm_list_t *list)
{
    if (ioopm_linked_list_size(list) == 0)
    {
        return true;
    }
    return false;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_int_predicate prop, void *extra)
{
    link_t *current = list->first->next;
    int index = 0;
    while(current->next != NULL)
    {
        if(!prop(list, index, current->element, extra))
        {
            return false;
        }
        current = current->next;
        index += 1;
    }
    return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_int_predicate prop, void *extra)
{
    link_t *current = list->first->next;
    int index = 0;
    while(current->next != NULL)
    {
        if(prop(list, index, current->element, extra))
        {
            return true;
        }
        current = current->next;
        index += 1;
    }
    return false;
}

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_int_function fun, void *extra)
{
    link_t *link = list->first;
    int index = 0;
    while(link->next)
    {
        link = link->next;
        fun(index, &link->element, extra);
        index++;
    }
}

char **ioopm_linked_list_to_array(ioopm_list_t *list, heap_t *heap)
{
    char **array = h_alloc_data(heap, ioopm_linked_list_size(list) * sizeof(elem_t));
    link_t *current = list->first->next;
    int i = 0;
    while(current)
    {
        array[i] = current->element.string_val;
        current = current->next;
        i++;
    }
    return array;
}



bool ioopm_linked_list_all_elements_eq(ioopm_list_t *list, elem_t extra)//for testing
{
    return ioopm_linked_list_all(list, element_eq, &extra);
}

bool ioopm_linked_list_some_element_eq(ioopm_list_t *list, int extra)//for testing
{
    return ioopm_linked_list_any(list, element_eq, &extra);
}

void ioopm_linked_list_add_to_all(ioopm_list_t *list, void *extra)//for testing
{
    ioopm_linked_list_apply_to_all(list, element_add, extra);
} 


// COMPARISON FUNCTIONS all behövs inte längre

static bool element_eq(ioopm_list_t *list, int index_ignored, elem_t element, void *arg)
{
    return list->eq_fn(element, *(elem_t *)arg);
}

static void element_add(int index_ignored, elem_t *element, void *extra)
{
    element->int_val += *((int *)extra);
}

bool ioopm_int_eq(elem_t a, elem_t b)
{
    return a.int_val == b.int_val;
}

bool ioopm_uint_eq(elem_t a, elem_t b)
{
    return a.uint_val == b.uint_val;
}

bool ioopm_float_eq(elem_t a, elem_t b)
{
    return a.float_val == b.float_val;
}

bool ioopm_str_eq(elem_t a, elem_t b)
{
    return strcmp(a.string_val, b.string_val) == 0 ? true : false;
}

bool ioopm_bool_eq(elem_t a, elem_t b)
{
    return a.bool_val == b.bool_val;
}
