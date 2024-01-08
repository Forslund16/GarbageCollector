#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "heap.h"

#define PAGE_SIZE 2048
#define PAGE_ALIGNMENT 16
#define ALLOC_MAP_SIZE_IN_BITS 128

typedef struct meta meta_t;

/// @brief Internal heap that keeps track of pages
struct internal_heap
{
    page_t **page_array;
    size_t initial_size;
    int total_alloc;
    int total_avail;
};

/// @brief A page containing 2048 bytes of memory
struct page
{
    void *start;
    void *first_free;
    char *alloc_map;
    bool active;
    size_t total_alloc;
    int num_of_objects;
};

/// @brief Meta data describing what object has been allocated
struct meta
{
    char *format_string;
    void *forwarding_adress;
    long size;
    void *extra;
};

/// @brief Struct containing pointers from the stack to our heap
struct stack_pointers
{
    void **pointer_array;
    void ***pointer_addresses;
    int num_of_pointers;
};

// Functions in need of declaring
void page_activate(internal_heap_t *heap, void *pointer);

page_t *page_create(size_t bytes)
{
    page_t *new_page = calloc(1, sizeof(page_t));
    new_page->start = calloc(1, bytes);
    new_page->first_free = new_page->start;
    new_page->alloc_map = calloc((PAGE_SIZE / ALLOC_MAP_SIZE_IN_BITS) + 1, sizeof(char));
    new_page->total_alloc = 0;
    new_page->active = false;
    new_page->num_of_objects = 0;
    return new_page;
}

internal_heap_t *heap_create(size_t bytes)
{
    // Make sure bytes is divisble by PAGE_SIZE
    // otherwise give user more memory than they wanted
    if (bytes < 2048)
        bytes += 2048;
    while (bytes % PAGE_SIZE != 0)
    {
        bytes++;
    }

    int page_amount = (bytes / PAGE_SIZE);

    internal_heap_t *new_heap = calloc(1, sizeof(internal_heap_t));
    new_heap->page_array = calloc(page_amount, sizeof(page_t *));

    for (int i = 0; i < page_amount; i++)
    {
        new_heap->page_array[i] = page_create(PAGE_SIZE);
    }

    new_heap->total_alloc = 0;
    new_heap->total_avail = bytes;
    new_heap->initial_size = bytes;

    return new_heap;
}

size_t heap_size(internal_heap_t *heap)
{
    return heap->initial_size;
}

void free_page_array(page_t **page_array, size_t size)
{
    int page_amount = (size / PAGE_SIZE);
    for (int i = 0; i < page_amount; i++)
    {
        free(page_array[i]->start);
        free(page_array[i]->alloc_map);
        free(page_array[i]);
    }
}

void heap_destroy(internal_heap_t *heap)
{
    free_page_array(heap->page_array, heap->initial_size);
    free(heap->page_array);
    free(heap);
}

void heap_dbg(internal_heap_t *heap, void *dbg_value)
{
    stack_pointers_t *sp = find_stack_pointers(heap);

    for (int i = 0; i < sp->num_of_pointers; i++)
    {
        *(sp->pointer_addresses[i]) = dbg_value;
    }

    clear_stack_pointers(sp);
    heap_destroy(heap);
}

// Returns true if pointer is allocated on alloc_map
bool alloc_map_check(page_t *page, void *pointer)
{
    // Calculate index for pointer on page
    int index = (pointer - page->start) / PAGE_ALIGNMENT;

    // If index is larger than 127 the pointer is not in the page
    if (index > (ALLOC_MAP_SIZE_IN_BITS - 1) || index < 0)
    {
        return false;
    }

    // Get the alloc_char where the bit value for our pointer is
    char alloc_char = page->alloc_map[index / 8];

    // Calculate on which bit on the alloc_char our bit value is
    char bitvector = index % 8;

    // Create the mask that corresponds to our bit value
    char mask = pow(2, (7 - bitvector)); // 0000 0000
    
    return (mask & alloc_char);
}

void alloc_map_set_true(page_t *page, void *pointer)
{
    int index = (pointer - page->start) / PAGE_ALIGNMENT;
    
    // If index is larger than 127 the pointer is not in the page
    if (index > (ALLOC_MAP_SIZE_IN_BITS - 1) || index < 0)
    {
        return;
    }

    // Get the alloc_char where the bit value for our pointer is
    char alloc_char = page->alloc_map[index / 8];

    // Calculate on which bit on the alloc_char our bit value is
    char bitvector = index % 8;

    // Create the mask that corresponds to our bit value
    char mask = pow(2, (7 - bitvector));

    char new_alloc_char = mask | alloc_char;

    page->alloc_map[index / 8] = new_alloc_char;
}

