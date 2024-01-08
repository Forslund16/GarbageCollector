#include <CUnit/Basic.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "../src/gc.h"
#include "../src/stack.h"
#include "../src/heap.h"

#define Dump_registers() \
	jmp_buf env;         \
	if (setjmp(env))     \
		abort();

#define PAGE_SIZE 2048
#define META_DATA_SIZE 32

typedef struct test test_t;
typedef struct next_struct next_struct_t;

typedef struct page page_t;
typedef struct meta meta_t;
typedef struct stack_pointers stack_pointers_t;
typedef struct alloc_map alloc_map_t;
typedef struct internal_heap internal_heap_t;

struct test
{
	char *string;
	next_struct_t *next;
	int i;
};

struct next_struct
{
	char *string;
	int i;
};

/// @brief Heap where user can store objects
struct heap
{
    internal_heap_t *heap;
    bool unsafe_stack;
    float gc_threshold;
    size_t total_size;
    bool display_error;
};


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

// helper function for tests
// page_number decides if first page or first two pages are supposed to be active
void check_active_pages(internal_heap_t *heap, int page_number)
{
	for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
	{
		page_t *current_page = heap->page_array[i];
		if (i == 0 || i == page_number)
		{
			CU_ASSERT_TRUE(current_page->active);
		}
		else
		{
			CU_ASSERT_FALSE(current_page->active);
		}
	}
}

int init_suite(void)
{
	// Change this function if you want to do something *before* you
	// run a test suite
	return 0;
}

