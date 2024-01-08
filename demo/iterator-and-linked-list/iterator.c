#include "iterator.h"
#include "common.h"

struct iter
{
    link_t *current;
    ioopm_list_t *list;
};

static link_t *link_create(elem_t elem, link_t *next, heap_t *heap)
{
    link_t *link = h_alloc_struct(heap, "l*");
    link->element = elem;
    link->next = next;
    return link;
}

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list, heap_t *heap)
{
    ioopm_list_iterator_t *iter = h_alloc_struct(heap, "**");

    iter->current = list->first;
    iter->list = list;

    return iter;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return iter->current->next != NULL;
}

elem_t *ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
    if(ioopm_iterator_has_next(iter))
    {
        iter->current = iter->current->next;
        return &iter->current->element;
    }
    return NULL;
}

elem_t *ioopm_iterator_remove(ioopm_list_iterator_t *iter)
{
    if(!ioopm_iterator_has_next(iter))
    {
        return NULL;
    }
    link_t *to_remove = iter->current->next;
    iter->list->first->element = to_remove->element; // Store the removed element in the sentinel link

    iter->current->next = to_remove->next;

    //free(to_remove);
    iter->list->size--;
    
    return &iter->list->first->element;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element, heap_t *heap)
{
    iter->current->next = link_create(element, iter->current->next, heap);
    iter->list->size++;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    iter->current = iter->list->first;
}

elem_t *ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    if(!ioopm_iterator_has_next(iter))
    {
        return NULL;
    }
    return &iter->current->next->element;
}