void alloc_map_all_false(char *alloc_map)
{
    for (int i = 0; i < PAGE_ALIGNMENT; i++)
    {
        alloc_map[i] = 0;
    }
} 

bool page_has_space(page_t *page, size_t bytes)
{
    if ((int)(bytes + sizeof(meta_t)) <= (int)(PAGE_SIZE - (page->first_free - page->start)))
    {
        return true;
    }
    return false;
}

page_t *find_first_free_page(internal_heap_t *heap, size_t bytes)
{
    page_t *current_page = NULL;
    page_t *potential_page = NULL;
    bool potential_found = false;
    for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
    {
        if (page_has_space(heap->page_array[i], bytes))
        {
            // Save the first free page in case we dont find any active page
            if (potential_found == false)
            {
                // First inactive page with enough space
                potential_page = heap->page_array[i];
                potential_found = true;
            }
            if (heap->page_array[i]->active == true)
            {
                // First active page with enough space
                current_page = heap->page_array[i];
                return current_page;
            }
        }
    }

    // if we didnt find any active page, take the first free page
    if (current_page == NULL && potential_found)
    {
        current_page = potential_page;
    }

    return current_page;
}

page_t *find_first_inactive_page(internal_heap_t *heap)
{
    page_t *inactive_page = NULL;

    for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
    {
        if (heap->page_array[i]->active == false)
        {
            // First active page with enough space
            inactive_page = heap->page_array[i];
            break;
        }
    }

    return inactive_page;
}

page_t *inactive_page_has_space(internal_heap_t *heap, page_t *page, size_t bytes)
{
    if (page_has_space(page, bytes))
    {
        return page;
    }
    else
    {
        page = find_first_inactive_page(heap);
        return page;
    }
}

size_t page_allign_bytes(size_t bytes)
{
    while(bytes % 16 != 0)
    {
        bytes++;
    }
    return bytes;
}

void *alloc_on_page(page_t *current_page, char *layout, size_t bytes)
{
    // Store meta data
    meta_t *meta_data = (meta_t *)current_page->first_free;
    meta_data->format_string = layout;
    meta_data->size = bytes;
    meta_data->forwarding_adress = NULL;
    meta_data->extra = NULL;

    // Store user alloc
    void *to_return = current_page->first_free + sizeof(meta_t);

    // Makes adresses 16 byte-alligned
    size_t internal_bytes = page_allign_bytes(bytes);

    // Updating page values
    current_page->first_free += (sizeof(meta_t) + internal_bytes);
    alloc_map_set_true(current_page, to_return);
    current_page->active = true;
    current_page->total_alloc += bytes;
    current_page->num_of_objects += 1;

    return to_return;
}

void *alloc(internal_heap_t *heap, size_t bytes, char *layout)
{
    // bytes is less than 0 or, object is to large for allocating or, bytes and layout is used simultaneously
    if ((int)bytes < 0 || bytes > (PAGE_SIZE - sizeof(meta_t)) || (bytes > 0 && layout != NULL))
    {
        return NULL;
    }

    // bytes == 0 if we want to alloc a struct
    if (bytes == 0)
    {
        // bytes == 0 and layout == NULL, something is wrong
        if (layout == NULL)
        {
            return NULL;
        }
        
        bytes = parse_format_string(layout);

        // If layout contains invalid char then NULL
        if (bytes == 0)
        {
            return NULL;
        }
    }
    
     
    // Find which page to allocate in
    page_t *current_page = find_first_free_page(heap, bytes);

    // There is not enough space to allocate
    if(current_page == NULL)
    {
        return NULL;
    }
    
    // Update heap size value
    heap->total_avail -= (sizeof(meta_t) + page_allign_bytes(bytes));

    return alloc_on_page(current_page, layout, bytes);
}

