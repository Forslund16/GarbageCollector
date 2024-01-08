#include "../src/gc.h"
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct test_link link_t;
typedef struct test_list list_t;

struct test_link
{
    long value;
    link_t *next_ptr;
};

struct test_list
{
    link_t *first;
    link_t *last;
};

////////////////////////////////////    TEST 1    //////////////////////////////////////////////////////////

clock_t test1_h_alloc()
{
    heap_t *heap = h_init(50000000, true, 1.0);
    clock_t start = clock();

    for (int i = 0; i < 25000; i++)
    {
        h_alloc_data(heap, 2016);
    }

    clock_t stop = clock();
    return (stop - start);
}

clock_t test1_malloc()
{
    clock_t start = clock();

    // for (int i = 0; i < 625000; i++) {malloc(16);}

    for (int i = 0; i < 25000; i++)
    {
        malloc(2016);
    }

    clock_t stop = clock();
    return (stop - start);
}

// Measure h_alloc vs malloc on allocating 625000 * 16 = 10MB worth of memory. (OBS! h_alloc uses 3 times as much)
void measure_alloc()
{
    printf("\n --TEST 1--\n");
    clock_t time_h_alloc = test1_h_alloc();
    clock_t time_malloc = test1_malloc();

    printf("\nTIME h_alloc: %f\nTIME malloc:  %f\n", (double)(time_h_alloc) / CLOCKS_PER_SEC, (double)(time_malloc) / CLOCKS_PER_SEC);
}

////////////////////////////////////    TEST 2    ///////////////////////////////////////////////////////////

clock_t test2_h_alloc()
{
    heap_t *heap = h_init(204800, true, 0.7);
    clock_t start = clock();

    for (int i = 0; i < 625000; i++)
    {
        h_alloc_data(heap, 16);
    }

    h_gc(heap);
    clock_t stop = clock();
    return (stop - start);
}

clock_t test2_malloc()
{
    clock_t start = clock();

    for (int i = 0; i < 625000; i++)
    {
        void *ptr = malloc(16);
        free(ptr);
    }

    clock_t stop = clock();
    return (stop - start);
}

void measure_gc()
{
    printf("\n --TEST 2--\n");
    clock_t time_h_alloc = test2_h_alloc();
    clock_t time_malloc = test2_malloc();

    // printf("\nTIME h_alloc: %ld\nTIME malloc: %ld", time_h_alloc, time_malloc);
    printf("\nTIME h_alloc: %f\nTIME malloc:  %f\n", (double)(time_h_alloc) / CLOCKS_PER_SEC, (double)(time_malloc) / CLOCKS_PER_SEC);
}

/////////////////////////////////   TEST 3   //////////////////////////////////////////////////////////////

int search(list_t *list, int random)
{
    if (list->first == NULL || list->first->next_ptr == NULL)
    {
        return 0;
    }

    link_t *cursor = list->first->next_ptr;
    while (cursor != list->last)
    {
        if (cursor->value == random)
        {
            return 1;
        }
        cursor = cursor->next_ptr;
    }
    return 0;
}

