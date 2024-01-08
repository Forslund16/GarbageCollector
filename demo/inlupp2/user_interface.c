#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"
#include "utils.h"
#include "hash_table.h"
#include "common.h"
#include "warehouse.h"
#include "cart.h"
#include "ui_utils.h"

#define No_Prints 20

static ioopm_merch_t *input_merch(heap_t *heap)
{
    char *name = ask_question_string("Name of the item: ", heap);
    char *desc = ask_question_string("Description of the item: ", heap);
    int price = ask_question_int("Price of the item: ");
    ioopm_merch_t *merch = ioopm_merch_create(name, desc, price, NULL, heap);
    return merch;
}

void ioopm_add_merch_to_wh(ioopm_hash_table_t *ht_items, heap_t *heap)
{
    while (true)
    {
        ioopm_merch_t *new_merch = input_merch(heap);
        if (ioopm_hash_table_lookup(ht_items, (elem_t)new_merch->name) == NULL)
        {
            ioopm_backend_add(ht_items, new_merch->name, new_merch, heap);
            break;
        }
        printf("Merch already exists\n");
        new_merch->name = NULL;
        new_merch->description = NULL;
        new_merch = NULL;
        // free(new_merch->name);
        // free(new_merch->description);
        // free(new_merch);
    }
}

elem_t *ioopm_list_merch(ioopm_hash_table_t *ht_items, heap_t *heap)
{
    size_t size = ioopm_hash_table_size(ht_items);
    elem_t *keys_array = ioopm_keys_sort(ht_items, size, heap);

    for (size_t i = 1; i < size + 1; i++)
    {
        printf("%ld: %s\n", i, keys_array[i - 1].string_val);
        if ((i % No_Prints) == 0 && i != size)
        {
            char *cont = ask_question_string("Do you want to see more merch? (y/n) ", heap);
            if (strcmp(cont, "n") == 0 || strcmp(cont, "N") == 0)
            {
                // free(cont);
                cont = NULL;
                break;
            }
            cont = NULL;
            // free(cont);
        }
    }
    return (keys_array);
}

static char *ask_for_index(ioopm_hash_table_t *ht, elem_t *keys_array, char *question)
{
    int index;
    size_t size = ioopm_hash_table_size(ht);
    while (true)
    {
        index = ask_question_int(question);
        if (index > 0 && index <= size && ioopm_hash_table_lookup(ht, (elem_t)keys_array[index - 1].string_val))
        {
            break;
        }
        printf("Item not found\n");
    }
    return keys_array[index - 1].string_val;
}

void ioopm_edit_merch(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap)
{
    elem_t *keys_array = ioopm_list_merch(wh->ht_items, heap);

    char *name = ask_for_index(wh->ht_items, keys_array, "What merch do you want to edit? ");
    keys_array = NULL;
    if (ioopm_in_cart(wh, shop, name, heap))
    {
        printf("Merch is currently in a cart, wait for checkout\n");
        name = NULL;
        // free(keys_array);
        return;
    }
    char *new_name = NULL;
    while (true)
    {
        new_name = ask_question_string("What do you want to change the name to? ", heap);
        if (!ioopm_hash_table_lookup(wh->ht_items, (elem_t)new_name))
        {
            break;
        }
        printf("Merch already exists\n");
        new_name = NULL;
        // free(new_name);
    }
    char *new_desc = ask_question_string("What do you want to change the description to? ", heap);
    int new_price = ask_question_int("What do you want to change the price to? ");

    ioopm_backend_edit(wh, name, new_name, new_desc, new_price, heap);
    name = NULL;
    // free(keys_array);
}

void ioopm_replenish_merch(ioopm_warehouse_t *wh, heap_t *heap)
{
    elem_t *keys_array = ioopm_list_merch(wh->ht_items, heap);

    char *name = ask_for_index(wh->ht_items, keys_array, "What merch do you want to replenish? ");

    int stock;
    char *shelf;
    while (true)
    {
        stock = ask_question_int("How much stock do you want to add? ");
        if (stock != 0)
        {
            break;
        }
    }
    while (true)
    {
        shelf = ask_question_shelf("Where should the merch be placed? ", heap);
        elem_t *name_in_ht = ioopm_hash_table_lookup(wh->ht_shelfs, (elem_t)shelf);

        if (name_in_ht == NULL)
        {
            break;
        }
        if (strcmp(name, name_in_ht->string_val) == 0)
        {
            break;
        }
        printf("Shelf already occupied\n");
        shelf = NULL;
        // free(shelf);
    }

    ioopm_backend_replenish(wh, name, shelf, stock, heap);
    name = NULL;
    keys_array = NULL;
    // free(keys_array);
}

