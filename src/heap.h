#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "layout.h"
#include "stack.h"

typedef struct page page_t;
typedef struct internal_heap internal_heap_t;
typedef struct stack_pointers stack_pointers_t;

/// @brief creates a heap
/// @param bytes Size of desired heap
/// @return a heap
internal_heap_t *heap_create(size_t bytes);

/// @brief retrieve the inital size of a internal_heap_t
/// @param heap the internal_heap_t to retrieve from
/// @return intitial size in size_t
size_t heap_size(internal_heap_t *heap);

/// @brief destroys a heap
/// @param heap Heap to destroy
void heap_destroy(internal_heap_t *heap);

/// @brief Delete a heap and trace, killing off stack pointers.
/// @param heap Heap to destroy
/// @param dbg_value a value to be written into every pointer into h on the stack
void heap_dbg(internal_heap_t *heap, void *dbg_value);

/// @brief Checks if a pointer is valid
/// @param page Page where pointer is located
/// @param pointer Pointer to check
/// @return true if something is allocated on pointers adress, else false
bool alloc_map_check(page_t *page, void *pointer);

/// @brief Set bool at index in alloc_map to true
/// @param alloc_map Map of allocations
/// @param index Index where alloc_map should be updated
void alloc_map_set_true(page_t *page, void *pointer);

/// @brief Allocates object on heap
///
/// Valid characters in format strings are:
/// - 'i' -- for sizeof(int) bytes 'raw' data
/// - 'l' -- for sizeof(long) bytes 'raw' data
/// - 'f' -- for sizeof(float) bytes 'raw' data
/// - 'c' -- for sizeof(char) bytes 'raw' data
/// - 'd' -- for sizeof(double) bytes 'raw' data
/// - '*' -- for a sizeof(void *) bytes pointer value
/// - '\0' -- null-character terminates the format string
///
/// @param heap Heap to allocate on
/// @param bytes 0 if user wants to allocate struct, else it is amount of bytes to allocate
/// @param layout the format string
/// @return a pointer to the allocated object on the heap. Else if input is invalid then NULL
void *alloc(internal_heap_t *heap, size_t bytes, char *layout);

/// @brief Checks if 'pointer' points to any active page in 'heap'
/// and is valid (according to alloc_map_check).
/// @param heap
/// @param pointer
/// @return true if pointer is safe/valid, false otherwise.
bool page_iterator(internal_heap_t *heap, void *pointer);

/// @brief Deactivates pages without living objects
/// @param heap Heap where objects are stored
/// @param pointer_array Array of pointers into the heap
void page_deactivate(internal_heap_t *heap, stack_pointers_t *sp);

/// @brief Compacts all objects reachable from stack pointers  
/// @param heap Heap where objects are stored
/// @param sp Stack pointers where all current stack pointers are stored
void compacting(internal_heap_t *heap, stack_pointers_t *sp);

/// @brief Duplicates a string and allocates it on heap
/// @param heap Heap where objects are stored
/// @param str String to duplicate
/// @return a pointer to the string allocated on heap
char *string_duplicate(internal_heap_t *heap, char *str);

/// @brief Calculates memory used by user's objects
/// @param heap Heap where objects are stored
/// @return the amount of used memory in heap
size_t used_memory(internal_heap_t *heap);

/// @brief Calulates available memory
/// @param heap Heap where objects are stored
/// @return the amount of available memory in heap
size_t avail_memory(internal_heap_t *heap);
