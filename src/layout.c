#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

// Maps char to its type size
size_t parse_format_char(char format_char)
{
    char c = format_char;
    if (c == '*')
        return sizeof(void *);
    else if (c == 'i')
        return sizeof(int);
    else if (c == 'c')
        return sizeof(char);
    else if (c == 'l')
        return sizeof(long);
    else if (c == 'f')
        return sizeof(float);
    else if (c == 'd')
        return sizeof(double);
    else
        return 0;
}



int calculate_layout_length(char *layout)
{
    int size = 0;
    int count = 0;
    int read = 0;
    for(int i = 0; i < strlen(layout); i++){
        char c = layout[i];
        if(c >= '1' && c <= '9')
        {
            count++;
        }
        else
        {
            int num = 0;
            for(int j = 0; j < count; j++){
                num += ((int)layout[read + j]-48) * pow(10,(count - j)-1);
            }
            if(num == 0){
                num = 1;
            }
            size += num;
            read += count + 1;
            count = 0;
        }
    }

    if(count == strlen(layout)){
        int num = 0;
        for(int j = 0; j < count; j++){
            num += ((int)layout[read + j]-48) * pow(10,(count - j)-1);
        }
        size = num;
    }
    return size;
}

void character_duplicate(char c, int num, char *new_string)
{
    for(int i = 0; i < num; i++){
        new_string[i] = c;
    }
}

void unfold_compact_format_string(char *layout, char *string_to_overwrite)
{
    int length = strlen(layout);
    int count = 0;
    int cursor = 0;
    int read = 0;
    int size = calculate_layout_length(layout);
    bool need_to_convert = false;
    for(int i = 0; i < length; i++){

        char c = layout[i];
        if(c >= '1' && c <= '9')
        {
            need_to_convert = true;
            count++;
        }
        else if(need_to_convert)
        {
            int num = 0;
            for(int j = 0; j < count; j++){
                num += ((int)layout[read + j]-48) * pow(10,(count - j)-1);
            }
            char str[num];
            character_duplicate(c, num, str);
            for(int k = cursor, l = 0; k < (num + cursor); k++, l++){
                string_to_overwrite[k] = str[l];
            }
            cursor += num;
            read += count + 1;
            count = 0;
            need_to_convert = false;
        }
        else
        {
            string_to_overwrite[cursor] = layout[i];
            cursor++;
            read++;
        }

    }

    if(count == length){
        int num = 0;
        for(int j = 0; j < count; j++){
            num += ((int)layout[read + j]-48) * pow(10,(count - j)-1);
        }
        for(int i = 0; i < num; i++){
            string_to_overwrite[i] = 'c';
        }
    }

    string_to_overwrite[size] = '\0';
}

// Returns 0 if format string is not supported
size_t parse_format_string(char *format_string)
{
    if(format_string == NULL)
    {
        return 0;
    }
    
    char unfolded_format_string[calculate_layout_length(format_string)];
    unfold_compact_format_string(format_string, unfolded_format_string);
    
    int length = strlen(unfolded_format_string);
    size_t size = 0;
    for (int i = 0; i < length; i++)
    {
        size_t value = parse_format_char(unfolded_format_string[i]);
        if (value == 0)
        {
            return 0;
        }
        size += value;
    }

    return size;
}