void ioopm_show_stock(ioopm_warehouse_t *wh, heap_t *heap)
{
    elem_t *keys_array = ioopm_list_merch(wh->ht_items, heap);
    char *name = ask_for_index(wh->ht_items, keys_array, "What merch do you want to view the stock for? ");
    ioopm_backend_show_stock(wh->ht_items, name, heap);
    name = NULL;
    keys_array = NULL;
    // free(keys_array);
}

void ioopm_remove_merch(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap)
{
    elem_t *keys_array = ioopm_list_merch(wh->ht_items, heap);
    char *name = ask_for_index(wh->ht_items, keys_array, "What merch do you want to remove? ");
    keys_array = NULL;
    if (ioopm_in_cart(wh, shop, name, heap))
    {
        printf("Merch is currently in a cart, wait for checkout\n");
        name = NULL;
        // free(keys_array);
        return;
    }
    ioopm_backend_remove(wh, name, heap);
    name = NULL;
    // free(keys_array);
}

static void print_wh_menu()
{
    fprintf(stdout, "[A]dd merch\n"
                    "[R]emove merch\n"
                    "[E]dit merch\n"
                    "[I]ncrease stock\n"
                    "[S]how stock\n"
                    "[U]ndo\n"
                    "[L]ist merch\n"
                    "[Q]uit\n");
}

static bool valid_wh_menu(char c)
{
    char corr[17] = {'L', 'l', 'Q', 'q', 'R', 'r', 'U', 'u', 'E', 'e', 'A', 'a', 's', 'S', 'i', 'I'};
    for (int i = 0; i < 17; i++)
    {
        if (corr[i] == c)
        {
            return true;
        }
    }
    return false;
}

static void event_loop_wh(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap)
{

    while (true)
    {
        printf("Total avail: %ld\n", h_avail(heap));

        size_t size = ioopm_hash_table_size(wh->ht_items);
        char answer = ask_question_menu("Action: ", valid_wh_menu, print_wh_menu);
        if (answer == 'A')
        {
            ioopm_add_merch_to_wh(wh->ht_items, heap);
        }
        else if (answer == 'Q')
        {
            break;
        }
        else if (answer == 'U')
        {
            printf("Not yet implemented\n");
        }
        else if (size == 0)
        {
            printf("No merch in database\n");
            continue;
        }
        else if (answer == 'L')
        {
            elem_t *arr = ioopm_list_merch(wh->ht_items, heap);
            arr = NULL;
            // free(arr);
        }
        else if (answer == 'R')
        {
            ioopm_remove_merch(wh, shop, heap);
        }
        else if (answer == 'I')
        {
            ioopm_replenish_merch(wh, heap);
        }
        else if (answer == 'E')
        {
            ioopm_edit_merch(wh, shop, heap);
        }
        else if (answer == 'S')
        {
            ioopm_show_stock(wh, heap);
        }
    }
}

//-----------------------CART-----------------------------------

void ioopm_cart_create(ioopm_shop_t *shop, heap_t *heap)
{
    size_t size = ioopm_hash_table_size(shop->ht_carts);

    char *name = NULL;
    while (true)
    {
        name = ask_question_string("Enter your username: ", heap);
        if (ioopm_has_username(shop, name, heap) == true && size != 0)
        {
            printf("Username already taken\n");
            name = NULL;
            // free(name);
            continue;
        }
        break;
    }
    shop->num_of_carts++;
    char str[(int)((shop->num_of_carts / 10) + 2)];
    sprintf(str, "%d", shop->num_of_carts);
    char *id = h_str_dup(heap, str);
    ioopm_cart_t *cart = ioopm_backend_cart_create(shop, name, id, heap);
}

static int cmpstringint(const void *p1, const void *p2) // taken from inlupp1
{
    int i1 = atoi(*(char *const *)p1);
    int i2 = atoi(*(char *const *)p2);

    if (i1 == i2)
    {
        return 0;
    }
    if (i1 > i2)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}
bool ioopm_list_carts(ioopm_shop_t *shop, heap_t *heap)
{
    size_t size = ioopm_hash_table_size(shop->ht_carts);
    if (size == 0)
    {
        printf("No active carts\n");
        return false;
    }

    ioopm_list_t *list = ioopm_hash_table_keys(shop->ht_carts, heap);
    elem_t *keys_array = ioopm_linked_list_to_array(list, heap);

    qsort(keys_array, size, sizeof(char *), cmpstringint);
    ioopm_linked_list_destroy(list);
    list = NULL;

    for (int i = 0; i < size; i++)
    {
        // NOTE: After x number of GC and lots of carts the ID prints as null
        printf("%s. %s \n", keys_array[i].string_val, ioopm_hash_table_lookup(shop->ht_carts, keys_array[i])->cart->id);
    }
    keys_array = NULL;

    return true;
}