int clean_suite(void)
{
	// Change this function if you want to do something *after* you
	// run a test suite
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
// HEAP TESTS
void test_create_destroy()
{
	heap_t *heap = h_init(2000, true, 0.90);
	CU_ASSERT_PTR_NOT_NULL(heap);
	h_delete(heap);
}

void test_alloc_raw()
{
	heap_t *heap = h_init(4096, true, 0.90);
	void *pointer = h_alloc_data(heap, 4);
	CU_ASSERT_PTR_NOT_NULL(pointer);

	int *num = h_alloc_data(heap, sizeof(int));
	*num = 42;
	CU_ASSERT_EQUAL(*num, 42);

	h_delete(heap);
}

void test_alloc_struct()
{
	heap_t *heap = h_init(4096, true, 0.90);

	test_t *test_struct = h_alloc_struct(heap, "**i");
	next_struct_t *next_struct = h_alloc_struct(heap, "*i");

	CU_ASSERT_PTR_NOT_NULL(test_struct);
	CU_ASSERT_PTR_NOT_NULL(next_struct);

	char *hej = h_str_dup(heap, "hej");

	test_struct->string = hej;
	test_struct->next = next_struct;
	test_struct->i = 42;
	CU_ASSERT(strcmp(test_struct->string, "hej") == 0);
	CU_ASSERT_PTR_NOT_NULL(test_struct->next);
	CU_ASSERT_EQUAL(test_struct->i, 42);

	char *next = h_str_dup(heap, "next");
	next_struct->string = next;
	next_struct->i = 12;
	CU_ASSERT(strcmp(next_struct->string, "next") == 0);
	CU_ASSERT_EQUAL(next_struct->i, 12);

	h_delete(heap);
}

void test_alloc_invalid_input()
{
	heap_t *heap = h_init(4096, true, 0.60);

	CU_ASSERT_PTR_NULL(h_alloc_data(heap, 0));
	CU_ASSERT_PTR_NULL(h_alloc_data(heap, -1));
	CU_ASSERT_PTR_NULL(h_alloc_struct(heap, NULL));
	CU_ASSERT_PTR_NULL(h_alloc_struct(heap, "iiof"));
	CU_ASSERT_PTR_NOT_NULL(h_alloc_data(heap, 1));
	CU_ASSERT_PTR_NOT_NULL(h_alloc_struct(heap, "i**"));

	h_delete(heap);
}

// Test that two objects use expected amount of memory and pages, and that invalid alloc doesn't use up memory.
void test_used_memory()
{
	heap_t *heap = h_init(8192, true, 0.80); // 4 pages
	CU_ASSERT_EQUAL(h_used(heap), 0);

	void *pointer = h_alloc_data(heap, 60);
	CU_ASSERT_PTR_NOT_NULL(pointer);
	CU_ASSERT_EQUAL(h_used(heap), 60);

	void *invalid_alloc = h_alloc_data(heap, -1000);
	CU_ASSERT_PTR_NULL(invalid_alloc);
	CU_ASSERT_EQUAL(h_used(heap), 60);

	void *big = h_alloc_data(heap, 2000);
	check_active_pages(heap->heap, 1); // check that first two pages are active
	CU_ASSERT_EQUAL(h_used(heap), 2060);

	h_delete(heap);
}

void test_str_dup()
{
	heap_t *heap = h_init(4096, true, 0.99);

	char *string = "Hello!";
	char *string_dup = h_str_dup(heap, string);
	CU_ASSERT_EQUAL(h_used(heap), 7);
	CU_ASSERT(strcmp(string_dup, "Hello!") == 0);

	char *string_dup_dup = h_str_dup(heap, string_dup);
	CU_ASSERT_NOT_EQUAL(string_dup_dup, string);
	CU_ASSERT_EQUAL(h_used(heap), 14);

	h_delete(heap);
}

void test_struct_size()
{
	internal_heap_t *heap = heap_create(10240);

	CU_ASSERT_EQUAL(parse_format_string("**i"), sizeof(void *) + sizeof(void *) + sizeof(int));
	CU_ASSERT_EQUAL(parse_format_string("*i"), sizeof(void *) + sizeof(int));

	heap_destroy(heap);
}

void test_object_size()
{
	internal_heap_t *heap = heap_create(10240);

	void *test = alloc(heap, 1001, NULL);
	meta_t *meta = test - sizeof(meta_t);

	CU_ASSERT_EQUAL(1001, meta->size);
	heap_destroy(heap);
}

void test_page_activation_heap()
{
	internal_heap_t *heap = heap_create(10240);

	test_t *test_struct = alloc(heap, 0, "**i");
	next_struct_t *next_struct = alloc(heap, 0, "*i");

	test_struct->string = alloc(heap, 4 * sizeof(char), NULL);
	test_struct->next = next_struct;
	next_struct->string = alloc(heap, 5 * sizeof(char), NULL);

	// Allocated to make sure page 2 is activated
	void *big_object = alloc(heap, 2000, NULL);
	CU_ASSERT_PTR_NOT_NULL(big_object);

	stack_pointers_t *sp = calloc(1, sizeof(stack_pointers_t));

	void **pointer_array = calloc(1, sizeof(void *));
	pointer_array[0] = (void *)test_struct;

	sp->pointer_array = pointer_array;
	sp->num_of_pointers = 1;

	// Check avail_memory and used_memory before deactivation
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char) + 2000) - 5 * sizeof(meta_t));
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char) + 2000);

	// Check that first and second page is active
	check_active_pages(heap, 1);

	// Deactivates pages without living objects
	page_deactivate(heap, sp);

	// Check that avail_memory and used_memory is updated after deactivation
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char)) - 4 * sizeof(meta_t));
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char));

	// Check that only the first page is active
	check_active_pages(heap, 0);

	free(pointer_array);
	free(sp);
	heap_destroy(heap);
}

void test_alloc_map()
{
	internal_heap_t *heap = heap_create(4096);

	void *pointer = alloc(heap, 4, NULL);
	CU_ASSERT_PTR_NOT_NULL(pointer);

	CU_ASSERT_TRUE(alloc_map_check(heap->page_array[0], pointer));

	void *pointer2 = alloc(heap, 12, NULL);
	CU_ASSERT_PTR_NOT_NULL(pointer2);

	char *pointer3 = alloc(heap, 2000, NULL);

	CU_ASSERT_TRUE(alloc_map_check(heap->page_array[0], pointer));
	CU_ASSERT_TRUE(alloc_map_check(heap->page_array[0], pointer2));
	CU_ASSERT_FALSE(alloc_map_check(heap->page_array[0], pointer3));
	CU_ASSERT_FALSE(alloc_map_check(heap->page_array[1], pointer3 + 16));
	CU_ASSERT_TRUE(alloc_map_check(heap->page_array[1], pointer3));

	alloc_map_set_true(heap->page_array[0], heap->page_array[0]->start - 16);
	alloc_map_set_true(heap->page_array[0], heap->page_array[0]->start + 144);

	CU_ASSERT_FALSE(alloc_map_check(heap->page_array[0], -1));
	CU_ASSERT_FALSE(alloc_map_check(heap->page_array[0], 128));

	stack_pointers_t *sp = calloc(1, sizeof(stack_pointers_t));

	void **pointer_array = calloc(2, sizeof(void *));
	pointer_array[0] = pointer;
	pointer_array[1] = pointer2;

	sp->pointer_array = pointer_array;
	sp->num_of_pointers = 2;

	page_deactivate(heap, sp);

	CU_ASSERT_TRUE(alloc_map_check(heap->page_array[0], pointer));
	CU_ASSERT_TRUE(alloc_map_check(heap->page_array[0], pointer2));
	CU_ASSERT_FALSE(alloc_map_check(heap->page_array[0], pointer3));
	CU_ASSERT_FALSE(alloc_map_check(heap->page_array[1], pointer3));

	free(pointer_array);
	free(sp);
	heap_destroy(heap);
}

