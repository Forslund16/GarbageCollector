#include "utils.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


bool not_empty(char *str)
{
    return strlen(str) > 0;
}

answer_t ask_question(char *question, check_func check, convert_func convert)
{
    int buf_size = 255;
    char buf[buf_size];
    while (true)
    {
        printf("%s", question);
        read_string(buf, buf_size);
        if (check(buf) && not_empty(buf))
        {
            break;
        }
    }
    answer_t result = convert(buf);
    return result;
}


void print(char *str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        putchar(str[i]);
        i++;
    }
}

void println(char *str)
{
    print(str);
    putchar('\n');
}

bool is_number(char *str)
{
    int len = strlen(str);

    if(str[len-1] == '\r')//if windows is used
    {
        str[len-1] = '\0';
        len -= 1;
    }

    
    
    int i = 0;
    /*
    if (str[0] == '-')
    {
        i++;
    }*/
    while (i < len)
    {
        if (isdigit(str[i]))
        {
            i += 1;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void clear_input_buffer()
{
    int c;
    do
    {
        c = getchar();
    }
    while (c != '\n' && c != EOF);
    return;
}

int ask_question_int(char *question)
{
    answer_t answer = ask_question(question, is_number, (convert_func) atoi);
    return answer.int_value; // svaret som ett heltal
}

bool valid_shelf(char *shelf)
{
    int len = strlen(shelf);

    if(shelf[len-1] == '\r')//if windows is used
    {
        shelf[len-1] = '\0';
        len -= 1;
    }

    if (len != 3) {return false;}
    if (isupper(shelf[0]))
    {
       for (int i = 1; shelf[i] != '\0'; i++)
       {
           if (is_number(&shelf[i])) {
               continue;
           }
           else
           {
               return false;
           }
       }
        return true;
    }
    return false;
}

char *ask_question_shelf(char *question, heap_t *heap)
{
    int buf_size = 255;
    char buf[buf_size];
    while (true)
    {
        printf("%s", question);
        read_string(buf, buf_size);
        if (valid_shelf(buf) && not_empty(buf))
        {
            break;
        }
    }
    char *result = h_str_dup(heap, buf);
    return result;
}

int read_string(char *buf, int buf_siz)
{
    int i = 0;
    int c;
    while (i < buf_siz - 1)
    {
        c = getchar();
        if (c != '\n')
        {
            buf[i] = c;
            i++;
        }
        else
        {
            buf[i] = '\0';
            return i;
        }
    }
    clear_input_buffer();
    return i;
}

char *ask_question_string(char *question, heap_t *heap)
{
    int buf_size = 255;
    char buf[buf_size];
    while (true)
    {
        printf("%s", question);
        read_string(buf, buf_size);
        if (not_empty(buf))
        {
            break;
        }
    }
    char *result = h_str_dup(heap, buf);
    return result;
}

char ask_question_menu(char *question, check_char check, print_func print_menu)
{
    print_menu();
    char c = ' ';
    while (!check(c))
    {
        printf("%s",question);
        c = getchar();
        clear_input_buffer();
    }
    return toupper(c);
}