char *ask_for_cart(ioopm_shop_t *shop, char *question, heap_t *heap)
{
    char *index = NULL;
    while (true)
    {
        index = ask_question_string(question, heap);

        if (ioopm_hash_table_lookup(shop->ht_carts, (elem_t)index))
        {
            break;
        }

        printf("No cart with that index, try again\n");
    }
    return index;
}

void ioopm_cart_remove(ioopm_shop_t *shop, ioopm_warehouse_t *wh, heap_t *heap)
{
    if (!ioopm_list_carts(shop, heap))
    {
        return;
    }
    char *index = ask_for_cart(shop, "Which cart do you want to remove?", heap);
    ioopm_remove_reserved_in_cart(wh, shop, index);
    ioopm_backend_cart_remove(shop, index);
}

static void pick_merch(ioopm_cart_t *cart, ioopm_warehouse_t *wh, heap_t *heap)
{
    elem_t *merch_array = ioopm_list_merch(wh->ht_items, heap);
    char *merch_name = ask_for_index(wh->ht_items, merch_array, "What merch do you want to purchase? ");
    int merch_amount;

    if (ioopm_has_stock(wh, merch_name) == false)
    {
        printf("Merch out of stock\n");
        merch_array = NULL;
        // free(merch_array);
        return;
    }

    while (true)
    {
        merch_amount = ask_question_int("How many do you want to purchase? ");
        if (ioopm_check_stock(wh, merch_name, merch_amount))
        {
            if (merch_amount != 0)
            {
                break;
            }
            printf("You can't buy 0 items!\n");
        }
    }
    int *merch_amount_ptr = h_alloc_data(heap, sizeof(int));
    *merch_amount_ptr = merch_amount;
    ioopm_backend_cart_add(wh, cart, merch_name, merch_amount_ptr, heap);
    merch_array = NULL;
    // free(merch_array);
}

void ioopm_add_merch_to_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap)
{
    if (!ioopm_list_carts(shop, heap))
    {
        return;
    }
    char *index = ask_for_cart(shop, "Which cart do you want to add merch to? ", heap);
    ioopm_cart_t *cart = ioopm_hash_table_lookup(shop->ht_carts, (elem_t)index)->cart;

    if (ioopm_hash_table_size(wh->ht_items) == 0)
    {
        printf("No merch in stock\n");
        return;
    }
    pick_merch(cart, wh, heap);
}

static void pick_item_to_remove(ioopm_cart_t *cart, ioopm_warehouse_t *wh, heap_t *heap)
{
    elem_t *merch_array = ioopm_list_merch(cart->ht, heap);
    char *merch_name = ask_for_index(cart->ht, merch_array, "What merch do you want to remove? ");
    int merch_amount = ioopm_hash_table_lookup(cart->ht, (elem_t)merch_name)->int_val;
    int remove_amount;
    while (true)
    {

        remove_amount = ask_question_int("How many do you want to remove? ");
        if (remove_amount <= merch_amount)
        {
            break;
        }
        printf("Invalid input. You have %d %s in cart\n", merch_amount, merch_name);
    }

    ioopm_backend_remove_from_cart(wh, cart, merch_name, remove_amount, heap);
    merch_array = NULL;
    // free(merch_array);
}

void ioopm_remove_from_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap)
{
    if (ioopm_list_carts(shop, heap))
    {
        char *index = ask_for_cart(shop, "Which cart do you want to remove merch from? ", heap);
        ioopm_cart_t *cart = ioopm_hash_table_lookup(shop->ht_carts, (elem_t)index)->cart;

        if (ioopm_hash_table_size(cart->ht) == 0)
        {
            printf("No merch in cart\n");
            return;
        }
        pick_item_to_remove(cart, wh, heap);
    }
}

static void print_receipt(ioopm_warehouse_t *wh, ioopm_cart_t *cart, heap_t *heap)
{
    size_t size = ioopm_hash_table_size(cart->ht);
    elem_t *sorted_list = ioopm_keys_sort(cart->ht, size, heap);
    printf("\n \tReceipt for %s\n", cart->id);
    printf("------------------------------------\n");
    int merch_amount;
    char *merch_name;
    int merch_price;
    for (int i = 0; i < size; i++)
    {
        merch_name = sorted_list[i].string_val;
        merch_amount = *ioopm_hash_table_lookup(cart->ht, (elem_t)merch_name)->int_ptr;
        merch_price = ioopm_hash_table_lookup(wh->ht_items, (elem_t)merch_name)->merch->price;
        printf("%s: %d * ", merch_name, merch_amount);
        print_price(merch_price);
    }
    printf("------------------------------------\n");

    printf("Total: ");
    print_price(cart->total_price);
    printf("(You saved 420.69 kr)\nThanks for shopping at W&G INC\n\n");
    sorted_list = NULL;
    // free(sorted_list);
}