void test_compacting_sp()
{
	internal_heap_t *heap = heap_create(10240);

	test_t *test_struct = alloc(heap, 0, "2*i");
	next_struct_t *next_struct = alloc(heap, 0, "*i");

	void *tmp1 = next_struct;
	void *tmp2 = test_struct;

	test_struct->string = string_duplicate(heap, "hej");
	test_struct->next = next_struct;
	test_struct->i = 42;
	next_struct->string = string_duplicate(heap, "next");

	// big object fits on first page with no more room left
	void *big_object = alloc(heap, 1792, NULL);
	CU_ASSERT_PTR_NOT_NULL(big_object);

	// pointer is allocated on page 2
	void *pointer = alloc(heap, 32, NULL);

	void *tmp3 = big_object;
	void *tmp4 = pointer;
	void *tmp5 = test_struct->string;

	stack_pointers_t *sp = find_stack_pointers(heap);

	page_deactivate(heap, sp);

	compacting(heap, sp);

	CU_ASSERT_PTR_EQUAL(tmp1, next_struct);
	CU_ASSERT_PTR_EQUAL(tmp2, test_struct);

	clear_stack_pointers(sp);
	heap_destroy(heap);
}

void test_compacting()
{
	internal_heap_t *heap = heap_create(10240);

	test_t *test_struct = alloc(heap, 0, "2*i");
	next_struct_t *next_struct = alloc(heap, 0, "*i");

	next_struct_t *tmp1 = next_struct;
	test_t *tmp2 = test_struct;

	test_struct->string = string_duplicate(heap, "hej");
	test_struct->next = next_struct;
	test_struct->i = 42;
	next_struct->string = string_duplicate(heap, "next");

	// big object fits on first page with no more room left
	void *big_object = alloc(heap, 1792, NULL);
	CU_ASSERT_PTR_NOT_NULL(big_object);

	// pointer is allocated on page 2
	void *pointer = alloc(heap, 32, NULL);

	void *tmp3 = big_object;
	void *tmp4 = pointer;
	void *tmp5 = test_struct->string;

	stack_pointers_t *sp = calloc(1, sizeof(stack_pointers_t));

	void **pointer_array = calloc(3, sizeof(void *));
	pointer_array[0] = (void *)test_struct;
	pointer_array[1] = pointer;
	pointer_array[2] = next_struct;

	void ***addresses = calloc(3, sizeof(void **));
	addresses[0] = (void **)&(test_struct);
	addresses[1] = &pointer;
	addresses[2] = &next_struct;

	sp->pointer_addresses = addresses;

	sp->pointer_array = pointer_array;
	sp->num_of_pointers = 3;

	// Check that strings can be read before compacting

	CU_ASSERT(strcmp(test_struct->string, "hej") == 0);
	CU_ASSERT(strcmp(next_struct->string, "next") == 0);

	// Check avail_memory and used_memory before deactivation
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char) + 1792 + 32) - 6 * sizeof(meta_t));
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char) + 1792 + 32);

	// Check that first and second page is active
	check_active_pages(heap, 1);

	// Deactivates pages without living objects
	page_deactivate(heap, sp);

	// Check that avail_memory and used_memory are the same after deactivation
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char) + 1792 + 32) - 6 * sizeof(meta_t));
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char) + 1792 + 32);

	// Check that both are still active
	check_active_pages(heap, 1);

	compacting(heap, sp);

	// Check that avail_memory and used_memory are the updated after compacting
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char) + 32);
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char) + 32) - 5 * sizeof(meta_t));

	// Check that only the first page is active
	for (int i = 0; i < (int)(heap->initial_size / PAGE_SIZE); i++)
	{
		page_t *current_page = heap->page_array[i];
		if (i == 2)
		{
			CU_ASSERT_TRUE(current_page->active);
		}
		else
		{
			CU_ASSERT_FALSE(current_page->active);
		}
	}

	// Check that strings can be read before compacting
	CU_ASSERT(strcmp(test_struct->string, "hej") == 0);
	CU_ASSERT(strcmp(test_struct->next->string, "next") == 0);

	CU_ASSERT_PTR_NOT_EQUAL(tmp1, next_struct);
	CU_ASSERT_PTR_NOT_EQUAL(tmp2, test_struct);
	CU_ASSERT_PTR_EQUAL(tmp3, big_object);
	CU_ASSERT_PTR_NOT_EQUAL(tmp4, pointer);
	CU_ASSERT_PTR_NOT_EQUAL(tmp5, test_struct->string);

	free(addresses);
	free(pointer_array);
	free(sp);
	heap_destroy(heap);
}