bool page_iterator(internal_heap_t *heap, void *pointer)
{
	for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
	{
		page_t *current_page = heap->page_array[i];

		if (pointer > current_page->start && pointer < (current_page->start + PAGE_SIZE))
		{
			if (current_page->active)
			{
				return (alloc_map_check(current_page, pointer));
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

page_t *page_find(internal_heap_t *heap, void *pointer)
{
    for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
    {
        page_t *current_page = heap->page_array[i];
        if (pointer > current_page->start && pointer < (current_page->start + PAGE_SIZE))
        {
            return current_page;
        }
    }
    return NULL;
}

void iterate_format_string_pointers(internal_heap_t *heap, void **pointer, char *format_string)
{
    size_t offset = 0;
    void *pointer_start = (void *)pointer;

    int length = strlen(format_string);
    for (int i = 0; i < length; i++)
    {
        char current_char = format_string[i];
        void **current_pointer_address = pointer_start + offset;

        void *current_pointer = current_pointer_address[0];
        
        if (current_char == '*' && current_pointer != NULL)
        {
            page_activate(heap, current_pointer); 
        }
        offset += parse_format_char(current_char); 
    }
}

void page_activate(internal_heap_t *heap, void *pointer)
{
    meta_t *current_meta = pointer - sizeof(meta_t);

    page_t *to_activate = page_find(heap, pointer);
    if(to_activate != NULL){
        to_activate->active = true;
    }
    else
    {
        // We had a bad pointer if we got here
        return;
    }
    // If the pointer is a struct and have internal pointers, iterate over them and activate their pages
    if (current_meta->format_string != NULL)
    {
        char unfolded_layout[calculate_layout_length(current_meta->format_string)];
        unfold_compact_format_string(current_meta->format_string, unfolded_layout);
        iterate_format_string_pointers(heap, pointer, unfolded_layout);
    }
}

void clear_page(page_t *page)
{
    for (int i = 0; i < (PAGE_SIZE); i++)
    {
        char *byte = (char *)page->start + i;
        *byte = '\0';
    }
}

void page_deactivate(internal_heap_t *heap, stack_pointers_t *sp)
{
    // Deactivates all pages
    for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
    {
        page_t *current_page = heap->page_array[i];
        current_page->active = false;
    }

    int array_size = sp->num_of_pointers;

    // Active all pages with pointers pointing to them
    for (int i = 0; i < array_size; i++)
    {
        void *current_pointer = sp->pointer_array[i];
        page_activate(heap, current_pointer);
    }

    // Reset pointer of all deactivated pages back to start and
    // update total available memory
    for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
    {
        page_t *current_page = heap->page_array[i];
        if (current_page->start < current_page->first_free && current_page->active == false)
        {
            heap->total_avail += (current_page->first_free - current_page->start);
            alloc_map_all_false(current_page->alloc_map);
            clear_page(current_page);
            current_page->total_alloc = 0; 
            current_page->first_free = current_page->start;
            current_page->num_of_objects = 0;
        }
    }
}

page_t **find_active_pages(internal_heap_t *heap)
{
    int num_of_pages = (int)(heap->initial_size / PAGE_SIZE);
    page_t **active_page_array = calloc(num_of_pages, sizeof(page_t *));

    int j = 0;
    for (int i = 0; i < num_of_pages; i++)
    {
        page_t *current_page = heap->page_array[i];
        if (current_page->active)
        {
            active_page_array[j] = current_page;
            j++;
        }
    }
    if (j == 0)
    {
        free(active_page_array);
        return NULL;
    }
    return active_page_array;
}

void clear_pages(page_t **page_array, internal_heap_t *heap)
{
    for (int i = 0; page_array[i] != NULL; i++)
    {
        page_t *current_active_page = page_array[i];
        if (current_active_page->total_alloc > 0)
        {
            heap->total_avail += (current_active_page->first_free - current_active_page->start);
            current_active_page->total_alloc = 0;
            current_active_page->num_of_objects = 0;
        }
        current_active_page->active = false;
        current_active_page->first_free = current_active_page->start;
        alloc_map_all_false(current_active_page->alloc_map);
        clear_page(current_active_page);
    }
}

void repoint_pointers(void **pointer_address)
{
    // Take out the pointer to the struct to get correct arithmetic pointer calculations
    void *current_pointer = (void *)pointer_address;
    meta_t *meta = current_pointer - sizeof(meta_t);
    char *format_string = meta->format_string;
    
    // Unfold compact string
    char unfolded_format_string[calculate_layout_length(format_string)];
    unfold_compact_format_string(format_string, unfolded_format_string);

    size_t bytes = meta->size;

    for (size_t offset = 0, i = 0; offset < bytes; offset += parse_format_char(unfolded_format_string[i]), i++) 
    {
        char c = unfolded_format_string[i];

        // If the pointer inside the struct is a pointer, we need to repoint it
        if (c == '*')
        {
            void **old_pointer_address = current_pointer + offset;

            void *old_pointer = old_pointer_address[0];

            // Gets triggered if the previous pointer wasn't set
            if(old_pointer == NULL)
            {
                continue;
            }

            meta_t *old_meta = old_pointer - sizeof(meta_t);

            void *new_pointer = old_meta->forwarding_adress;

            if(new_pointer == NULL){
                continue;
            }
            // Update the pointervalue to the new pointer
            *(old_pointer_address) = new_pointer;

            if (old_meta->format_string != NULL)
            {
                repoint_pointers(new_pointer);
                old_meta->format_string = NULL;
            }
        }
    }
}

page_t *compact_object(internal_heap_t *heap, page_t *page, void *pointer)
{
    meta_t *meta = pointer - sizeof(meta_t);
    char *format_string = meta->format_string;

    // if the pointer have a forwarding address it means it has
    // already been moved so don't move it again
    if (meta->forwarding_adress != NULL)
    {
        return page;
    }

    bool has_internal_pointers = false;
    if (format_string != NULL)
    {
        has_internal_pointers = true;
    }
    size_t bytes = meta->size;

    // Updating previous page values
    page_t *previous_page = page_find(heap, pointer);

    size_t internal_bytes = page_allign_bytes(bytes);

    previous_page->first_free -= (internal_bytes + sizeof(meta_t));
    previous_page->num_of_objects -= 1;

    page = inactive_page_has_space(heap, page, internal_bytes);
    if(page == NULL)
    {
        return NULL;
    }

    void *new_pointer = alloc_on_page(page, meta->format_string, bytes);

    // Copy data to new page
    memcpy(new_pointer, pointer, internal_bytes);

    meta->forwarding_adress = new_pointer;

    if (has_internal_pointers)
    {
        size_t offset = 0;
        char unfolded_format_string[calculate_layout_length(meta->format_string)];
        unfold_compact_format_string(format_string, unfolded_format_string);
        for (int i = 0; i < (int)strlen(unfolded_format_string); i++)
        {
            
            // Compact the internal pointer
            // Go to next start of internal pointer
            void **next_pointer = pointer + offset;
            if (unfolded_format_string[i] == '*' && next_pointer[0] != NULL)
            {
                page = compact_object(heap, page, next_pointer[0]);
                if(page == NULL){
                    return NULL;
                }
            }

            offset += parse_format_char(unfolded_format_string[i]);
        }
    }

    return page;
}



void compacting(internal_heap_t *heap, stack_pointers_t *sp)
{
    page_t **active_page_array = find_active_pages(heap);

    page_t *current_page = find_first_inactive_page(heap);

    // There is no inactive pages and compacting cant be executed
    if(current_page == NULL)
    {
        printf("HEAP IS FULL, TERMINATING PROGRAM!!!\n");
        abort();
    }

    for (int i = 0; i < sp->num_of_pointers; i++)
    {
        void *current_pointer = sp->pointer_array[i];
        current_page = compact_object(heap, current_page, current_pointer);

        // There is not any pages with space left, heap is full
        if(current_page == NULL)
        {
            printf("HEAP IS FULL, TERMINATING PROGRAM!!!\n");
            abort();
        }

    }

    for (int i = 0; i < sp->num_of_pointers; i++)
    {
        void **current_pointer_address = sp->pointer_addresses[i];
        void *current_pointer = sp->pointer_array[i];
        meta_t *current_meta = (meta_t *)(current_pointer - sizeof(meta_t));
        void *new_pointer = current_meta->forwarding_adress;

        // Update stack pointer with the new pointer
        *current_pointer_address = new_pointer;

        // Updates the internal pointers
        if (current_meta->format_string != NULL)
        {
            repoint_pointers(new_pointer);
        }
    }

    // Set active pages to inactive and update size information
    if(active_page_array != NULL)
    {
        clear_pages(active_page_array, heap);
    }
    free(active_page_array);
}

char *string_duplicate(internal_heap_t *heap, char *str)
{
    int length = strlen(str);
    char *pointer = alloc(heap, length * sizeof(char) + 1, NULL);
    for (int i = 0; i < length; i++)
    {
        pointer[i] = str[i];
    }
    pointer[length] = '\0';
    return pointer;
}

size_t used_memory(internal_heap_t *heap)
{
    page_t **page_array = heap->page_array;
    int num_of_pages = (int)heap->initial_size / PAGE_SIZE;
    heap->total_alloc = 0;

    for(int i = 0; i < num_of_pages; i++)
    {
        page_t *current_page = page_array[i];
        if(current_page->active)
        {
            heap->total_alloc += current_page->total_alloc;
        }
    }
    return heap->total_alloc;
}

size_t avail_memory(internal_heap_t *heap)
{
    return heap->total_avail;
}