void print_price(int a)
{
    if ((a % 100) > 9)
    {
        printf("%d.%d kr\n", a / 100, a % 100);
    }
    else
    {
        printf("%d.0%d kr\n", a / 100, a % 100);
    }
}
void ioopm_checkout_cart(ioopm_warehouse_t *wh, ioopm_shop_t *shop, heap_t *heap)
{
    if (!ioopm_list_carts(shop, heap))

    {
        return;
    }
    char *index = ask_for_cart(shop, "Which cart do you want to checkout? ", heap);
    ioopm_cart_t *cart = ioopm_hash_table_lookup(shop->ht_carts, (elem_t)index)->cart;

    if (ioopm_hash_table_size(cart->ht) == 0)
    {
        printf("No merch in cart\n");
        return;
    }

    print_receipt(wh, cart, heap);
    while (true)
    {
        char *confirm = ask_question_string("Are you sure you want to checkout? (y/n) ", heap);
        if (strcmp(confirm, "y") == 0 || (strcmp(confirm, "Y") == 0))
        {
            ioopm_backend_cart_checkout(wh, shop, index);
        }
        confirm = NULL;
        // free(confirm);
        break;
    }
}

static void print_cart_menu()
{
    fprintf(stdout, "[A]dd cart\n"
                    "[R]emove cart\n"
                    "[B]rowse merch\n"
                    "[E]dit cart\n"
                    "[L]ist carts\n"
                    "[C]heckout\n"
                    "[Q]uit\n");
}

static bool valid_cart_menu(char c)
{
    char corr[14] = {'L', 'l', 'Q', 'q', 'R', 'r', 'B', 'b', 'A', 'a', 'C', 'c', 'E', 'e'};
    for (int i = 0; i < 14; i++)
    {
        if (corr[i] == c)
        {
            return true;
        }
    }
    return false;
}

static void event_loop_cart(ioopm_shop_t *shop, ioopm_warehouse_t *wh, heap_t *heap)
{

    while (true)
    {
        printf("Total avail: %ld\n", h_avail(heap));

        char answer = ask_question_menu("Action: ", valid_cart_menu, print_cart_menu);
        if (answer == 'A')
        {
            ioopm_cart_create(shop, heap);
        }
        else if (answer == 'L')
        {
            ioopm_list_carts(shop, heap);
        }
        else if (answer == 'R')
        {
            ioopm_cart_remove(shop, wh, heap);
        }
        else if (answer == 'B')
        {
            ioopm_add_merch_to_cart(wh, shop, heap);
        }
        else if (answer == 'Q')
        {
            break;
        }
        else if (answer == 'E')
        {
            ioopm_remove_from_cart(wh, shop, heap);
        }
        else if (answer == 'C')
        {
            ioopm_checkout_cart(wh, shop, heap);
        }
    }
}

static void print_main_menu()
{
    fprintf(stdout, "[W]arehouse \n"
                    "[S]hop\n"
                    "[Q]uit\n");
}

static bool valid_main_menu(char c)
{
    char corr[8] = {'Q', 'q', 'S', 's', 'W', 'w', 'G', 'g'};
    for (int i = 0; i < 8; i++)
    {
        if (corr[i] == c)
        {
            return true;
        }
    }
    return false;
}

static void main_event_loop(ioopm_shop_t *shop, ioopm_warehouse_t *wh, heap_t *heap)
{
    while (true)
    {
        char answer = ask_question_menu("Action: ", valid_main_menu, print_main_menu);
        if (answer == 'W')
        {
            event_loop_wh(wh, shop, heap);
        }
        if (answer == 'S')
        {
            event_loop_cart(shop, wh, heap);
        }
        if (answer == 'Q')
        {
            break;
        }
        if (answer == 'G')
        {
            h_gc(heap);
        }
    }
}
int main()
{
    heap_t *heap = h_init(20000, false, 0.4);
    ioopm_warehouse_t *wh = ioopm_warehouse_create(heap);
    ioopm_shop_t *shop = ioopm_shop_create(heap);

    main_event_loop(shop, wh, heap);

    // ioopm_shop_destroy(shop);
    // ioopm_warehouse_destroy(wh);
    h_delete(heap);
    return 0;
}