void test_avail_memory()
{
	internal_heap_t *heap = heap_create(4096);
	CU_ASSERT_EQUAL(avail_memory(heap), 4096);

	void *pointer = alloc(heap, 4, NULL);
	CU_ASSERT_PTR_NOT_NULL(pointer);

	CU_ASSERT_EQUAL(avail_memory(heap), (4096 - 16 - META_DATA_SIZE));

	void *pointer2 = alloc(heap, 12, NULL);
	CU_ASSERT_PTR_NOT_NULL(pointer2);

	CU_ASSERT_EQUAL(avail_memory(heap), (4096 - 32 - (2 * META_DATA_SIZE)));

	heap_destroy(heap);
}

void test_layout_functions()
{
	heap_t *heap = h_init(2000, true, 0.90);

	CU_ASSERT_EQUAL(parse_format_char('*'), 8);
	CU_ASSERT_EQUAL(parse_format_char('i'), 4);
	CU_ASSERT_EQUAL(parse_format_char('c'), 1);
	CU_ASSERT_EQUAL(parse_format_char('l'), 8);
	CU_ASSERT_EQUAL(parse_format_char('f'), 4);
	CU_ASSERT_EQUAL(parse_format_char('d'), 8);
	CU_ASSERT_EQUAL(parse_format_char('n'), 0);

	CU_ASSERT_EQUAL(parse_format_string("***ll"), 8 * 3 + 8 * 2);
	CU_ASSERT_EQUAL(parse_format_string("3*2l"), 8 * 3 + 8 * 2);
	CU_ASSERT_EQUAL(parse_format_string(NULL), 0);
	CU_ASSERT_EQUAL(parse_format_string("25"), 25);

	char str[6];
	unfold_compact_format_string("5", str);
	CU_ASSERT(strcmp(str, "ccccc") == 0);

	h_delete(heap);
}

//////////////////////////////////////////////////////////////////////////////////////
// STACK TESTS

void test_find_single()
{
	// Dump_registers();
	internal_heap_t *heap = heap_create(4096);
	int *num = alloc(heap, sizeof(int), NULL);
	*num = 1337;
	stack_pointers_t *stack_pointers = find_stack_pointers(heap);

	CU_ASSERT_EQUAL(stack_pointers->num_of_pointers, 1);
	CU_ASSERT_EQUAL(stack_pointers->pointer_array[0], num);

	clear_stack_pointers(stack_pointers);
	heap_destroy(heap);
}

void test_find_multiple()
{
	// Dump_registers();
	heap_t *external_heap = h_init(65596, false, 0.7);
	internal_heap_t *heap = external_heap->heap;
	int *num = h_alloc_data(external_heap, sizeof(num));
	char *chr = h_alloc_data(external_heap, 20);
	test_t *test_struct = h_alloc_struct(external_heap, "**i");
	next_struct_t *next_struct = h_alloc_struct(external_heap, "*i");
	stack_pointers_t *stack_pointers = find_stack_pointers(heap);

	CU_ASSERT_EQUAL(stack_pointers->num_of_pointers, 4);
	CU_ASSERT_EQUAL(stack_pointers->pointer_array[0], num);
	CU_ASSERT_EQUAL(stack_pointers->pointer_array[1], chr);
	CU_ASSERT_EQUAL(stack_pointers->pointer_array[2], test_struct);
	CU_ASSERT_EQUAL(stack_pointers->pointer_array[3], next_struct);

	clear_stack_pointers(stack_pointers);
	h_delete(external_heap);
}

