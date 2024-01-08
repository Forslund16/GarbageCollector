#pragma once
#include "heap.h"

typedef struct stack_pointers stack_pointers_t;
typedef struct internal_heap internal_heap_t;

/// @brief frees stack_pointers
void clear_stack_pointers(stack_pointers_t *stack_pointers);

/// @brief Searches stack (from current to environ) for pointers to heap.
/// Use clear_stack_pointers to free return value. (Might need to Dump_registers() before calling this function)
/// @param heap
/// @return array of all found pointers in order of allocation (-confirm this!) aswell as the count (array size)
/// Also returns an array of all adresses to the stack where the pointer-variables are located.
stack_pointers_t *find_stack_pointers(internal_heap_t *heap);