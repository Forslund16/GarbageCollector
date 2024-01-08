#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "heap.h"
#include "stack.h"

/// @brief Interprets a char and calulates size of the object its representing
/// @param format_char a char mapping to a certain type
/// @return size of object mapping to format_char
size_t parse_format_char(char format_char);

/// @brief Interprets a string and calulates size of the object its representing
/// @param format_string a string wiht chars mapping to certain types
/// @return size of object mapping to all chars in format_string
size_t parse_format_string(char *format_string);

/// @brief Calculates what the unfolded layout length should be of a compact layout
/// @param layout the compact layout
/// @return unfoleded layout length as an int
int calculate_layout_length(char *layout);

/// @brief Unfolds a compact layout by doing character duplication according to the compact layout
/// @param layout the compact layout
/// @param string_to_overwrite the location to save the unfolded layout
void unfold_compact_format_string(char *layout, char *string_to_overwrite);