void test_page_activation_sp()
{
	// Dump_registers();
	internal_heap_t *heap = heap_create(10240);

	test_t *test_struct = alloc(heap, 0, "**i");
	next_struct_t *next_struct = alloc(heap, 0, "*i");

	test_struct->string = alloc(heap, 4 * sizeof(char), NULL);
	test_struct->next = next_struct;
	next_struct->string = alloc(heap, 5 * sizeof(char), NULL);

	// Allocated to make sure page 2 is activated
	void *big_object = alloc(heap, 2000, NULL);
	CU_ASSERT_PTR_NOT_NULL(big_object);
	big_object = NULL;

	stack_pointers_t *sp = find_stack_pointers(heap);

	// Check avail_memory and used_memory before deactivation
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char) + 2000) - 5 * sizeof(meta_t));
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char) + 2000);

	// Check that first and second page is active
	check_active_pages(heap, 1);

	// Deactivates pages without living objects
	page_deactivate(heap, sp);

	// Check that avail_memory and used_memory is updated after deactivation
	CU_ASSERT_EQUAL(avail_memory(heap), 10240 - (parse_format_string("**i") + 16 + parse_format_string("*i") + 32 * sizeof(char)) - 4 * sizeof(meta_t));
	CU_ASSERT_EQUAL(used_memory(heap), parse_format_string("**i") + parse_format_string("*i") + 9 * sizeof(char));

	// Check that only the first page is active
	check_active_pages(heap, 0);

	clear_stack_pointers(sp);
	heap_destroy(heap);
}

void test_create_destroy_dbg() // update to use gc.h functions once h_gc_dbg is added.
{
	internal_heap_t *heap = heap_create(4096);
	void *pointer = alloc(heap, 4, NULL);
	CU_ASSERT_PTR_NOT_NULL(pointer);

	int *num = alloc(heap, sizeof(int), NULL);
	*num = 42;
	CU_ASSERT_EQUAL(*num, 42);

	heap_dbg(heap, "null");

	CU_ASSERT(strcmp(pointer, "null") == 0);
	CU_ASSERT(strcmp((char *)num, "null") == 0);
}

//////////////////////////////////////////////////////////////////////////////////////
// GC TESTS

void test_gc()
{
	heap_t *heap = h_init(8096, true, 0.75);
	void *big1 = h_alloc_data(heap, 800);
	void *small = h_alloc_data(heap, 20);
	void *big2 = h_alloc_data(heap, 900);

	void **big2cpy = calloc(1, sizeof(void *));
	*big2cpy = big2;

	h_gc(heap);
	CU_ASSERT_EQUAL(h_used(heap), 1720);
	CU_ASSERT_NOT_EQUAL(*big2cpy, big2); // check that adress in big2 has changed
	free(big2cpy);

	big1 = NULL;
	h_gc(heap);
	CU_ASSERT_EQUAL(h_used(heap), 920);

	h_delete(heap);
}

void test_scattered()
{
	heap_t *heap = h_init(8096, true, 0.75);

	void *trash = h_alloc_data(heap, 50);
	void *big1 = h_alloc_data(heap, 800);
	void *small = h_alloc_data(heap, 20);
	void *bigtrash = h_alloc_data(heap, 1400);
	void *big2 = h_alloc_data(heap, 400);
	check_active_pages(heap->heap, 1); // 2 active pages
	CU_ASSERT_EQUAL(h_used(heap), 50 + 800 + 20 + 1400 + 400);

	trash = NULL;
	bigtrash = NULL;
	h_gc(heap);
	CU_ASSERT_EQUAL(h_used(heap), 800 + 20 + 400);
	CU_ASSERT_TRUE(heap->heap->page_array[1]->active); // Remaining objects are moved into page 2.

	h_delete(heap);
}

void test_threshold()
{
	heap_t *heap = h_init(4096, true, 0.25);
	void *big1 = h_alloc_data(heap, 1200);
	void *small = h_alloc_data(heap, 20);

	CU_ASSERT_EQUAL(h_used(heap), 1220);
	big1 = NULL;						  // Now memory allocated for big1 wont be found, should therefore be freed by GC
	void *big2 = h_alloc_data(heap, 500); // should trigger GC as its over threshold (0.25 * 4096 = 1024)

	CU_ASSERT_EQUAL(h_used(heap), 520);

	h_delete(heap);
}

