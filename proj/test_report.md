# Test rapport
Se sektionen __Kvalitetssäkring__ i projektrapporten för utförligare information om kvalitetsäkringen i sin helhet.

## Enhetstestning
Enhetstester har skrivits i samband med utvecklingen av de två moduler som projektet består av, heap.h och stack.h. Testern är ämnade att testa enskilda funktioner och bekräfta att de utför det dom är tänkta att göra. 

## Integrationstestning
Integrationstester har skrivits för själva skräpsamlaren gc.h, i vilken heap.h och stack.h interagerar med varandra genom implementation av funktioner för skräpsamlingsalgoritmen.

## Regressionstestning
Samlingen av integrationstester samt enhetstester körs i samband med pull requests för att få någon idé om det jobb man gjort i en viss branch är vettigt. 

#### Några exempel:

### void test_alloc_raw()
	void test_alloc_raw() 
	{
		heap_t *heap = h_init(4096, true, 90);
		void *pointer = h_alloc_data(heap, 4);
		CU_ASSERT_PTR_NOT_NULL(pointer);

		int *num = h_alloc_data(heap, sizeof(int));
		*num = 42;
		CU_ASSERT_EQUAL(*num, 42);

		h_delete(heap);
	}

Detta testar att rå allokering, det vill säga allokering av en mängd bytes som inte nödvändigtvis är associerat med en struct eller datatyp fungerar som det ska.

### void test_alloc_invalid_input()
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

Detta testar att felaktig allokeringar beter sig förutsägbart.

### void test_page_activation()
	void test_page_activation() 
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

Detta testar att pages i heapen aktiveras vid allokering och avaktiveras på ett korrekt sätt när inga levande pekare längre existerar till en page.

### void test_used_memory()
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

Detta testar att funktionerna för att ge använt minne stämmer överrens med hur mycket man allokerat, att rätt antal sidor är aktiverade samt att felaktig allokering inte tar upp utrymme.

### void test_find_multiple()
	void test_find_multiple()
	{
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

Detta testar att allokeringar med levande referenser hittas av vår stack crawler, att antalet hittade pekare är rätt, samt att perkarna pekar ut rtätt objekt på heapen.

## Bygga och köra tester
Alla tester har skrivits under projektets gång parallellt med utvecklingsprocessen.
Alla tester är automatiserade med hjälp av makefile.

make memtest: Bygger och kör testfilen i test mappen som innehåller både enhetstester för 
heap och stack modulerna samt integrationstester för dessa

make all_memtest: Bygger och kör enhetstester för heap och stack modulerna som finns i src mappen

## Code coverage
![ioopm_project_code_coverage](https://user-images.githubusercontent.com/80210925/211007559-c63dc030-2de0-4ff6-9d19-b15c13d90cbf.png)

## Länk till 6 github issues

BUG: length of char *str is not sizeof(str) / sizeof(char) like all the other arrays
https://github.com/IOOPM-UU/Omegalul/issues/3

Håriga stack pointer grejer
https://github.com/IOOPM-UU/Omegalul/issues/8

BUG: When dereferencing a pointer you don't get to the first internal object, we have to add the offset which is the size of the object
https://github.com/IOOPM-UU/Omegalul/issues/11

BUG: didn't check for when forwarding_adress is NULL
https://github.com/IOOPM-UU/Omegalul/issues/13

BUG: Important to initialize sp before calling on find_stack_pointers, otherwise it will get taken there and we will have 2 variables with the same address which lead to change of sp as a side-effect
https://github.com/IOOPM-UU/Omegalul/issues/20

BUG: segfault when creating a lot of carts
https://github.com/IOOPM-UU/Omegalul/issues/23