clock_t test3_h_alloc(int M, int N)
{

    heap_t *heap = h_init(50000000, true, 0.8); // 50MB
    clock_t start1 = clock();

    list_t *list1 = h_alloc_struct(heap, "**");
    list_t *list2 = h_alloc_struct(heap, "**");
    list_t *list3 = h_alloc_struct(heap, "**");
    list_t *list4 = h_alloc_struct(heap, "**");
    list1->first = h_alloc_struct(heap, "l*"), list1->last = list1->first;
    list2->first = h_alloc_struct(heap, "l*"), list2->last = list2->first;
    list3->first = h_alloc_struct(heap, "l*"), list3->last = list3->first;
    list4->first = h_alloc_struct(heap, "l*"), list4->last = list4->first;

    for (int i = 0; i < M; i++)
    {
        int random = rand() % 4000000000;
        link_t *new_link = h_alloc_struct(heap, "l*");
        new_link->value = random;
        // new_link -> next_ptr = NULL;

        if (random < 1000000000)
        {
            list1->last->next_ptr = new_link;
            list1->last = new_link;
        }
        else if (1000000000 <= random && random < 2000000000)
        {
            list2->last->next_ptr = new_link;
            list2->last = new_link;
        }
        else if (2000000000 <= random && random < 3000000000)
        {
            list3->last->next_ptr = new_link;
            list3->last = new_link;
        }
        else
        {
            list4->last->next_ptr = new_link;
            list4->last = new_link;
        }
    }

    h_gc(heap); // fungerar ej för M > 70000 då vi får stack overflow
    clock_t start2 = clock();
    int count = 0;
    for (int i = 0; i < N; i++)
    {
        int random = rand() % 4000000000;
        if (random < 1000000000)
        {
            count += search(list1, random);
        }
        else if (1000000000 <= random && random < 2000000000)
        {
            count += search(list2, random);
        }
        else if (2000000000 <= random && random < 3000000000)
        {
            count += search(list3, random);
        }
        else
        {
            count += search(list4, random);
        }
    }
    //printf("\nCOUNT h_alloc: %d\n", count);
    clock_t stop = clock();
    printf("SEARCH TIME h_alloc:  %f s\n", (double)(stop - start2) / CLOCKS_PER_SEC);
    return (stop - start1);
}

clock_t test3_malloc(int M, int N)
{
    clock_t start1 = clock();

    list_t *list1 = malloc(sizeof(list_t));
    list_t *list2 = malloc(sizeof(list_t));
    list_t *list3 = malloc(sizeof(list_t));
    list_t *list4 = malloc(sizeof(list_t));
    list1->first = malloc(sizeof(link_t)), list1->last = list1->first;
    list2->first = malloc(sizeof(link_t)), list2->last = list2->first;
    list3->first = malloc(sizeof(link_t)), list3->last = list3->first;
    list4->first = malloc(sizeof(link_t)), list4->last = list4->first;

    for (int i = 0; i < M; i++)
    {
        int random = rand() % 4000000000;
        link_t *new_link = malloc(sizeof(link_t));
        new_link->value = random;

        if (random < 1000000000)
        {
            list1->last->next_ptr = new_link;
            list1->last = new_link;
        }
        else if (1000000000 <= random && random < 2000000000)
        {
            list2->last->next_ptr = new_link;
            list2->last = new_link;
        }
        else if (2000000000 <= random && random < 3000000000)
        {
            list3->last->next_ptr = new_link;
            list3->last = new_link;
        }
        else
        {
            list4->last->next_ptr = new_link;
            list4->last = new_link;
        }
    }

    clock_t start2 = clock();
    int count = 0;
    for (int i = 0; i <= N; i++)
    {
        int random = rand() % 4000000000;
        if (random < 1000000000)
        {
            count += search(list1, random);
        }
        else if (1000000000 <= random && random < 2000000000)
        {
            count += search(list2, random);
        }
        else if (2000000000 <= random && random < 3000000000)
        {
            count += search(list3, random);
        }
        else
        {
            count += search(list4, random);
        }
    }
    //printf("\nCOUNT malloc: %d\n", count);
    clock_t stop = clock();
    printf("SEARCH TIME malloc:  %f s\n", (double)(stop - start2) / CLOCKS_PER_SEC);
    return (stop - start1);
}

void measure_linked_list(int M, int N)
{
    srand((unsigned int)(clock())); // set seed for rand()
    printf("\n --TEST 3--\n");

    clock_t time_h_alloc = test3_h_alloc(M, N);
    clock_t time_malloc = test3_malloc(M, N);

    printf("\nTOTAL TIME h_alloc: %f\nTOTAL TIME malloc:  %f\n", (double)(time_h_alloc) / CLOCKS_PER_SEC, (double)(time_malloc) / CLOCKS_PER_SEC);
}

///////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    measure_alloc();
    measure_gc();
    measure_linked_list(70000, 10000);
    return 0;
}