void test_gc_pointers()
{
	heap_t *heap = h_init(4096, true, 0.5);
	test_t *test_pointer = h_alloc_struct(heap, "**i");
	next_struct_t *object = h_alloc_struct(heap, "*i"); // 32 bytes total
	object->i = 42;
	test_pointer->next = object;
	object = NULL; // object now only accessible via stack->test_pointer->object
	h_gc(heap);
	CU_ASSERT_EQUAL(h_used(heap), 32);
	CU_ASSERT_EQUAL(test_pointer->next->i, 42);

	h_delete(heap);
}

void test_pointers_long()
{
	heap_t *heap = h_init(4096, true, 0.95);

	test_t *head = h_alloc_struct(heap, "**i");
	test_t *link1 = h_alloc_struct(heap, "**i");
	void *big = h_alloc_data(heap, h_avail(heap) % PAGE_SIZE); // Fills up the rest of page 1.
	big = NULL;
	test_t *link2 = h_alloc_struct(heap, "**i");
	test_t *tail = h_alloc_struct(heap, "**i");
	head->next = link1;
	link1->next = link2;
	link2->next = tail;
	link2 = NULL;
	tail = NULL;
	link1 = NULL;
	h_gc(heap);

	CU_ASSERT_EQUAL(h_used(heap), 80);
	CU_ASSERT_TRUE(heap->heap->page_array[1]->active); // Linked list is moved to page 2 during compacting.
	CU_ASSERT_FALSE(heap->heap->page_array[0]->active);
	h_delete(heap);
}

void test_stack_pointer()
{
	heap_t *heap = h_init(4096, true, 0.99);

	int *num_on_heap_1 = h_alloc_data(heap, sizeof(int));
	*num_on_heap_1 = 42;
	int *num_on_heap_2 = h_alloc_data(heap, sizeof(int));
	*num_on_heap_2 = 123456;
	int *num_on_heap_3 = h_alloc_data(heap, sizeof(int));
	*num_on_heap_3 = 1337;

	h_gc(heap);
	CU_ASSERT_EQUAL(*num_on_heap_1, 42);
	CU_ASSERT_EQUAL(*num_on_heap_2, 123456);
	CU_ASSERT_EQUAL(*num_on_heap_3, 1337);

	h_delete(heap);
}

void test_full_threshold()
{
	heap_t *heap = h_init(4096, true, 1.0);

	void *big1 = h_alloc_data(heap, 2016);
	void *big2 = h_alloc_data(heap, 2016);
	big1 = NULL;
	void *big3 = h_alloc_data(heap, 2016);
	CU_ASSERT_EQUAL(h_avail(heap), 0);

	h_delete(heap);
}

void test_delete_dbg()
{
	heap_t *heap = h_init(4096, true, 0.90);

	int num = 42;
	int *ptr_to_num = &num;

	void *heap_obj = h_alloc_data(heap, 8);
	h_delete_dbg(heap, NULL);

	CU_ASSERT_PTR_NULL(heap_obj);
	CU_ASSERT_EQUAL(*ptr_to_num, 42); // other pointers on the stack should be unaffected
}

void test_gc_dbg()
{
	heap_t *heap = h_init(4096, false, 0.90);

	CU_ASSERT_FALSE(heap->unsafe_stack);
	char *str = h_str_dup(heap, "hej");
	str = NULL;
	CU_ASSERT_FALSE(heap->unsafe_stack);

	CU_ASSERT_NOT_EQUAL(h_gc_dbg(heap, true), 0);
	CU_ASSERT_TRUE(heap->unsafe_stack);

	CU_ASSERT_EQUAL(h_gc_dbg(heap, true), 0);
	CU_ASSERT_TRUE(heap->unsafe_stack);

	h_delete(heap);
}

