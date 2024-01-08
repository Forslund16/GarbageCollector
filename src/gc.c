#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "gc.h"
#include "heap.h"
#include "stack.h"

/// @brief Heap where user can store objects
struct heap
{
    internal_heap_t *heap;
    bool unsafe_stack;
    float gc_threshold;
    size_t total_size;
    bool display_error;
};

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
    heap_t *new_heap = calloc(1, sizeof(heap_t));
    new_heap->heap = heap_create(bytes);
    new_heap->total_size = heap_size(new_heap->heap);
    new_heap->gc_threshold = gc_threshold;
    new_heap->unsafe_stack = unsafe_stack;
    new_heap->display_error = true;
    return new_heap;
}

void h_delete(heap_t *h)
{
    heap_destroy(h->heap);
    free(h);
}

void h_delete_dbg(heap_t *h, void *dbg_value)
{
    heap_dbg(h->heap, dbg_value);
    free(h);
}

void gc_check(heap_t *h)
{
    if((h->total_size - h_avail(h)) >= (h->gc_threshold * h->total_size))
    {
        h_gc(h);
    }
}

void *h_alloc_struct(heap_t *h, char *layout)
{
    gc_check(h);
    void *pointer = alloc(h->heap, 0, layout);
    return pointer;
}

void *h_alloc_data(heap_t *h, size_t bytes)
{
    gc_check(h);
    void *pointer = alloc(h->heap, bytes, NULL);
    return pointer;
}

size_t h_gc(heap_t *h)
{
    size_t before = h_avail(h);
    stack_pointers_t *sp = NULL;
    sp = find_stack_pointers(h->heap);
    page_deactivate(h->heap, sp);
    compacting(h->heap, sp);
    size_t after = h_avail(h);
    size_t result = after - before;
    clear_stack_pointers(sp);
    if(result == 0 && h->display_error)
    {
        h->display_error = false;
        printf("WARNING, GC COULDN'T REMOVE ANY DATA, ADVISED TO INCREASE SIZE OF HEAP OR INCREASE THRESHOLD!\n");
    }
    printf("GC: Removed %ld bytes\n", result);
    return result;
}

size_t h_gc_dbg(heap_t *h, bool unsafe_stack)
{
    size_t removed = h_gc(h);
    h->unsafe_stack = unsafe_stack;
    return removed;
}

char *h_str_dup(heap_t *h, char *str)
{
    return string_duplicate(h->heap, str);
}

size_t h_avail(heap_t *h)
{
    return avail_memory(h->heap);
}

size_t h_used(heap_t *h)
{
    return used_memory(h->heap);
}