#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include "../../src/gc.h"

/**
 * @file utils.h
 * @author William Forslund, Marcus Nyfjäll
 * @date 30 sep 2022
 * @brief Utility functions for user input
 */

typedef union {
    int   int_value;
    float float_value;
    char *string_value;
    char char_value;
} answer_t;

typedef bool(*check_func)(char *);
typedef answer_t (*convert_func)(char *);
typedef bool(*check_char)(char);
typedef void(*print_func)(void);

extern char *strdup(const char *);


/// @brief Ask user for string and checks if it is not empty
/// @param question the question being asked to the user
/// @return a correct string from the user
char *ask_question_string(char *question, heap_t *heap);

/// @brief Ask user for int and checks if it is an int
/// @param question the question being asked to the user
/// @return a correct int corresponding to the input as an int
int ask_question_int(char *question);

/// @brief Ask user for shelf and checks if it is correct
/// @param question the question being asked to the user
/// @return a correct string corresponding to the input as a shelf
char *ask_question_shelf(char *question, heap_t *heap);

/// @brief Ask user for char and checks if it is in menu
/// @param question The question to be asked
/// @param check a check function(in this case seeing if a char is in the menu)
/// @param print prints the correct menu
/// @return The upper case letter corresponding to the input, if it is in menu
char ask_question_menu(char *question, check_char check, print_func print);


#endif