int main()
{
	// First we try to set up CUnit, and exit if we fail
	if (CU_initialize_registry() != CUE_SUCCESS)
		return CU_get_error();

	// We then create an empty test suite and specify the name and
	// the init and cleanup functions
	CU_pSuite heap_suite = CU_add_suite("Project Omegalul heap.c tests", init_suite, clean_suite);
	CU_pSuite stack_suite = CU_add_suite("Project Omegalul stack.c tests", init_suite, clean_suite);
	CU_pSuite gc_suite = CU_add_suite("Project Omegalul gc.c tests", init_suite, clean_suite);
	if (heap_suite == NULL || stack_suite == NULL || gc_suite == NULL)
	{
		// If the test suite could not be added, tear down CUnit and exit
		CU_cleanup_registry();
		return CU_get_error();
	}

	printf("Unit tests\n");

	// This is where we add the test functions to our test suite.
	// For each call to CU_add_test we specify the test suite, the
	// name or description of the test, and the function that runs
	// the test in question. If you want to add another test, just
	// copy a line below and change the information
	if (
		//////////////////////////////////////////////////////////////////////////////////////
		// HEAP TESTS
		(CU_add_test(heap_suite, "Create and Destroy", test_create_destroy) == NULL) ||
		(CU_add_test(heap_suite, "Alloc raw data", test_alloc_raw) == NULL) ||
		(CU_add_test(heap_suite, "Alloc struct", test_alloc_struct) == NULL) ||
		(CU_add_test(heap_suite, "Alloc invalid input", test_alloc_invalid_input) == NULL) ||
		(CU_add_test(heap_suite, "Used memory", test_used_memory) == NULL) ||
		(CU_add_test(heap_suite, "Avail memory", test_avail_memory) == NULL) ||
		(CU_add_test(heap_suite, "Activation", test_page_activation_heap) == NULL) ||
		(CU_add_test(heap_suite, "Struct size", test_struct_size) == NULL) ||
		(CU_add_test(heap_suite, "Alloc map", test_alloc_map) == NULL) ||
		(CU_add_test(heap_suite, "Compacting", test_compacting) == NULL) ||
		(CU_add_test(heap_suite, "Compacting Stack Pointers", test_compacting_sp) == NULL) ||
		(CU_add_test(heap_suite, "Object size", test_object_size) == NULL) ||
		(CU_add_test(heap_suite, "Struct size", test_create_destroy_dbg) == NULL) ||
		(CU_add_test(heap_suite, "Layout string", test_layout_functions) == NULL) ||

		//////////////////////////////////////////////////////////////////////////////////////
		// STACK TESTS
		(CU_add_test(stack_suite, "Search stack for single object on heap", test_find_single) == NULL) ||
		(CU_add_test(stack_suite, "Find multiple in order", test_find_multiple) == NULL) ||
		(CU_add_test(stack_suite, "Page activation", test_page_activation_sp) == NULL) ||
		(CU_add_test(stack_suite, "Free heap and overwrite all pointers to it on stack", test_create_destroy_dbg) == NULL) ||
		//////////////////////////////////////////////////////////////////////////////////////
		// GC TESTS
		(CU_add_test(gc_suite, "Simple GC use", test_gc) == NULL) ||
		(CU_add_test(gc_suite, "GC on scattered data on multiple pages", test_scattered) == NULL) ||
		(CU_add_test(gc_suite, "GC at threshold", test_threshold) == NULL) ||
		(CU_add_test(gc_suite, "GC with objects only accessible trough pointers", test_gc_pointers) == NULL) ||
		(CU_add_test(gc_suite, "GC on linked list with only head on stack", test_pointers_long) == NULL) ||
		(CU_add_test(gc_suite, "stack pointer updates to correct new adress after gc", test_stack_pointer) == NULL) ||
		(CU_add_test(gc_suite, "Delete heap and update stack pointers", test_delete_dbg) == NULL) ||
		(CU_add_test(gc_suite, "allocate beyond heap capacity", test_full_threshold) == NULL) ||
		(CU_add_test(gc_suite, "duplicating a string on the stack aswell as string on heap", test_str_dup) == NULL) ||
		(CU_add_test(gc_suite, "GC DBG", test_gc_dbg) == NULL) ||
		0)
	{
		// If adding any of the tests fails, we tear down CUnit and exit
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Set the running mode. Use CU_BRM_VERBOSE for maximum output.
	// Use CU_BRM_NORMAL to only print errors and a summary
	CU_basic_set_mode(CU_BRM_NORMAL);

	// This is where the tests are actually run!
	CU_basic_run_tests();

	// Tear down CUnit before exiting
	CU_cleanup_registry();
	return CU_get_error();
}