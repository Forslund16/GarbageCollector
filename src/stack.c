#include <stdlib.h>
#include <setjmp.h>
#include "stack.h"
#include "heap.h"

extern char **environ; // bottom of stack

/// @brief Struct containing pointers from the stack to our heap
struct stack_pointers
{
    void **pointer_array;
    void ***pointer_addresses;
    int num_of_pointers;
};

// frees stack_pointers
void clear_stack_pointers(stack_pointers_t *stack_pointers)
{
	free(stack_pointers->pointer_array);
	free(stack_pointers->pointer_addresses);
	free(stack_pointers);
	stack_pointers = NULL;
}

stack_pointers_t *find_stack_pointers(internal_heap_t *heap)
{
	stack_pointers_t *stack_pointers = calloc(1, sizeof(stack_pointers_t));
	stack_pointers->pointer_array = calloc(heap_size(heap) / 32, sizeof(void *));
	stack_pointers->pointer_addresses = calloc(heap_size(heap) / 32, sizeof(void **));
	stack_pointers->num_of_pointers = 0;

	void *top = __builtin_frame_address(0);
	char **bottom = environ;
	void *pointer = NULL;

	for (void *i = top; i < bottom; i = i + 8) 
	{
		
		//NOTE: its illegal do dereference an unitiliazed pointer which gives us the errors
		pointer = *((void **)i);

		if (page_iterator(heap, pointer))
		{	
			stack_pointers->pointer_array[stack_pointers->num_of_pointers] = pointer;
			stack_pointers->pointer_addresses[stack_pointers->num_of_pointers] = (void **)i;
			stack_pointers->num_of_pointers++;
		}
	}
	return stack_pointers;
}
